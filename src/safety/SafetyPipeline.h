#pragma once

#include "ISafetyLayer.h"

#include <memory>
#include <vector>

namespace dnxr {

class SafetyPipeline {
public:
    static SafetyPipeline createDefault();

    void addLayer(std::unique_ptr<ISafetyLayer> layer);
    SafetyDecisionTrace evaluate(const SafetyGovernorInput& input) const;
    std::size_t layerCount() const;

private:
    static SafetyDecision aggregate(const std::vector<SafetyLayerDecision>& decisions);

    std::vector<std::unique_ptr<ISafetyLayer>> layers_;
};

} // namespace dnxr
