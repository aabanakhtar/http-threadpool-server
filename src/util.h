#ifndef UTIL_H
#define UTIL_H


#include <string>
#include <iostream>
#include <cerrno> 
#include <cstring>

namespace util {

    constexpr inline int checkUnixCall(int result, const std::string& syscall_name, bool should_exit) {
        if (result == -1) {
            std::cerr << "[syscall] "
                      << syscall_name
                      << " failed: "
                      << std::strerror(errno)
                      << '\n';

            if (should_exit) {
                std::exit(1);
            }
        }

        return result;
    }

}

#endif // UTIL_H