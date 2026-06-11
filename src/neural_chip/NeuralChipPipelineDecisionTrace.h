#pragma once

#include <cstdint>
#include <vector>

namespace dnxr {

struct PipelineDecisionEvent {
    std::uint64_t frameId = 0;
    const char* stage = "";
    const char* modality = "";
    const char* fieldPath = "";
    const char* rule = "";
    float before01 = 0.0f;
    float after01 = 0.0f;
    float risk01 = 0.0f;
};

class NeuralChipPipelineDecisionTrace {
public:
    void record(const PipelineDecisionEvent& event);
    const std::vector<PipelineDecisionEvent>& events() const;
    void clear();

private:
    std::vector<PipelineDecisionEvent> events_{};
};

} // namespace dnxr
