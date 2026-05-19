#ifndef HTTP_H
#define HTTP_H

#include <cstdint>

class HttpServer {
public:
    explicit HttpServer(std::uint16_t port);
    ~HttpServer(); 

    bool initialize(); 
    void startListening();

private:
    int connection_fd; 
    const std::uint16_t port;
};

#endif // HTTP_H