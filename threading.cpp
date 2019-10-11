#include "threading.h"
#include "registry.h"
#include "util.h"
#include <Windows.h>

// dont call the destructor on shutdown - none cares
Async_Worker &worker = *(new Async_Worker());

static void setup_thread(const wchar_t *thread_name, u64 affinity) {
    // set priority to low, I don't like stuttering audio and youtube etc. when I'm testing this marvel of software engineering
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

    std::wstring name = thread_name + std::to_wstring(affinity);
    SetThreadDescription(GetCurrentThread(), name.c_str());
}

static void worker_thread_entrypoint(Async_Worker *self, Signal *work_available, u64 affinity, const wchar_t *thread_name) {
    setup_thread(thread_name, affinity);

    while (true) {
        work_available->wait();

        if (self->is_shutting_down.load()) {
            break;
        }

        while (true) {
            i32 local_index = self->current_job_index++;

            // haven't finished with this task group
            if (local_index < self->tasks.size()) {
                self->tasks[local_index].task_function(self->tasks[local_index].data);
            } else {
                // we have finished - but are we the last one?
                i32 local_threads_executing = --self->threads_executing;

                // suspend this thread
                work_available->reset();

                if (local_threads_executing == 0) {
                    self->work_complete.notify();
                }

                // stop processing this group
                break;
            }
        }
    }
}

static void task_processor_entrypoint(Async_Worker *self, u64 affinity, const wchar_t *thread_name) {
    setup_thread(thread_name, affinity);

    while (true) {
        Async_Task at = self->parallel_tasks.pop(); // blocking pop
        at.task_function(at.data);
    }

    RT_UNUSED(self)
}

Async_Worker::~Async_Worker() {
    is_shutting_down.store(true);
    for (Signal &sig : work_available) {
        sig.notify(); // wake up the threads
    }

    for (auto &t : worker_threads) {
        t.join();
    }
}

void Async_Worker::init() {
    std::string fmt;
    i32         cpu_reservation = reg_get_i32("cpu_reservation", 0);

#if 0
    hw_threads = 1;
#else
    hw_threads = std::thread::hardware_concurrency();
#endif

    // probably not gonna happen but who knows for sure
    num_threads = hw_threads - cpu_reservation < 1 ? 1 : hw_threads - cpu_reservation;

    is_shutting_down.store(false);
    threads_executing.store(num_threads);

    report("\n");
    report("Starting up worker threads:\n"
           "        HW threads: %d\n"
           "    Worker threads: %d\n\n",
           hw_threads, num_threads);

    for (i32 i = 0; i < num_threads; i++) {
        work_available.emplace_back();
        worker_threads.emplace_back(worker_thread_entrypoint, this, &work_available.back(), i, L"worker_thread_");
        task_processors.emplace_back(task_processor_entrypoint, this, i, L"task_processor_");
    }

    tasks.reserve(1024);
}

void Async_Worker::submit(void *data, Task_Fun_Ptr task_fun) {
    tasks.emplace_back(data, task_fun);
}

void Async_Worker::parallel_submit(void *data, Task_Fun_Ptr task_fun) {
    parallel_tasks.emplace(data, task_fun);
}

void Async_Worker::wait() {
    current_job_index.store(0);
    threads_executing.store(num_threads);
    work_complete.reset();

    for (Signal &sig : work_available) {
        sig.notify(); // wake up the threads
    }

    work_complete.wait();
    tasks.clear();
}
