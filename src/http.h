#ifndef HTTP_H
#define HTTP_H

#include <cstdint>
#include <unordered_map>
#include <sstream>
#include "threadpool.h" 

enum class ResponseCode {
    OK = 200
};

enum class ContentType {
    HTML, 
    TEXT
};

struct HttpResponse {
    ResponseCode response_code;
    ContentType content_type; 
    std::string body;


    // string_view removes allocs
    const inline static std::unordered_map<ResponseCode, std::string_view> code_to_meta = {
        {ResponseCode::OK, "200 OK"}
    };    

    const inline static std::unordered_map<ContentType, std::string_view> content_type_to_meta = {
        {ContentType::HTML, "text/html; charset=UTF-8"}, 
        {ContentType::TEXT, "text/plain; charset=UTF-8"}
    };

    inline std::string constructResponse() { 
        std::stringstream ss;
        std::size_t content_length = body.length();
        // http requires carraige return at the end of every header line
        ss << "HTTP/1.1 " << code_to_meta.at(response_code) << "\r\n";
        ss << "Content-Type: " << content_type_to_meta.at(content_type) << "\r\n";
        ss << "Content-Length: " << content_length << "\r\n";
        ss << "\r\n"; 
        ss << body;
        return ss.str();
    }
};

class HttpServer {
public:
    explicit HttpServer(std::uint16_t port);
    ~HttpServer(); 

    bool initialize(); 
    void startListening();

private:
    void handleRequestTask(int data);

private:
    int connection_fd; 
    const std::uint16_t port;
    ThreadPool thread_pool; 
};

#endif // HTTP_H