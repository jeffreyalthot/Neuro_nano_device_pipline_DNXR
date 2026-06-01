#pragma once

#include "LogSink.h"

#include <cstddef>
#include <string>

namespace dnxr {

class FileLogSink final : public LogSink {
public:
    explicit FileLogSink(
        std::string path,
        bool jsonLines = false,
        std::size_t rotateBytes = 1024U * 1024U);

    void write(const LogRecord& record) override;
    const std::string& path() const;

private:
    void rotateIfNeeded();
    std::string formatPlain(const LogRecord& record) const;
    std::string formatJson(const LogRecord& record) const;

    std::string path_;
    bool jsonLines_ = false;
    std::size_t rotateBytes_ = 0U;
};

} // namespace dnxr
