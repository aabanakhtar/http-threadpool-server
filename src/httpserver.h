#ifndef HTTP_H
#define HTTP_H

#include <cstdint>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <syncstream>
#include "threadpool.h" 
#include "http_util.h"

class HttpServer {
public:
    explicit HttpServer(std::uint16_t port, std::size_t n_threads = std::thread::hardware_concurrency());
    ~HttpServer(); 

    bool initialize(); 
    void startListening();


    void setDirectoryIndex(const HttpPage& page) {
        directory_index = page;
    }

    void setContentDirectory(const std::string& path) {
        content_directory = path;
    }

    // Paths are defined relative to the content root.
    void createErrorPageBinding(const ResponseCode errorCode, const HttpPage& page) {
        if (errorCode == ResponseCode::OK) {
            // std::osyncstream is like a lock on cout
            std::osyncstream(std::cerr) << "Can not bind code 200 OK to a page!\n";
            return;
        }

        error_pages[errorCode] = page; 
    }

    void enableMetrics() { enable_metrics = true; }

private:
    void handleRequestTask(int client) const; 
    void dispatchResponse(const int client, const HttpRequest& request) const;

    // tries to create a user error page if possible, otherwise falls back to text-based simple descriptions of the error.
    void generateErrorPage(const ResponseCode error_code, HttpResponse& response) const;

    // http methods
    void httpGet(const HttpRequest& req, HttpResponse& response) const;


    void sendFile(int client_fd, int file_fd, std::size_t fileSize) const;
    void Send(int client_fd, const std::string& buffer) const; 
private:
    int connection_fd; 
    const std::uint16_t port;
    ThreadPool thread_pool;
    std::string content_directory = "/www/"; 
    bool enable_metrics = false;

    // todo: make bindings
    HttpPage directory_index = HttpPage{
        .type = ContentType::TEXT,
        .path = "", 
    };

    std::unordered_map<ResponseCode, HttpPage> error_pages = {}; 
};

#endif // HTTP_H