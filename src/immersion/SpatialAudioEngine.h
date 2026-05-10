#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct SpatialAudioSource {
    std::string id;
    std::string category;
    float distanceMeters;
    float azimuthDegrees;
    float baseLoudness;
};

class SpatialAudioEngine {
public:
    /// Synthesizes a soft-normalized spatial audio packet from world-linked audio sources.
    std::vector<float> synthesizeSpatialMix(const std::vector<SpatialAudioSource>& sources,
                                            float listenerComfort,
                                            float immersionIntensity,
                                            float exitFade) const;

private:
    /// Applies soft-knee normalization to prevent abrupt amplitude spikes in neural audio channels.
    static float softNormalize(float sample);
};

} // namespace dnxr
