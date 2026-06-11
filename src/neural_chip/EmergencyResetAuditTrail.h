#pragma once

#include <cstdint>
#include <vector>

namespace dnxr {

struct EmergencyResetAuditRecord {
    std::uint64_t sessionId = 0;
    const char* operatorId = "";
    const char* reason = "";
    double timestampMs = 0.0;
    bool simulationOperatorConfirmed = false;
    bool safetyChecklistConfirmed = false;
    bool resetAccepted = false;
};

class EmergencyResetAuditTrail {
public:
    void record(const EmergencyResetAuditRecord& record);
    const std::vector<EmergencyResetAuditRecord>& records() const;

private:
    std::vector<EmergencyResetAuditRecord> records_{};
};

} // namespace dnxr
