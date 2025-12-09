#include "common.h"

int main(int argc, char** argv)
{

    if (argc < 2)
    {
        cerr << "Error: not enough arguments";
        return EXIT_FAILURE;
    }

    string client_num_str(argv[1]);
    string request_fifo_path = request_fifo_prefix + client_num_str;
    string response_fifo_path = response_fifo_prefix + client_num_str;

    int response_fifo_fd = open(response_fifo_path.c_str(),O_RDONLY);
    int request_fifo_fd = open(request_fifo_path.c_str(), O_WRONLY);
    

    while (true)
    {
        int operation_num;
        cout << "Choose operation to perform:\n 0) Exit the programm\n 1) modify employee's record\n 2) retrive employee's record\n";
        cin >> operation_num;
        switch (operation_num)
        {
        case 0:
        {
            close(response_fifo_fd);
            close(request_fifo_fd);
            return EXIT_SUCCESS;
        }
        case 1:
        {
            cout << "Enter employee's number\n";
            int employee_num;
            cin >> employee_num;
            Request_package request{true, {employee_num, "", 0.0}};
            write(request_fifo_fd, &request, sizeof(Request_package));
            Response_package response;
            read(response_fifo_fd, &response, sizeof(Response_package));
            if(!response.is_valid)
            {
                cout << "There's no employee with such number\n";
                break;
            }
            cout << "Data of the employee:\n\tEmployee's name:" << response.employee.name
            <<  "\n\tEmployee's worked hours:" << response.employee.num;
            cout << "Enter new name\n";
            string temp_employee_name;
            cin >> temp_employee_name;
            strncpy(request.employee.name, temp_employee_name.c_str(), 9);
            cout << "Enter new worked hours:\n";
            cin >> request.employee.hours;
            request.is_write = true;
            write(request_fifo_fd, &request, sizeof(Request_package));
            cout << "Your request is fulfiled\n";
        }
            break;
        case 2:
        {
            cout << "Enter employee's number\n";
            int employee_num;
            cin >> employee_num;
            Request_package request{false, {employee_num, "", 0.0}};
            write(request_fifo_fd, &request, sizeof(Request_package));
            Response_package response;
            read(response_fifo_fd, &response, sizeof(Response_package));
            if(!response.is_valid)
            {
                cout << "There's no employee with such number\n";
                break;
            }
            cout << "Data of the employee:\n\tEmployee's name:" << response.employee.name
            <<  "\n\tEmployee's worked hours:" << response.employee.num
            << "\n\nPress Enter to end the access to the record\n";
            cin.get();
            char notify_byte = '0';
            write(request_fifo_fd,&notify_byte, sizeof(char));
        }    
            break;
        default:
            cout << "You've entered wrong number\n";
            break;
        }
    }
    

    
}