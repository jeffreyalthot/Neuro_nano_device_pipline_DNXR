#pragma once

#include <cstdint>

namespace dnxr {

struct EmergencyLatchResetRequest {
    std::uint64_t sessionId = 0;
    const char* operatorId = "";
    const char* reason = "";
    bool simulationOperatorConfirmed = false;
    bool safetyChecklistConfirmed = false;
};

class NeuralChipEmergencyLatch {
public:
    void trigger(const char* reason);
    bool latched() const;
    const char* reason() const;
    bool resetWithOperatorAuthorization(const EmergencyLatchResetRequest& request);

private:
    static bool present(const char* value);
    bool latched_ = false;
    const char* reason_ = "";
};

} // namespace dnxr
