#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iostream>
#include <cerrno> 
#include <cstring>
#include <optional>


namespace util {

    constexpr inline std::optional<int> checkUnixCall(int result, const std::string& syscall_name) {
        if (result == -1) {
            std::cerr << "[syscall] "
                      << syscall_name
                      << " failed: "
                      << std::strerror(errno)
                      << '\n';
            return std::nullopt;
        }


        return std::optional<int>(result);
    }

}

#endif // UTIL_H