#include "FileLogSink.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

std::string escapeJson(const std::string& value) {
    std::ostringstream out;
    for (char ch : value) {
        switch (ch) {
            case '\\': out << "\\\\"; break;
            case '"': out << "\\\""; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: out << ch; break;
        }
    }
    return out.str();
}

} // namespace

FileLogSink::FileLogSink(std::string path, bool jsonLines, std::size_t rotateBytes)
    : path_(std::move(path)),
      jsonLines_(jsonLines),
      rotateBytes_(rotateBytes) {}

void FileLogSink::write(const LogRecord& record) {
    const std::filesystem::path outputPath(path_);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    rotateIfNeeded();

    std::ofstream output(path_, std::ios::app);
    if (!output) {
        return;
    }

    output << (jsonLines_ ? formatJson(record) : formatPlain(record)) << '\n';
}

const std::string& FileLogSink::path() const {
    return path_;
}

void FileLogSink::rotateIfNeeded() {
    if (rotateBytes_ == 0U || !std::filesystem::exists(path_)) {
        return;
    }

    const auto size = static_cast<std::size_t>(std::filesystem::file_size(path_));
    if (size < rotateBytes_) {
        return;
    }

    const std::string rotatedPath = path_ + ".1";
    std::error_code ignored;
    std::filesystem::remove(rotatedPath, ignored);
    std::filesystem::rename(path_, rotatedPath, ignored);
}

std::string FileLogSink::formatPlain(const LogRecord& record) const {
    std::ostringstream out;
    out << "[" << toString(record.level) << "] "
        << "[" << record.timestamp << "] ";
    if (!record.module.empty()) {
        out << "[" << record.module << "] ";
    }
    if (record.runtimeCycle > 0U) {
        out << "[cycle=" << record.runtimeCycle << "] ";
    }
    out << record.message;
    return out.str();
}

std::string FileLogSink::formatJson(const LogRecord& record) const {
    std::ostringstream out;
    out << "{"
        << "\"timestamp\":\"" << escapeJson(record.timestamp) << "\","
        << "\"level\":\"" << escapeJson(toString(record.level)) << "\","
        << "\"module\":\"" << escapeJson(record.module) << "\","
        << "\"runtime_cycle\":" << record.runtimeCycle << ","
        << "\"message\":\"" << escapeJson(record.message) << "\""
        << "}";
    return out.str();
}

} // namespace dnxr
