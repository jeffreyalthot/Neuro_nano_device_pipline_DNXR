// Logger.h

#pragma once

#include "LogSink.h"

#include <cstddef>
#include <memory>
#include <string>

namespace dnxr {

class Logger {
public:
    static void trace(const std::string& msg);
    static void debug(const std::string& msg);

    /// Log informatif (état normal système)
    static void info(const std::string& msg);

    /// Log avertissement (dégradation non critique)
    static void warn(const std::string& msg);

    /// Log critique (erreur ou état dangereux)
    static void error(const std::string& msg);
    static void fatal(const std::string& msg);

    static void log(
        LogLevel level,
        const std::string& module,
        const std::string& msg,
        std::uint64_t runtimeCycle = 0U);

    static void setMinLevel(LogLevel level);
    static LogLevel minLevel();

    static void addSink(std::shared_ptr<LogSink> sink);
    static void clearSinks();
    static void resetToDefaultSinks();
    static void configureFileSink(
        const std::string& path,
        bool jsonLines = false,
        std::size_t rotateBytes = 1024U * 1024U);
};

} // namespace dnxr
