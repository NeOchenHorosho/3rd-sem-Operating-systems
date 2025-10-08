#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

const timespec MIN_MAX_SLEEP_TIME{0, 7000};
const timespec AVERAGE_SLEEP_TIME{0, 12000};

struct min_max_ret_struct
{
    min_max_ret_struct(size_t min_, size_t max_) : min_index(min_), max_index(max_) {}
    size_t min_index;
    size_t max_index;
};

void *min_max(void *arr_)
{
    std::vector<int> &arr = *static_cast<std::vector<int> *>(arr_);
    size_t min_index = 0, max_index = 0;
    for (size_t i = 0; i < arr.size(); i++)
    {
        if (arr[i] < arr[min_index])
        {
            nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
            min_index = i;
            continue;
        }
        nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
        if (arr[i] > arr[max_index])
            max_index = i;
        nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
    }

    return new min_max_ret_struct(min_index, max_index);
}

void *average(void *arr_)
{
    std::vector<int> &arr = *static_cast<std::vector<int> *>(arr_);
    long double sum = 0;
    for (size_t i = 0; i < arr.size(); i++)
    {
        sum += arr[i];
        nanosleep(&AVERAGE_SLEEP_TIME, NULL);
    }

    return new int(static_cast<int>(std::round(sum / arr.size())));
}

int main()
{
    /*Я использую std::vector, потому что он соответсвует
    стилю c++ больше, чем указатели.*/

    size_t arr_size;
    std::cout << "Enter number of elements\n";
    std::cin >> arr_size;
    std::vector<int> arr(arr_size);
    std::cout << "Enter elements\n";
    for (size_t i = 0; i < arr_size; i++)
    {
        std::cin >> arr[i];
    }
    // стоило ли здесь декомпозировать код?

    pthread_t min_max_thread;
    pthread_create(&min_max_thread, NULL, min_max, &arr);

    pthread_t average_thread;
    pthread_create(&average_thread, NULL, average, &arr);


    void *min_max_ret_val;
    void *average_ret_val;
    pthread_join(min_max_thread, &min_max_ret_val);
    pthread_join(average_thread, &average_ret_val);
    min_max_ret_struct &min_max_res = *static_cast<min_max_ret_struct *>(min_max_ret_val);
    int &average_res= *static_cast<int *>(average_ret_val);
    arr[min_max_res.max_index]=average_res;
    arr[min_max_res.min_index]=average_res;
    for(auto i: arr)
    {
        std::cout << i << "\t";
    }
    std::cout << "\n";
    return EXIT_SUCCESS;
}