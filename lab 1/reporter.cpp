#include "employee.h"
#include <vector>
#include <algorithm>

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        std::cout << "Неверное количество аргументов\n";
        return EXIT_FAILURE;
    }
    std::string source = argv[1];
    std::string filename = argv[2];
    double salary = std::stod(argv[3]);

    std::ifstream fin(filename, std::ios::binary);
    if(!fin.is_open())
    {
        std::cout << "Не удалось открыть исходный файл.\n";
        return EXIT_FAILURE;
    }

    std::ofstream fout(filename, std::ios::binary);
    if(!fout.is_open())
    {
        std::cout << "Не удалось открыть файл отчёта.\n";
        return EXIT_FAILURE;
    }

    std::vector<employee> employees;
    employee temp;
    while (temp.read(fin))
    {
        employees.push_back(temp);
    }

    std::sort(employees.begin(), employees.end());
    return EXIT_SUCCESS;
}