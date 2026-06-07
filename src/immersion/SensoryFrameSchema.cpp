#include "SensoryFrameSchema.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

const SensoryChannelDescriptor* SensoryFrameSchema::findChannel(std::size_t index) const {
    const auto it = std::find_if(
        channels.begin(),
        channels.end(),
        [index](const SensoryChannelDescriptor& descriptor) {
            return descriptor.index == index;
        });
    return it == channels.end() ? nullptr : &(*it);
}

SensoryModality SensoryFrameSchema::modalityForChannel(std::size_t index) const {
    if (const auto* descriptor = findChannel(index)) {
        return descriptor->modality;
    }
    return legacyModalityForChannel(index);
}

float SensoryFrameSchema::safetyWeightForChannel(std::size_t index) const {
    if (const auto* descriptor = findChannel(index)) {
        return std::isfinite(descriptor->safetyWeight) && descriptor->safetyWeight > 0.0f
            ? descriptor->safetyWeight
            : 1.0f;
    }
    return 1.0f;
}

bool SensoryFrameSchema::covers(SensoryModality modality) const {
    return std::any_of(
        channels.begin(),
        channels.end(),
        [modality](const SensoryChannelDescriptor& descriptor) {
            return descriptor.modality == modality;
        });
}

bool SensoryFrameSchema::valid() const {
    std::vector<std::size_t> indices;
    indices.reserve(channels.size());
    for (const auto& channel : channels) {
        if (channel.modality == SensoryModality::Count ||
            !std::isfinite(channel.safetyWeight) ||
            channel.safetyWeight <= 0.0f) {
            return false;
        }
        if (std::find(indices.begin(), indices.end(), channel.index) != indices.end()) {
            return false;
        }
        indices.push_back(channel.index);
    }
    return true;
}

SensoryFrameSchema SensoryFrameSchema::defaultSixSenseSchema(std::size_t channelCount) {
    SensoryFrameSchema schema;
    schema.channels.reserve(channelCount);
    for (std::size_t i = 0; i < channelCount; ++i) {
        schema.channels.push_back({i, legacyModalityForChannel(i), "legacy_channel_" + std::to_string(i), 1.0f});
    }
    return schema;
}

SensoryModality SensoryFrameSchema::legacyModalityForChannel(std::size_t index) {
    if (index <= 2U) {
        return SensoryModality::Visual;
    }
    if (index <= 4U) {
        return SensoryModality::Auditory;
    }
    if (index <= 6U) {
        return SensoryModality::HapticTouch;
    }
    if (index == 7U || index == 8U) {
        return SensoryModality::Olfactory;
    }
    if (index == 9U) {
        return SensoryModality::Proprioceptive;
    }
    if (index == 10U) {
        return SensoryModality::Gustatory;
    }
    if (index == 11U) {
        return SensoryModality::Vestibular;
    }
    return SensoryModality::Vestibular;
}

} // namespace dnxr
