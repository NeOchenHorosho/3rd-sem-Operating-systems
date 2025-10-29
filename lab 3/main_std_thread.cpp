#include <thread>
#include <barrier>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <vector>

const size_t MAX_THREADS = 1000;
const std::chrono::duration SLEEP_TIME = std::chrono::milliseconds(5);

enum class ThreadState
{
    Wait,
    Resume,
    Kill,
};


struct marker_args
{
    std::vector<int> *arr;
    size_t thread_num;
    ThreadState *state;
};

void print_arr(const std::vector<int> &arr)
{
    std::cout << "Массив: ";
    for (auto i : arr)
    {
        std::cout << i << "\t";
    }
    std::cout << "\n";
}

bool is_valid_thread_to_terminate(size_t thread_to_terminate, size_t num_threads, std::vector<ThreadState> &threads_state);

int main()
{
    size_t arr_size;
    std::cin >> arr_size;
    std::vector<int> arr(arr_size, 0);
    size_t num_threads;
    std::cin >> num_threads;
    if (num_threads > MAX_THREADS)
    {
        std::cerr << "Многовато потоков\n";
        return EXIT_FAILURE;
    }
    std::vector<std::thread> threads;
    std::vector<ThreadState> threads_state(num_threads, ThreadState::Resume);
    std::barrier init_barrier(num_threads + 1);
    std::mutex arr_mutex;
    std::mutex threads_state_mutex;
    std::condition_variable check_thread_state;
    std::condition_variable thread_finished;

    auto marker = [&](size_t thread_num)
    {
        auto &state = threads_state[thread_num];
        srand(thread_num);
        init_barrier.arrive_and_wait();
        while (true)
        {
            arr_mutex.lock();
            size_t possible_index = rand() % arr.size();
            if (arr[possible_index] == 0)
            {
                std::this_thread::sleep_for(SLEEP_TIME);
                arr[possible_index] = thread_num + 1;
                arr_mutex.unlock();
                std::this_thread::sleep_for(SLEEP_TIME);
            }
            else
            {
                size_t marked = 0;
                for (auto a : arr)
                {
                    if (a != 0)
                        marked++;
                }
                arr_mutex.unlock();
                std::cout << "Порядковый номер: " << thread_num;
                std::cout << "\nКоличество помеченныъ элементов: " << marked;
                std::cout << "\nНевозможно пометить: " << possible_index << '\n';
                bool is_killed;
                {
                std::unique_lock<std::mutex> lock(threads_state_mutex);
                state = ThreadState::Wait;
                thread_finished.notify_all();
                check_thread_state.wait(lock, [&state](){return state!=ThreadState::Wait;});
                is_killed = state == ThreadState::Kill;
                }
                if (is_killed)
                {
                    arr_mutex.lock();
                    for (auto &i : arr)
                    {
                        if (i == thread_num + 1)
                            i = 0;
                    }
                    arr_mutex.unlock();
                    return;
                }
            }
        }
    };
    for (size_t i = 0; i < num_threads; i++)
    {
        threads.push_back(std::thread(marker, i));
    }
    init_barrier.arrive_and_wait();
    size_t active_threads = num_threads;
    while (active_threads != 0)
    {
        {
            std::unique_lock threads_state_mutex_lock(threads_state_mutex);
            while (true)
            {
                bool finished_work = true;
                for (auto &a : threads_state)
                {
                    if (a == ThreadState::Resume)
                        finished_work = false;
                }
                if (finished_work)
                    break;
                thread_finished.wait(threads_state_mutex_lock);
            }
        }
        arr_mutex.lock();
        print_arr(arr);
        arr_mutex.unlock();
        std::cout << "Введите номер потока, который требуется уничтожить (начиная с нуля, кончено же)\n";
        size_t thread_to_terminate;
        std::cin >> thread_to_terminate;
        threads_state_mutex.lock();
        if (is_valid_thread_to_terminate(thread_to_terminate, num_threads, threads_state))
        {
            std::cout << "Вы ввели неверный номер потока\n";
            threads_state_mutex.unlock();
            continue;
        }
        threads_state[thread_to_terminate] = ThreadState::Kill;
        active_threads--;
        threads_state_mutex.unlock();
        check_thread_state.notify_all();
        threads[thread_to_terminate].join();
        threads_state_mutex.lock();
        arr_mutex.lock();
        print_arr(arr);
        arr_mutex.unlock();
        for (auto &a : threads_state)
            if (a == ThreadState::Wait)
                a = ThreadState::Resume;
        threads_state_mutex.unlock();
        check_thread_state.notify_all();
    }

    return EXIT_SUCCESS;
}

bool is_valid_thread_to_terminate(size_t thread_to_terminate, size_t num_threads, std::vector<ThreadState> &threads_state)
{
    return thread_to_terminate >= num_threads || threads_state[thread_to_terminate] == ThreadState::Kill;
}
