#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

struct min_max_return
{
    int min;
    int max;
};

void* min_max(void* arr_)
{
    std::vector<int> &arr = *static_cast<std::vector<int>*>(arr_); 
    return EXIT_SUCCESS;
}

int main()
{
    /*Я использую std::vector, потому что он соответсвует
    стилю c++ больше, чем указатели.

    А ещё, как я понимаю, раз мы изучаем OS и в случае
    с разработкой под windows пользуемся winapi, а не std::threads,
    то в случае с posix-compliant OS должны использовать pthreads*/

    size_t arr_size;
    std::cout << "Enter number of elements\n";
    std::cin >> arr_size;
    std::vector<int> arr(arr_size);
    std::cout << "Enter elements\n";
    for (size_t i = 0; i < arr_size; i++)
    {
        std::cin >> arr[i];
    }
    //стоило ли здесь декомпозировать код?

    pthread_t min_max_thread;
    pthread_create(&min_max_thread, NULL, min_max, &arr); 
    min_max_return* min_max_res;
    pthread_join(min_max_thread, static_cast<void**>(&min_max_res));
    return EXIT_SUCCESS;
}