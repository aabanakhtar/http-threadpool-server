#include "threadpool.h"
#include <iostream>

ThreadPool::ThreadPool(std::size_t n_threads) {
    workers.reserve(n_threads);

    for (std::size_t i = 0; i < n_threads; ++i) {
        // its a member function, must pass this
        workers.push_back(std::thread(&ThreadPool::threadMain, this));
    }
}

void ThreadPool::threadMain() {
    while (true) {
        Task task; 
        {
            // need to use this with condition_variable
            std::unique_lock<std::mutex> guard(pool_mutex); 
            cv.wait(guard, [this]() {
                return !jobs.empty() || terminate;
            });
            
            // nothing to do
            if (terminate && jobs.empty()) {
                return;
            }

            // do a task
            task = std::move(jobs.front()); 
            jobs.pop();
        }
        
        try {
            task.task();
        } catch (...) {
            std::cerr << "Task threw." << std::endl;
        }
    }
}

void ThreadPool::postTask(Task t) {
    // lock the resource for our own usage
    {
        std::lock_guard<std::mutex> guard(pool_mutex);
        jobs.push(std::move(t)); 
    } 

    // wake up one thread that is waiting on a task, handled internally
    cv.notify_one();
}

ThreadPool::~ThreadPool() {
    // set the termination condition true so threads can escape if no task left
    {
        std::lock_guard<std::mutex> guard(pool_mutex); 
        terminate = true; 
    }    

    cv.notify_all(); // wake up all threads from waiting to get exiting going

    for (auto& worker : workers) {
        worker.join();
    }
}
