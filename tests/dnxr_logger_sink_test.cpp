#include "../src/utils/FileLogSink.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

} // namespace

int main() {
    const std::string plainPath = "build/dnxr_logger_sink_test.log";
    const std::string jsonPath = "build/dnxr_logger_sink_test.jsonl";
    std::remove(plainPath.c_str());
    std::remove(jsonPath.c_str());

    assert(dnxr::toString(dnxr::LogLevel::Warn) == "WARN");
    assert(dnxr::parseLogLevel("debug") == dnxr::LogLevel::Debug);
    assert(dnxr::parseLogLevel("FATAL") == dnxr::LogLevel::Fatal);

    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Warn);
    dnxr::Logger::configureFileSink(plainPath, false, 1024U * 1024U);
    dnxr::Logger::log(dnxr::LogLevel::Info, "logger_test", "filtered", 7U);
    dnxr::Logger::log(dnxr::LogLevel::Warn, "logger_test", "visible warning", 7U);

    const auto plain = readFile(plainPath);
    assert(plain.find("visible warning") != std::string::npos);
    assert(plain.find("logger_test") != std::string::npos);
    assert(plain.find("cycle=7") != std::string::npos);
    assert(plain.find("filtered") == std::string::npos);

    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Trace);
    dnxr::Logger::addSink(std::make_shared<dnxr::FileLogSink>(jsonPath, true, 1024U * 1024U));
    dnxr::Logger::log(dnxr::LogLevel::Error, "network", "packet \"tamper\" rejected", 9U);

    const auto json = readFile(jsonPath);
    assert(json.find("\"level\":\"ERROR\"") != std::string::npos);
    assert(json.find("\"module\":\"network\"") != std::string::npos);
    assert(json.find("packet \\\"tamper\\\" rejected") != std::string::npos);

    dnxr::Logger::resetToDefaultSinks();
    std::remove(plainPath.c_str());
    std::remove(jsonPath.c_str());
    return 0;
}
