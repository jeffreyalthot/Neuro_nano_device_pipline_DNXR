#include "Logger.h"

#include <iostream>

namespace dnxr {

void Logger::info(const std::string& msg) {
    // This function writes informational diagnostics to the standard output stream.
    std::cout << "[INFO] " << msg << "\n";
}

void Logger::warn(const std::string& msg) {
    // This function writes warning diagnostics to the standard output stream.
    std::cout << "[WARN] " << msg << "\n";
}

void Logger::error(const std::string& msg) {
    // This function writes error diagnostics to the standard error stream.
    std::cerr << "[ERROR] " << msg << "\n";
}

} // namespace dnxr
