#pragma once
#include <cstddef>
namespace dnxr {
constexpr float DNXR_NEURAL_CHIP_TARGET_FRAME_RATE_HZ = 60.0f;
constexpr std::size_t DNXR_NEURAL_CHIP_CHANNELS_PER_WIRE = 16;

struct ChipCapabilities {
    bool canVisual = false, canAuditory = false, canHaptic = false,
         canVestibular = false, canProprioceptive = false,
         canOlfactory = false, canGustatory = false;
    float maxSafeRateHz = 0;
    int maxPacketSize = 0;
    std::size_t electrodeCount = 0;
    std::size_t wireCount = 0;
    std::size_t maxStimulusChannels = 0;
    bool supportsHardwareCutoff = false, supportsClosedLoopFeedback = false;
};

class ChipCapabilityNegotiation {
public:
    ChipCapabilities clampForClinical(const ChipCapabilities& advertised) const;
    bool supportsFiveSenseClosedLoop(const ChipCapabilities& c) const;
};
}
