#pragma once

#include "SessionSafetyJournal.h"

#include <string>
#include <vector>

namespace dnxr {

struct AuditWriteResult {
    bool sessionWritten = false;
    bool latestWritten = false;
};

class AuditFileWriter {
public:
    static bool writeEvents(const std::string& path, const std::vector<SessionAuditEvent>& events);
    static bool readEvents(const std::string& path, std::vector<SessionAuditEvent>& events);
    static AuditWriteResult writeSessionAndLatest(
        const std::string& sessionPath,
        const std::string& latestPath,
        const std::vector<SessionAuditEvent>& events);
    static std::string makeTimestampedSessionPath(const std::string& directory = "logs/safety");
};

} // namespace dnxr
