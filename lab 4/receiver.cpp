#include "common.h"

using std::cerr;
using std::cin;
using std::cout;
using std::fstream;
using std::string;

int main()
{
    cout << "Enter filename\n";
    string filename_cpp;
    fstream binary_file;
    binary_file.open(filename_cpp, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    cin >> filename_cpp;
    cout << "Enter number of records\n";
    int records_num;
    cin >> records_num;

    if (records_num <= 0 || records_num > MAX_RECORD_NUM)
    {
        cerr << "Invalid number of records\n";
        return EXIT_FAILURE;
    }
    if (!binary_file.is_open())
    {
        cerr << "Couldn't open the file\n";
        return EXIT_FAILURE;
    }
    Message empty_msg{};
    for (int i = 0; i < records_num; ++i)
    {
        binary_file.write((char *)&empty_msg, sizeof(Message));
    }


    cout << "Enter number of senders:\n";
    int senders_num;
    cin >> senders_num;

    if (senders_num <= 0 || senders_num > MAX_SENDERS_NUM)
    {
        cerr << "Invalid number of senders\n";
        return EXIT_FAILURE;
    }

    shm_unlink(shm_name);
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        cerr << "shm_open failed";
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(Info)) == -1)
    {
        cerr << "ftruncate failed";
        return EXIT_FAILURE;
    }

    Info *info = (Info *)mmap(NULL, sizeof(Info), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (info == MAP_FAILED)
    {
        cerr << "mmap failed";
        return EXIT_FAILURE;
    }

    info->records_num = records_num;
    info->head = 0;
    info->tail = 0;
    info->stop_flag = false; 

    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&info->shm_mutex, &m_attr);
    pthread_mutexattr_destroy(&m_attr);

    pthread_barrierattr_t b_attr;
    pthread_barrierattr_init(&b_attr);
    pthread_barrierattr_setpshared(&b_attr, PTHREAD_PROCESS_SHARED);
    pthread_barrier_init(&info->init_barrier, &b_attr, senders_num + 1);
    pthread_barrierattr_destroy(&b_attr);

    sem_unlink(free_recs_sem_name);
    sem_unlink(full_recs_sem_name);

    sem_t *free_sem = sem_open(free_recs_sem_name, O_CREAT, 0666, records_num);
    sem_t *full_sem = sem_open(full_recs_sem_name, O_CREAT, 0666, 0);

    if (free_sem == SEM_FAILED || full_sem == SEM_FAILED)
    {
        cerr << "sem_open failed";
        return EXIT_FAILURE;
    }

    std::vector<pid_t> senders_pids(senders_num); 
    extern char **environ;

    char *fname_arg = const_cast<char *>(filename_cpp.c_str());
    char *const argv[] = {(char *)"kitty", (char *)"./Sender", fname_arg, NULL};

    for (int i = 0; i < senders_num; i++)
    {
        if (posix_spawnp(&senders_pids[i], "kitty", NULL, NULL, argv, environ))
        {
            cerr << "Couldn't create process " << i << '\n';
            return EXIT_FAILURE;
        }
    }

    pthread_barrier_wait(&info->init_barrier);
    while (true)
    {
        cout << "\n1. Read message\n0. Exit\n> ";
        int choice;
        cin >> choice;

        if (choice == 0)
        {
            info->stop_flag = true;
            for (int i = 0; i < senders_num; ++i)
                sem_post(free_sem);
            break;
        }

        if (choice == 1)
        {
            cout << "Waiting for message\n";
            if (sem_wait(full_sem) == -1)
            {
                cerr << "Wait failed\n";
                break;
            }

            pthread_mutex_lock(&info->shm_mutex);

            binary_file.seekg(info->head * sizeof(Message), std::ios::beg);

            Message msg;
            binary_file.read((char *)&msg, sizeof(Message));

            cout << "Received: " << msg.message << '\n';

            info->head = (info->head + 1) % info->records_num;

            pthread_mutex_unlock(&info->shm_mutex);

            sem_post(free_sem);
        }
    }

    munmap(info, sizeof(Info));
    shm_unlink(shm_name);
    sem_close(free_sem);
    sem_close(full_sem);
    sem_unlink(free_recs_sem_name);
    sem_unlink(full_recs_sem_name);
    binary_file.close();
    return 0;
}