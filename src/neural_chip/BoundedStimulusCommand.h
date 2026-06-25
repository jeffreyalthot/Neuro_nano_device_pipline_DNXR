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
    bool validatedBySafetyKernel = false;
    std::string qmsTraceId;
    std::string targetBindingId;
    std::string authorizationScope;
    std::int64_t expiresAtUnixSeconds = 0;
    std::string commandHash;
    std::string previousCommandHash;
    bool freshAckRequired = true;
    float normalizedCeiling = 0.0f;
    float maxDeltaPerFrame = 0.0f;
    float cumulativeBudget = 0.0f;
};
}
