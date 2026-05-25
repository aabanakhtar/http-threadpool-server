#ifndef HTTP_H
#define HTTP_H

#include <cstdint>
#include <unordered_map>
#include <sstream>
#include "threadpool.h" 
#include "http_util.h"


class HttpServer {
public:
    explicit HttpServer(std::uint16_t port);
    ~HttpServer(); 

    bool initialize(); 
    void startListening();

    void setContentDirectory(const std::string& path);

private:
    void handleRequestTask(int data);

private:
    int connection_fd; 
    const std::uint16_t port;
    ThreadPool thread_pool; 
};

#endif // HTTP_H