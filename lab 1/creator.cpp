#include "employee.h"
int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cout << "Неверное количество аргументов\n";
        return EXIT_FAILURE;
    }
    std::string filename = argv[1];
    size_t num_records = std::stoull(argv[2]);
    Employee temp;
    std::ofstream out(filename, std::ios::binary);
    if(!out.is_open())
    {
        std::cout << "Не удалось открыть файл.\n";
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < num_records; i++)
    {
        std::cout << "Введите идентификационный номер сотрудника, имя сотрудника (9 букв) и количество отработанных часов\n";
        std::cin >> temp.num >>temp.name >> temp.hours;
        temp.write(out);
    }
    out.close();
    return EXIT_SUCCESS;
}