#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include <string>
#include <unordered_map>
#include <sstream>

enum class ResponseCode {
    OK = 200,
    BAD_REQUEST = 400
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
    const inline static std::unordered_map<ResponseCode, std::string_view> code_to_meta = {
        {ResponseCode::OK, "200 OK"}
    };    

    const inline static std::unordered_map<ContentType, std::string_view> content_type_to_meta = {
        {ContentType::HTML, "text/html; charset=UTF-8"}, 
        {ContentType::TEXT, "text/plain; charset=UTF-8"}
    };

    std::string constructResponse(); 
};

#endif // HTTP_UTIL_H