#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include <string>
#include <unordered_map>
#include <sstream>

enum class ResponseCode {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    FORBIDDEN = 403
};

enum class ContentType {
    HTML, 
    TEXT
};


enum class RequestMethod { 
    GET,
    BAD 
};

struct HttpRequest {
    RequestMethod method; 
    std::string resource_uri;
    
    const inline static std::unordered_map<std::string, RequestMethod> str_to_request_method = {
        {"GET", RequestMethod::GET}
    };    

    HttpRequest(const std::string& request) {
        std::istringstream ss(request);
        std::string method_key, http_version; 

        ss >> method_key >> resource_uri >> http_version; 
        
        // possible error 400, bad request
        if (ss.fail()) {
            method = RequestMethod::BAD; 
            return;
        }

        // lookup the key and ensure the request is well-formed
        auto it = str_to_request_method.find(method_key);
        if (it == str_to_request_method.end() || http_version != "HTTP/1.1") {
            method = RequestMethod::BAD; 
            return;
        }

        method = it->second; 
    }
}; 

struct HttpResponse {
    ResponseCode response_code;
    ContentType content_type; 
    std::string body;


    // string_view removes allocs
    const inline static std::unordered_map<ResponseCode, std::string> code_to_meta = {
        {ResponseCode::OK, "200 OK"},
        {ResponseCode::BAD_REQUEST, "400 Bad Request"},
        {ResponseCode::NOT_FOUND, "404 Not Found"},
        {ResponseCode::FORBIDDEN, "403 Forbidden"}
    };    

    const inline static std::unordered_map<ContentType, std::string> content_type_to_meta = {
        {ContentType::HTML, "text/html; charset=UTF-8"}, 
        {ContentType::TEXT, "text/plain; charset=UTF-8"}
    };

    std::string constructResponse(); 
};

// utility struct for setting up default pages like 404, index, etc.
struct HttpPage {
    ContentType type = ContentType::TEXT; 
    std::string path = ""; 
};

#endif // HTTP_UTIL_H