#pragma once

#include "LogSink.h"

namespace dnxr {

class ConsoleLogSink final : public LogSink {
public:
    void write(const LogRecord& record) override;
};

} // namespace dnxr
