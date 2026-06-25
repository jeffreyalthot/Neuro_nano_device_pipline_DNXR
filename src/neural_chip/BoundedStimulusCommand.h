#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr {
struct RawSensoryPayload { std::vector<float> channels; };
struct SanitizedNeuralFrame { std::uint64_t sequenceId = 0; std::vector<float> boundedChannels; bool safetyChecked = false; };
struct BoundedStimulusCommand {
    std::uint64_t sequenceId = 0;
    std::vector<float> boundedChannels;
    float maxOutputCeiling = 0.0f;
    bool activeOutputEnabled = false;
    bool dummyLoadAttached = false;
    std::string safetyCaseId;
};
}
