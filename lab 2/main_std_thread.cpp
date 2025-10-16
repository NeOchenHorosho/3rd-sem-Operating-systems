#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

const timespec MIN_MAX_SLEEP_TIME{0, 7000};
const timespec AVERAGE_SLEEP_TIME{0, 12000};

void min_max(const std::vector<int> &arr, size_t &min_index, size_t &max_index)
{
    min_index = max_index = 0;
    for (size_t i = 0; i < arr.size(); i++)
    {
        nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
        if (arr[i] < arr[min_index])
        {
            nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
            min_index = i;
            continue;
        }
        if (arr[i] > arr[max_index])
            max_index = i;
        nanosleep(&MIN_MAX_SLEEP_TIME, NULL);
        std::cout << arr[max_index] << '\t' << arr[min_index] << '\n';
    }
}

void average(const std::vector<int> &arr, int &result)
{
    long double sum = 0;
    for (size_t i = 0; i < arr.size(); i++)
    {
        sum += arr[i];
        nanosleep(&AVERAGE_SLEEP_TIME, NULL);
    }

    result = static_cast<int>(std::round(sum / arr.size()));
    std::cout << result << '\n';
}

int main()
{
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
    size_t min_index, max_index;
    std::thread min_max_thread(min_max, std::ref(arr), std::ref(min_index), std::ref(max_index));
    int average_res;
    std::thread average_thread(average, std::ref(arr), std::ref(average_res));
    min_max_thread.join();
    average_thread.join();
    arr[max_index] = average_res;
    arr[min_index] = average_res;
    for (auto i : arr)
    {
        std::cout << i << "\t";
    }

    std::cout << "\n";
    return EXIT_SUCCESS;
}