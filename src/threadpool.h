#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <cstdint>
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>
#include <atomic>

struct Task {
    std::function<void()> task;
};

class ThreadPool {
public:
    explicit ThreadPool(std::size_t n_threads = std::thread::hardware_concurrency()); 
    ~ThreadPool();

    ThreadPool(const ThreadPool& t) = delete;

    void postTask(Task task); 

private:
    void threadMain();
    void drainTasks(); 

private:    
    bool terminate = false; 
    std::queue<Task> jobs;
    std::vector<std::thread> workers;
    std::mutex pool_mutex;
    std::condition_variable cv;
    std::condition_variable drain_cv;  
    std::atomic_int active_tasks = 0; 

};



#endif // THREADPOOL_H