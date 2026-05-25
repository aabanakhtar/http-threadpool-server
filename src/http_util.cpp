#include "http_util.h"

#include <sstream>

std::string HttpResponse::constructResponse() { 
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