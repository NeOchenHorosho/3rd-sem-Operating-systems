#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>

const size_t MAX_THREADS = 1000;
const timespec SLEEP_TIME{0, 5000000};

enum ThreadState
{
    Wait,
    Resume,       
    Kill,
    Dead
};

pthread_barrier_t init_barrier;
pthread_mutex_t arr_mutex;
pthread_mutex_t threads_state_mutex;
pthread_cond_t check_threads_state;

struct marker_args
{
    std::vector<int> *arr;
    size_t *marked;
    size_t thread_num;
    ThreadState * state;
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

void *marker(void *arg_)
{
    auto arg = static_cast<marker_args *>(arg_);
    std::vector<int> &arr = *arg->arr;
    size_t thread_num = arg->thread_num;
    size_t &marked = *arg->marked;
    auto state = *arg->state;
    srand(thread_num);
    pthread_barrier_wait(&init_barrier);

    while (true)
    {
        int rand_num = rand();
        pthread_mutex_lock(&arr_mutex);
        size_t possible_index = rand() % arr.size();
        if (arr[possible_index] == 0)
        {
            nanosleep(&SLEEP_TIME, NULL);
            arr[possible_index] = thread_num + 1;
            marked++;
            pthread_mutex_unlock(&arr_mutex);
            nanosleep(&SLEEP_TIME, NULL);
            continue;
        }
        else
        {
            std::cout << "Порядковый номер: " << thread_num;
            std::cout << "\nКоличество помеченныъ элементов: " << marked;
            std::cout << "\nНевозможно пометить: " << possible_index << '\n';

            
            pthread_mutex_lock(&threads_state_mutex);
            state = ThreadState::Wait;
            while(state == ThreadState::Wait)
            {
                pthread_cond_wait(&check_threads_state, &threads_state_mutex);
            }
            if (state == ThreadState::Kill)
            {
                pthread_mutex_lock(&arr_mutex);
                for (auto i : arr)
                {
                    if (i == thread_num + 1)
                        i = 0;
                }
                state = ThreadState::Dead;
                delete arg_;
                return nullptr;
            }
            pthread_mutex_unlock(&threads_state_mutex);
            
            // pthread_barrier_wait(&unable_to_work_barrier); где-то поставить
        }
    }
}

int main()
{
    size_t arr_size;
    std::cin >> arr_size;
    size_t *marked = new size_t(0);
    std::vector<int> arr(arr_size, 0);
    size_t num_threads;
    std::cin >> num_threads;
    if (num_threads > MAX_THREADS)
    {
        std::cerr << "Многовато потоков\n";
        return EXIT_FAILURE;
    }
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadState> threads_state(num_threads, Resume);
    pthread_barrier_init(&init_barrier, NULL, num_threads + 1);
    pthread_mutex_init(&arr_mutex, NULL);
    pthread_mutex_init(&threads_state_mutex, NULL);
    for (size_t i = 0; i < num_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, marker, new marker_args{&arr, marked, i, &threads_state[i]}) != 0)
        {
            std::cerr << "Поток не создался\n";
            return EXIT_FAILURE;
        }
    }
    pthread_barrier_wait(&init_barrier);
    size_t active_threads = num_threads;
    while (active_threads != 0)
    {
        pthread_mutex_lock(&arr_mutex);
        print_arr(arr);
        pthread_mutex_unlock(&arr_mutex);
        std::cout << "Введите номер потока, который требуется уничтожить (начиная с нуля, кончено же)\n";
        size_t thread_to_terminate;
        std::cin >> thread_to_terminate;
        if (thread_to_terminate >= num_threads || threads_state[thread_to_terminate])
        {
            std::cout << "Вы ввели неверный номер потока\n";
            continue;
        }
        pthread_mutex_lock(&threads_state_mutex);
        threads_state[thread_to_terminate] = ThreadState::Kill;
        pthread_mutex_unlock(&threads_state_mutex);
        pthread_cond_broadcast(&check_threads_state);
        
    }

    pthread_barrier_destroy(&init_barrier);
    return EXIT_SUCCESS;
}