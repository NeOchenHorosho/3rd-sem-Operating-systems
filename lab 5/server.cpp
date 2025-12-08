#include "common.h"
#include <spawn.h>
#include <thread>
const int MAX_CLIENTS_NUM = 10;
const int MAX_RECORDS_NUM = 100;

void client_handler_thread(int client_number)
{
    string client_num_str = std::to_string(client_number);
    string request_fifo_path = request_fifo_prefix + client_num_str;
    string response_fifo_path = response_fifo_prefix + client_num_str;

    int response_fifo_fd = open(response_fifo_path.c_str(), O_WRONLY);
    int request_fifo_fd = open(request_fifo_path.c_str(), O_RDONLY);


}

int main()
{
    cout << "Enter filename\n";
    string filename_cpp;
    fstream binary_file;
    cin >> filename_cpp;
    binary_file.open(filename_cpp, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    cout << "Enter number of records\n";
    int records_num;
    cin >> records_num;

    if (!is_valid_int(records_num, 1, MAX_RECORDS_NUM))
    {
        cerr << "Invalid number of records\n";
        return EXIT_FAILURE;
    }
    if (!binary_file.is_open())
    {
        cerr << "Couldn't open the file\n";
        return EXIT_FAILURE;
    }
    Employee empty_employee{};
    for (int i = 0; i < records_num; ++i)
    {
        binary_file.write((char *)&empty_employee, sizeof(Employee));
    }
    binary_file.flush();

    cout << "Enter number of clients:\n";
    int clients_num;
    cin >> clients_num;

    if (clients_num <= 0 || clients_num > MAX_CLIENTS_NUM)
    {
        cerr << "Invalid number of clients\n";
        return EXIT_FAILURE;
    }

    std::vector<pid_t> clients_pids(clients_num); 
    extern char **environ;
    std::vector<std::thread> client_handler_threads;
    for (int i = 0; i < clients_num; i++)
    {
        string client_num_str = std::to_string(i);
        string request_fifo_path = request_fifo_prefix + client_num_str;
        string response_fifo_path = response_fifo_prefix + client_num_str;

        mkfifo(request_fifo_path.c_str(), 0666);
        mkfifo(response_fifo_path.c_str(), 0666);

        client_handler_threads.push_back(std::thread(client_handler_thread, i));
        char *argv[] = {"kitty", "./Sender", const_cast<char*>(client_num_str.c_str()), NULL};
        if (posix_spawnp(&clients_pids[i], "kitty", NULL, NULL, argv, environ))
        {
            cerr << "Couldn't create process " << i << '\n';
            return EXIT_FAILURE;
        }
    }
    cout << "Server is working and waiting for all the clients to terminate\n";
    for (auto& a: client_handler_threads)    
    {
        a.join();
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