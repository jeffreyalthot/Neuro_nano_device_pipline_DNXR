#pragma once

#include <string>

namespace dnxr {

class Logger {
public:
    /// Emits an informational operational log message.
    static void info(const std::string& msg);
    /// Emits a warning log message for degraded but recoverable states.
    static void warn(const std::string& msg);
    /// Emits an error log message for critical or unsafe states.
    static void error(const std::string& msg);
};

} // namespace dnxr
