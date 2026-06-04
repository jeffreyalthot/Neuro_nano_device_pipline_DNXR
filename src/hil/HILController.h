#pragma once

#include "FaultInjectionEngine.h"
#include "HardwarePhantomProfile.h"
#include "LatencyInstrumentation.h"

#include <string>

namespace dnxr {

struct HILSessionResult {
    bool safetyResponse = false;
    bool emergencyStopped = false;
    bool recovered = false;
    bool passed = false;
    std::string summary;
};

class HILController {
public:
    HILSessionResult runSession(const HardwarePhantomProfile& profile,
                                const FaultInjectionEngine& faults,
                                const LatencyInstrumentation& latency) const;
};

} // namespace dnxr
