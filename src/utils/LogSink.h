#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Fatal = 5
};

std::string toString(LogLevel level);
LogLevel parseLogLevel(const std::string& value);

struct LogRecord {
    LogLevel level = LogLevel::Info;
    std::string timestamp;
    std::string module;
    std::string message;
    std::uint64_t runtimeCycle = 0;
};

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogRecord& record) = 0;
};

} // namespace dnxr
