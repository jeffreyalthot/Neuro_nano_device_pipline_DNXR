// Logger.cpp

#include "Logger.h"

#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace dnxr {

namespace {

std::mutex g_logMutex;

std::string timestamp() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void write(std::ostream& os, const std::string& level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_logMutex);

    os << "[" << level << "] "
       << "[" << timestamp() << "] "
       << msg << "\n";
}

} // namespace

void Logger::info(const std::string& msg) {
    write(std::cout, "INFO", msg);
}

void Logger::warn(const std::string& msg) {
    write(std::cout, "WARN", msg);
}

void Logger::error(const std::string& msg) {
    write(std::cerr, "ERROR", msg);
}

} // namespace dnxr