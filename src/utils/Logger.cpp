// Logger.cpp

#include "Logger.h"

#include "ConsoleLogSink.h"
#include "FileLogSink.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <utility>
#include <vector>

namespace dnxr {

namespace {

std::mutex g_logMutex;
LogLevel g_minLevel = LogLevel::Info;
std::vector<std::shared_ptr<LogSink>> g_sinks;

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

std::vector<std::shared_ptr<LogSink>>& sinks() {
    if (g_sinks.empty()) {
        g_sinks.push_back(std::make_shared<ConsoleLogSink>());
    }
    return g_sinks;
}

} // namespace

std::string toString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warn: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
    }
    return "INFO";
}

LogLevel parseLogLevel(const std::string& value) {
    if (value == "trace" || value == "TRACE") return LogLevel::Trace;
    if (value == "debug" || value == "DEBUG") return LogLevel::Debug;
    if (value == "info" || value == "INFO") return LogLevel::Info;
    if (value == "warn" || value == "WARN") return LogLevel::Warn;
    if (value == "error" || value == "ERROR") return LogLevel::Error;
    if (value == "fatal" || value == "FATAL") return LogLevel::Fatal;
    return LogLevel::Info;
}

void Logger::trace(const std::string& msg) {
    log(LogLevel::Trace, {}, msg);
}

void Logger::debug(const std::string& msg) {
    log(LogLevel::Debug, {}, msg);
}

void Logger::info(const std::string& msg) {
    log(LogLevel::Info, {}, msg);
}

void Logger::warn(const std::string& msg) {
    log(LogLevel::Warn, {}, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::Error, {}, msg);
}

void Logger::fatal(const std::string& msg) {
    log(LogLevel::Fatal, {}, msg);
}

void Logger::log(
    LogLevel level,
    const std::string& module,
    const std::string& msg,
    std::uint64_t runtimeCycle) {

    std::lock_guard<std::mutex> lock(g_logMutex);
    if (static_cast<int>(level) < static_cast<int>(g_minLevel)) {
        return;
    }

    LogRecord record;
    record.level = level;
    record.timestamp = timestamp();
    record.module = module;
    record.message = msg;
    record.runtimeCycle = runtimeCycle;

    for (const auto& sink : sinks()) {
        if (sink) {
            sink->write(record);
        }
    }
}

void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_minLevel = level;
}

LogLevel Logger::minLevel() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    return g_minLevel;
}

void Logger::addSink(std::shared_ptr<LogSink> sink) {
    if (!sink) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_logMutex);
    g_sinks.push_back(std::move(sink));
}

void Logger::clearSinks() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_sinks.clear();
}

void Logger::resetToDefaultSinks() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_sinks.clear();
    g_sinks.push_back(std::make_shared<ConsoleLogSink>());
    g_minLevel = LogLevel::Info;
}

void Logger::configureFileSink(const std::string& path, bool jsonLines, std::size_t rotateBytes) {
    addSink(std::make_shared<FileLogSink>(path, jsonLines, rotateBytes));
}

} // namespace dnxr
