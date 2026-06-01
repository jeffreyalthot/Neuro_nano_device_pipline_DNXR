#include "AuditFileWriter.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

std::string encodeField(const std::string& value) {
    std::ostringstream out;
    out << std::hex << std::uppercase;
    for (unsigned char ch : value) {
        if (ch == '%' || ch == '|' || ch == '\n' || ch == '\r') {
            out << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
        } else {
            out << static_cast<char>(ch);
        }
    }
    return out.str();
}

int hexValue(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return 10 + (ch - 'a');
    if (ch >= 'A' && ch <= 'F') return 10 + (ch - 'A');
    return -1;
}

std::string decodeField(const std::string& value) {
    std::string out;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2U < value.size()) {
            const int hi = hexValue(value[i + 1U]);
            const int lo = hexValue(value[i + 2U]);
            if (hi >= 0 && lo >= 0) {
                out.push_back(static_cast<char>((hi << 4) | lo));
                i += 2U;
                continue;
            }
        }
        out.push_back(value[i]);
    }
    return out;
}

std::vector<std::string> splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    return fields;
}

std::string serializeEvent(const SessionAuditEvent& event) {
    std::ostringstream out;
    out << event.sequenceId << '|'
        << event.timestampMs << '|'
        << encodeField(event.category) << '|'
        << encodeField(event.message) << '|'
        << std::setprecision(9) << event.riskScore << '|'
        << event.previousHash << '|'
        << event.eventHash << '|'
        << event.runtimeCycle << '|'
        << encodeField(event.safetyAction) << '|'
        << (event.renderAllowed ? 1 : 0) << '|'
        << (event.emergencyStop ? 1 : 0);
    return out.str();
}

bool parseEvent(const std::string& line, SessionAuditEvent& event) {
    const auto fields = splitLine(line);
    if (fields.size() != 11U) {
        return false;
    }

    try {
        event.sequenceId = static_cast<std::uint64_t>(std::stoull(fields[0]));
        event.timestampMs = static_cast<std::int64_t>(std::stoll(fields[1]));
        event.category = decodeField(fields[2]);
        event.message = decodeField(fields[3]);
        event.riskScore = std::stof(fields[4]);
        event.previousHash = fields[5];
        event.eventHash = fields[6];
        event.runtimeCycle = static_cast<std::uint64_t>(std::stoull(fields[7]));
        event.safetyAction = decodeField(fields[8]);
        event.renderAllowed = fields[9] == "1";
        event.emergencyStop = fields[10] == "1";
    } catch (...) {
        return false;
    }

    return true;
}

} // namespace

bool AuditFileWriter::writeEvents(const std::string& path, const std::vector<SessionAuditEvent>& events) {
    const std::filesystem::path outputPath(path);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        return false;
    }

    output << "dnxr_audit_v1\n";
    for (const auto& event : events) {
        output << serializeEvent(event) << '\n';
    }
    return static_cast<bool>(output);
}

bool AuditFileWriter::readEvents(const std::string& path, std::vector<SessionAuditEvent>& events) {
    std::ifstream input(path);
    if (!input) {
        return false;
    }

    std::string header;
    if (!std::getline(input, header) || header != "dnxr_audit_v1") {
        return false;
    }

    std::vector<SessionAuditEvent> loaded;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        SessionAuditEvent event;
        if (!parseEvent(line, event)) {
            return false;
        }
        loaded.push_back(event);
    }

    events = std::move(loaded);
    return true;
}

AuditWriteResult AuditFileWriter::writeSessionAndLatest(
    const std::string& sessionPath,
    const std::string& latestPath,
    const std::vector<SessionAuditEvent>& events) {
    AuditWriteResult result;
    result.sessionWritten = writeEvents(sessionPath, events);
    result.latestWritten = writeEvents(latestPath, events);
    return result;
}

std::string AuditFileWriter::makeTimestampedSessionPath(const std::string& directory) {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &nowTime);
#else
    localtime_r(&nowTime, &tm);
#endif

    std::ostringstream name;
    name << directory << "/session_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".audit";
    return name.str();
}

} // namespace dnxr
