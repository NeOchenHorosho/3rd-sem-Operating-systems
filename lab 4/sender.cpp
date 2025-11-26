#include "common.h"

using std::cerr;
using std::cin;
using std::cout;
using std::fstream;
using std::string;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Usage: ./Sender <filename>\n";
        return EXIT_FAILURE;
    }

    string filename = argv[1];

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        cerr << "Sender: shm_open failed\n";
        return EXIT_FAILURE;
    }

    Info *info = (Info *)mmap(NULL, sizeof(Info), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (info == MAP_FAILED) {
        cerr << "Sender: mmap failed\n";
        return EXIT_FAILURE;
    }

    sem_t *free_sem = sem_open(free_recs_sem_name, 0);
    sem_t *full_sem = sem_open(full_recs_sem_name, 0);
    if (free_sem == SEM_FAILED || full_sem == SEM_FAILED) {
        cerr << "Sender: sem_open failed\n";
        return EXIT_FAILURE;
    }

    fstream binary_file;
    binary_file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!binary_file.is_open()) {
        cerr << "Sender: file open failed\n";
        return EXIT_FAILURE;
    }

    pthread_barrier_wait(&info->init_barrier);

    while (true) {
        if (info->stop_flag) break;

        cout << "\n1. Send message\n0. Exit\n";
        int choice;
        if (choice == 0) break;

        if (choice == 1) {
            cout << "Enter message (max " << MAX_MESSAGE_SIZE << " chars): ";
            string input_str;
            cin >> input_str; 

            if (sem_wait(free_sem) == -1) {
                if (info->stop_flag) break; 
                cerr << "Wait failed\n";
                break;
            }

            if (info->stop_flag) break;

            pthread_mutex_lock(&info->shm_mutex);

            Message msg;
            strncpy(msg.message, input_str.c_str(), MAX_MESSAGE_SIZE);
            msg.message[MAX_MESSAGE_SIZE] = '\0'; 

            binary_file.seekp(info->tail * sizeof(Message), std::ios::beg);
            binary_file.write((char*)&msg, sizeof(Message));
            binary_file.flush(); 

            cout << "Sent to record " << info->tail << ": " << msg.message << '\n';

            info->tail = (info->tail + 1) % info->records_num;

            pthread_mutex_unlock(&info->shm_mutex);

            sem_post(full_sem);
        }
    }

    munmap(info, sizeof(Info));
    sem_close(free_sem);
    sem_close(full_sem);
    binary_file.close();

    return 0;
}