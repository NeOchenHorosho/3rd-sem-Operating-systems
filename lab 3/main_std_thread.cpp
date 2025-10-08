#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>

const size_t MAX_THREADS = 1000; //Какое значение оптимально?

void marker()

int main()
{
    return EXIT_SUCCESS;
    size_t arr_size;
    std::cin >> arr_size;
    std::vector<int> arr(arr_size, 0);

    size_t num_threads;
    std::cin >> num_threads;
    if(num_threads > MAX_THREADS) return EXIT_FAILURE;

    for (size_t i = 0; i < num_threads; i++)
    {
        
    }
    
}