#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iostream>
#include <cerrno> 
#include <cstring>
#include <optional>
#include <syncstream>
#include <filesystem>

namespace util {

    constexpr inline std::optional<int> checkUnixCall(int result, const std::string& syscall_name) {
        if (result == -1) {
            std::osyncstream(std::cerr) << "[syscall] "
                      << syscall_name
                      << " failed: "
                      << std::strerror(errno)
                      << '\n';
            return std::nullopt;
        }


        return std::optional<int>(result);
    }

    // TODO: unit test w/ catch 2?
    inline bool isPathSafe(const std::filesystem::path& root, const std::filesystem::path& access_dir) {
        return true;
    }
}

#endif // UTIL_H