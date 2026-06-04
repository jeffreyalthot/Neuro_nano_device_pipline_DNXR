#pragma once

#include "ThreatEvent.h"

#include <vector>

namespace dnxr {

class SecurityAuditTrail {
public:
    void append(const ThreatEvent& event);
    std::vector<ThreatEvent> events() const;
    bool containsCriticalEvent() const;

private:
    std::vector<ThreatEvent> events_;
};

} // namespace dnxr
