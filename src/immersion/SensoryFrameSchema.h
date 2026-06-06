#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class SensoryModality : std::size_t {
    Visual = 0,
    Auditory = 1,
    HapticTouch = 2,
    Olfactory = 3,
    Gustatory = 4,
    Proprioceptive = 5,
    Vestibular = 6,
    ChemicalTasteSmell = Olfactory,
    Count = 7
};

struct SensoryChannelDescriptor {
    std::size_t index = 0;
    SensoryModality modality = SensoryModality::Visual;
    std::string semanticName;
    float safetyWeight = 1.0f;
};

struct SensoryFrameSchema {
    std::vector<SensoryChannelDescriptor> channels;
    std::uint32_t schemaVersion = 1;

    const SensoryChannelDescriptor* findChannel(std::size_t index) const;
    SensoryModality modalityForChannel(std::size_t index) const;
    float safetyWeightForChannel(std::size_t index) const;
    bool covers(SensoryModality modality) const;
    bool valid() const;

    static SensoryFrameSchema defaultSixSenseSchema(std::size_t channelCount = 12U);
    static SensoryModality legacyModalityForChannel(std::size_t index);
};

} // namespace dnxr
