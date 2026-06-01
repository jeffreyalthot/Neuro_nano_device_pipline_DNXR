#include "ConsoleLogSink.h"

#include <iostream>

namespace dnxr {

void ConsoleLogSink::write(const LogRecord& record) {
    std::ostream& out = record.level >= LogLevel::Error ? std::cerr : std::cout;
    out << "[" << toString(record.level) << "] "
        << "[" << record.timestamp << "] ";
    if (!record.module.empty()) {
        out << "[" << record.module << "] ";
    }
    if (record.runtimeCycle > 0U) {
        out << "[cycle=" << record.runtimeCycle << "] ";
    }
    out << record.message << "\n";
}

} // namespace dnxr
