#pragma once

#include "SafetyDecisionTrace.h"

#include <string>

namespace dnxr {

class ISafetyLayer {
public:
    virtual ~ISafetyLayer() = default;

    virtual std::string name() const = 0;
    virtual SafetyLayerDecision evaluate(const SafetyGovernorInput& input) const = 0;
};

} // namespace dnxr
