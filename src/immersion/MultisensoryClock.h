#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class SensoryChannelKind {
    Visual,
    Audio,
    Haptic,
    Olfactory,
    Gustatory,
    Proprioceptive,
    Vestibular,
    Unknown
};

struct SensoryChannelPacket {
    SensoryChannelKind kind = SensoryChannelKind::Unknown;
    std::uint64_t sequenceId = 0;
    double timestampMs = 0.0;
    float intensity = 0.0f;
    float confidence = 1.0f;
    float safetyLimit = 1.0f;
    std::string source = "unknown";
};

struct MultisensoryClockContract {
    double targetTimestampMs = 0.0;
    double maxPhaseErrorMs = 2.0;
    std::uint64_t expectedSequenceId = 0;
    float maxSafeIntensity = 1.0f;
};

struct MultisensoryAlignmentReport {
    std::vector<float> alignedFrame;
    double maxObservedPhaseErrorMs = 0.0;
    float confidence = 1.0f;
    float recommendedIntensityScale = 1.0f;
    bool synchronized = true;
    bool degradationApplied = false;
    std::string reason = "multisensory_sync_nominal";
};

class MultisensoryClock {
public:
    MultisensoryAlignmentReport align(
        const std::vector<SensoryChannelPacket>& packets,
        const MultisensoryClockContract& contract) const;

    MultisensoryAlignmentReport alignVector(
        const std::vector<float>& frame,
        std::uint64_t sequenceId,
        double timestampMs,
        double maxPhaseErrorMs) const;

    static SensoryChannelKind kindForChannelIndex(std::size_t index);
    static std::string kindName(SensoryChannelKind kind);

private:
    float sanitizeIntensity(float value, float safetyLimit) const;
    float sanitizeConfidence(float value) const;
};

} // namespace dnxr
