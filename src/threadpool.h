#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <cstdint>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t n_threads); 
    ~ThreadPool();

    ThreadPool(const ThreadPool& t) = delete;


};



#endif // THREADPOOL_H