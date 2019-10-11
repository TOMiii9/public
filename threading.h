#ifndef THREADING_H
#define THREADING_H

#include "typedefs.h"

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

struct Signal {
  private:
    std::mutex              mutex;
    std::condition_variable cv;
    bool                    flag = false;

  public:
    void reset() {
        std::unique_lock<std::mutex> lock(mutex);
        flag = false;
    }

    void notify() {
        std::unique_lock<std::mutex> lock(mutex);
        flag = true;
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);

        while (flag == false) {
            cv.wait(lock);
        }
    }
};

template <typename T>
struct Blocking_Queue {
  private:
    std::mutex              mutex;
    std::condition_variable cv;
    std::queue<T>           queue;

  public:
    int size() {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.size();
    }

    void push(const T &value) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(value);
        cv.notify_one();
    }

    template <typename... Types>
    void emplace(Types... args) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.emplace(args...);
        cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex);

        while (this->queue.empty()) {
            cv.wait(lock);
        }

        T ret(std::move(queue.front()));
        queue.pop();

        return ret;
    }

    bool try_pop(T &out) {
        std::unique_lock<std::mutex> lock(mutex);

        if (this->queue.empty()) {
            return false;
        }

        out = std::move(queue.front());
        queue.pop();
        return true;
    }
};

using Task_Fun_Ptr = void (*)(void *);

struct Async_Task {
    void *       data;
    Task_Fun_Ptr task_function;

    Async_Task(void *data, Task_Fun_Ptr task_function) {
        this->data          = data;
        this->task_function = task_function;
    }
};

struct Async_Worker {
  private:
    list_of<std::thread> worker_threads;
    list_of<std::thread> task_processors;
    list_of<Signal>      work_available;
    i32                  hw_threads;
    i32                  num_threads;

  public:
    atomic_bool                is_shutting_down;
    atomic_i32                 current_job_index;
    atomic_i32                 threads_executing;
    Blocking_Queue<Async_Task> parallel_tasks;
    array_of<Async_Task>       tasks;
    Signal                     work_complete;

    void init();
    void submit(void *data, Task_Fun_Ptr task_fun);
    void parallel_submit(void *data, Task_Fun_Ptr task_fun);
    void wait();

    ~Async_Worker();
};

extern Async_Worker &worker;

#endif
