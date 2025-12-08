#include "employee.h"
#include <vector>
#include <algorithm>
#include <iomanip>

int main(int argc, char** argv)
{
    
    if(argc != 4)
    {
        std::cout << "Неверное количество аргументов\n";
        return EXIT_FAILURE;
    }
    std::string source = argv[1];
    std::string filename = argv[2];
    double salary_per_hour = std::stod(argv[3]);

    std::ifstream fin(source, std::ios::binary);
    if(!fin.is_open())
    {
        std::cout << "Не удалось открыть исходный файл.\n";
        return EXIT_FAILURE;
    }

    std::ofstream fout(filename);
    if(!fout.is_open())
    {
        std::cout << "Не удалось открыть файл отчёта.\n";
        return EXIT_FAILURE;
    }

    std::vector<Employee> employees;
    Employee temp;
    while (temp.read(fin))
    {
        employees.push_back(temp);
    }
    std::sort(employees.begin(), employees.end());
    fout << "Отчёт по файлу " << source << "\n\n";
    for(auto i: employees)
    {
        fout << std::setw(6) << std::left <<i.num << " | " << std::setw(10) << std::left << i.name << " | " << i.hours*salary_per_hour << "\n";
    }

    fout.close();
    return EXIT_SUCCESS;
}