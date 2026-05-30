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
    // Makes sure that paths are proper and are not illegitimate / attempts to redirect out of the server's public files
    inline bool isPathSafe(const std::filesystem::path& root, const std::filesystem::path& access_dir) {
        // no absolute paths should be used
        if (access_dir.is_absolute()) {
            return false;
        }

        auto canonicalized_base = std::filesystem::weakly_canonical(root); 
        auto canonicalized_access = std::filesystem::weakly_canonical(canonicalized_base / access_dir); 
        
        // checks for mismatches in the individual directory folders 
        auto it = std::mismatch(canonicalized_base.begin(), canonicalized_base.end(), canonicalized_access.begin());
        // checks if no mismatch found?
        // mismatch has two iterators
        return it.first == canonicalized_base.end();
    }
}

#endif // UTIL_H