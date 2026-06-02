#include "http_util.h"
#include "util.h"

#include <sstream>

std::string HttpResponse::constructHead() { 
    // get the content length based on whether its an fd or string
    long long size = std::visit([](auto&& arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string>) {
            return arg.size();
        } else if constexpr (std::is_same_v<T, FileDescriptor>) {
            return util::fileSize(arg);
        }

        return 0;
    }, body_variant);

    std::stringstream ss;
    // if the size read failed, we can't possibly send anything
    if (size == -1) {
        ss << "HTTP/1.1 " << code_to_meta.at(ResponseCode::INTERNAL_SERVER_ERROR) << "\r\n";
        ss << "\r\n"; 
    } else {
        // http requires carraige return at the end of every header line
        ss << "HTTP/1.1 " << code_to_meta.at(response_code) << "\r\n";
        ss << "Content-Type: " << content_type_to_meta.at(content_type) << "\r\n";
        ss << "Content-Length: " << size << "\r\n";
        ss << "\r\n"; 
    }

    return ss.str();
}