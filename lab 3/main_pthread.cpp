#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstdlib>

const size_t MAX_THREADS = 1000;
const timespec SLEEP_TIME{0, 5000000};

enum class ThreadState
{
    Wait,
    Resume,
    Kill,
};

pthread_barrier_t init_barrier;
pthread_mutex_t arr_mutex;
pthread_mutex_t threads_state_mutex;
pthread_mutex_t io_mutex;
pthread_cond_t check_threads_state;
pthread_cond_t thread_finished;


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

void *marker(void *arg_)
{
    auto arg = static_cast<marker_args *>(arg_);
    std::vector<int> &arr = *arg->arr;
    size_t thread_num = arg->thread_num;
    auto &state = *arg->state;
    srand(thread_num);
    pthread_barrier_wait(&init_barrier);

    while (true)
    {
        pthread_mutex_lock(&arr_mutex);
        size_t possible_index = rand() % arr.size();
        if (arr[possible_index] == 0)
        {
            nanosleep(&SLEEP_TIME, NULL);
            arr[possible_index] = thread_num + 1;
            pthread_mutex_unlock(&arr_mutex);
            nanosleep(&SLEEP_TIME, NULL);
        }
        else
        {
            size_t marked = 0;
            for (auto a : arr)
            {
                if (a == thread_num+1)
                    marked++;
            }
            pthread_mutex_unlock(&arr_mutex);

            pthread_mutex_lock(&io_mutex);
            std::cout << "Порядковый номер: " << thread_num;
            std::cout << "\nКоличество помеченныъ элементов: " << marked;
            std::cout << "\nНевозможно пометить: " << possible_index << "\n\n";
            pthread_mutex_unlock(&io_mutex);

            pthread_mutex_lock(&threads_state_mutex);
            state = ThreadState::Wait;
            pthread_cond_broadcast(&thread_finished);
            while (state == ThreadState::Wait)
                pthread_cond_wait(&check_threads_state, &threads_state_mutex);

            bool is_killed = state == ThreadState::Kill;
            pthread_mutex_unlock(&threads_state_mutex);
            if (is_killed)
            {
                pthread_mutex_lock(&arr_mutex);
                for (auto &i : arr)
                {
                    if (i == thread_num + 1)
                        i = 0;
                }
                pthread_mutex_unlock(&arr_mutex);
                delete arg;
                return nullptr;
            }
        }
    }
}

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
    std::vector<pthread_t> threads(num_threads);
    std::vector<ThreadState> threads_state(num_threads, ThreadState::Resume);
    pthread_barrier_init(&init_barrier, NULL, num_threads + 1);
    pthread_mutex_init(&arr_mutex, NULL);
    pthread_mutex_init(&threads_state_mutex, NULL);
    pthread_mutex_init(&io_mutex, NULL);
    pthread_cond_init(&check_threads_state, NULL);
    pthread_cond_init(&thread_finished, NULL);
    for (size_t i = 0; i < num_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, marker, new marker_args{&arr, i, &threads_state[i]}) != 0)
        {
            std::cerr << "Поток не создался\n";
            return EXIT_FAILURE;
        }
    }
    pthread_barrier_wait(&init_barrier);
    size_t active_threads = num_threads;
    while (active_threads != 0)
    {
        pthread_mutex_lock(&threads_state_mutex);
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
            pthread_cond_wait(&thread_finished, &threads_state_mutex);
        }
        pthread_mutex_unlock(&threads_state_mutex);
        pthread_mutex_lock(&arr_mutex);
        print_arr(arr);
        pthread_mutex_unlock(&arr_mutex);
        pthread_mutex_lock(&io_mutex);
        std::cout << "Введите номер потока, который требуется уничтожить (начиная с нуля, кончено же)\n";
        size_t thread_to_terminate;
        std::cin >> thread_to_terminate;
        pthread_mutex_unlock(&io_mutex);
        pthread_mutex_lock(&threads_state_mutex);
        if (thread_to_terminate >= num_threads || threads_state[thread_to_terminate] == ThreadState::Kill)
        {
            
            pthread_mutex_lock(&io_mutex);
            std::cout << "Вы ввели неверный номер потока\n";
            pthread_mutex_unlock(&threads_state_mutex);
            continue;
            pthread_mutex_unlock(&io_mutex);
        }
        threads_state[thread_to_terminate] = ThreadState::Kill;
        active_threads--;
        pthread_mutex_unlock(&threads_state_mutex);
        pthread_cond_broadcast(&check_threads_state);
        pthread_join(threads[thread_to_terminate], NULL);
        pthread_mutex_lock(&threads_state_mutex);
        pthread_mutex_lock(&arr_mutex);
        print_arr(arr);
        pthread_mutex_unlock(&arr_mutex);
        for (auto &a : threads_state)
            if (a == ThreadState::Wait)
                a = ThreadState::Resume;
        pthread_mutex_unlock(&threads_state_mutex);
        pthread_cond_broadcast(&check_threads_state);
    }

    pthread_mutex_destroy(&arr_mutex);
    pthread_mutex_destroy(&threads_state_mutex);
    pthread_barrier_destroy(&init_barrier);
    pthread_cond_destroy(&check_threads_state);
    pthread_cond_destroy(&thread_finished);
    return EXIT_SUCCESS;
}