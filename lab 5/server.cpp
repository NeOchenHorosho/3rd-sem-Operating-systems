#include "common.h"
#include <spawn.h>
#include <thread>
#include <mutex>
#include <iomanip>
#include <condition_variable>
#include <functional>
const int MAX_CLIENTS_NUM = 10;
const int MAX_RECORDS_NUM = 100;

void client_handler_thread(int client_number, int binary_file_fd, std::vector<bool>& is_accessed_table,
                           std::mutex &is_accessed_table_mutex, std::condition_variable &is_accessed_table_modified)
{
    string client_num_str = std::to_string(client_number);
    string request_fifo_path = request_fifo_prefix + client_num_str;
    string response_fifo_path = response_fifo_prefix + client_num_str;

    int response_fifo_fd = open(response_fifo_path.c_str(), O_WRONLY);
    int request_fifo_fd = open(request_fifo_path.c_str(), O_RDONLY);

    Request_package request;
    Response_package response;
    while (read(request_fifo_fd, &request, sizeof(Request_package)))
    {
        response.is_valid = false;
        int entry_num;
        for (int i = 0; i < is_accessed_table.size(); i++)
        {
            int temp_id;
            pread(binary_file_fd, &temp_id, sizeof(int), i * sizeof(Employee));
            if (temp_id == request.employee.num)
            {
                response.is_valid = true;
                entry_num = i;
                break;
            }
        }
        if (!response.is_valid)
        {
            response.is_valid = false;
            write(response_fifo_fd, &response, sizeof(response));
            continue;
        }
        {
            std::unique_lock lock(is_accessed_table_mutex);
            is_accessed_table_modified.wait(lock, [&]
                                            { return !is_accessed_table[entry_num]; });
            is_accessed_table[entry_num] = true;
        }
        if (request.is_write)
        {
            pread(binary_file_fd, &response.employee, sizeof(Employee), entry_num * sizeof(Employee));
            write(response_fifo_fd, &response, sizeof(Response_package));
            read(request_fifo_fd, &request, sizeof(Request_package));
            pwrite(binary_file_fd, &request.employee, sizeof(Employee), entry_num * sizeof(Employee));
        }
        else
        {
            pread(binary_file_fd, &response.employee, sizeof(Employee), entry_num * sizeof(Employee));
            write(response_fifo_fd, &response, sizeof(Response_package));
            char notify_char;
            read(request_fifo_fd, &notify_char, sizeof(char));
        }
        is_accessed_table[entry_num] = false;
        is_accessed_table_modified.notify_all();
    }
}

int main()
{
    cout << "Enter filename\n";
    string filename_cpp;
    cin >> filename_cpp;
    int binary_file_fd = open(filename_cpp.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0666);
    cout << "Enter number of records\n";
    int records_num;
    cin >> records_num;

    if (!is_valid_int(records_num, 1, MAX_RECORDS_NUM))
    {
        cerr << "Invalid number of records\n";
        return EXIT_FAILURE;
    }
    if (binary_file_fd == -1)
    {
        cerr << "Couldn't open the file\n";
        return EXIT_FAILURE;
    }
    Employee temp_employee{};
    for (int i = 0; i < records_num; ++i)
    {
        cout << "\nEnter employee's id\n";
        cin >> temp_employee.num;
        cout << "Enter employee's name\n";
        string temp_employee_name;
        cin >> temp_employee_name;
        strncpy(temp_employee.name, temp_employee_name.c_str(), 9);
        cout << "Enter employee's worked hours\n";
        cin >> temp_employee.hours;

        write(binary_file_fd, &temp_employee, sizeof(Employee));
    }

    cout << "Enter number of clients:\n";
    int clients_num;
    cin >> clients_num;

    if (clients_num <= 0 || clients_num > MAX_CLIENTS_NUM)
    {
        cerr << "Invalid number of clients\n";
        return EXIT_FAILURE;
    }

    std::vector<pid_t> clients_pids(clients_num);
    std::vector<bool> is_accessed_table(clients_num, false);
    std::mutex is_accessed_table_mutex;
    std::condition_variable is_accessed_table_modified;
    extern char **environ;
    std::vector<std::thread> client_handler_threads;
    client_handler_threads.reserve(clients_num);
    for (int i = 0; i < clients_num; i++)
    {
        string client_num_str = std::to_string(i);
        string request_fifo_path = request_fifo_prefix + client_num_str;
        string response_fifo_path = response_fifo_prefix + client_num_str;

        mkfifo(request_fifo_path.c_str(), 0666);
        mkfifo(response_fifo_path.c_str(), 0666);

        client_handler_threads.push_back(std::thread(client_handler_thread, i, binary_file_fd, std::ref(is_accessed_table), std::ref(is_accessed_table_mutex), std::ref(is_accessed_table_modified)));
        char *argv[] = {"kitty", "./Client", const_cast<char *>(client_num_str.c_str()), NULL};
        if (posix_spawnp(&clients_pids[i], "kitty", NULL, NULL, argv, environ))
        {
            cerr << "Couldn't create process " << i << '\n';
            return EXIT_FAILURE;
        }
    }
    cout << "Server is working and waiting for all the clients to terminate\n\n";
    for (auto &a : client_handler_threads)
    {
        a.join();
    }
    cout << "Final table:\n";
    std::cout << std::left 
              << std::setw(10) << "ID"
              << std::setw(10) << "Name"
              << std::setw(10) << "Worked hours"
              << "\n";

    for (int i = 0; i < is_accessed_table.size(); i++)
    {
        Employee temp_employee;
        pread(binary_file_fd, &temp_employee, sizeof(Employee), i * sizeof(Employee));
        cout << std::left << std::setw(10) << temp_employee.num
             << std::setw(10) << temp_employee.name
             << std::setprecision(2) << std::setw(10) << temp_employee.hours << '\n';
    }
    return 0;
}
