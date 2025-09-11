#include "employee.h"
#include <sys/wait.h>
int main(int, char**)
{
    std::string filename_creator;
    size_t num_records;

    std::cout << "Введите имя бинарного файла и количество записей в нем\n";
    std::cin >> filename_creator >> num_records;
    if(num_records  == 0)
    {
        std::cout << "Вы ввели некорректное количество записей\n";
        return EXIT_FAILURE;
    }
    char* const args_creator[] = {"Creator", const_cast<char*>(filename_creator.c_str()), const_cast<char*>(std::to_string(num_records).c_str()), NULL}; 
    pid_t pid_creator = fork();
    int creator_res = 1;
    if(pid_creator == 0)
    {
        execv("./Creator", args_creator);
        _exit(EXIT_FAILURE);
    }
    else wait(&creator_res);

    if(WIFEXITED(creator_res))
    {
        if(WEXITSTATUS(creator_res))
        {
        std::cout << "Произошла ошибка во время вызова Creator. Creator завершил работу с ошибкой\n";
        return EXIT_FAILURE;
        }
    }
    else
    {
        std::cout << "Произошла ошибка во время вызова Creator. Creator был остоновлен\n";
        return EXIT_FAILURE;
    }
    std::ifstream bin_fin(filename_creator, std::ios::binary);
    if(!bin_fin.is_open())
    {
        std::cout << "Не удалось открыть файл.\n";
        return EXIT_FAILURE;
    }
    employee temp_employee;
    while (temp_employee.read(bin_fin))
    {
        std::cout << "\n\nНомер работника: " << temp_employee.num;
        std::cout << "\nИмя работника: " << temp_employee.name;
        std::cout << "\nКоличетсво отработанных часов работника: " << temp_employee.hours;
    }
    bin_fin.close();
    std::cout << "\n\n";


    std::string filename_reporter;
    double salary;

    std::cout << "Введите имя отчёта и оплату за час работу\n";
    std::cin >> filename_reporter >> salary;
    if(salary < 0)
    {
        std::cout << "Вы ввели некорректную оплату за час\n";
        return EXIT_FAILURE;
    }

    char* const args_reporter[] = {"Reporter", const_cast<char*>(filename_creator.c_str()), const_cast<char*>(filename_reporter.c_str()), const_cast<char*>(std::to_string(salary).c_str()), NULL}; 
    pid_t pid_reporter = fork();
    int reporter_res = 1;
    if(pid_reporter == 0)
    {
        execv("./Reporter", args_reporter);
        _exit(EXIT_FAILURE);
    }
    else wait(&reporter_res);
    if(WIFEXITED(reporter_res))
    {
        if(WEXITSTATUS(reporter_res))
        {
        std::cout << "Произошла ошибка во время вызова Reporter. Reporter завершил работу с ошибкой\n";
        return EXIT_FAILURE;
        }
    }
    else
    {
        std::cout << "Произошла ошибка во время вызова Reporter. Reporter был остоновлен\n";
        return EXIT_FAILURE;
    }


    std::ifstream txt_fin(filename_reporter);
    if(!txt_fin.is_open())
    {
        std::cout << "Не удалось открыть файл.\n";
        return EXIT_FAILURE;
    }

    std::string temp_string;
    while(std::getline(txt_fin, temp_string))
    {
        std::cout << temp_string << "\n";
    }
    return EXIT_SUCCESS;
}
