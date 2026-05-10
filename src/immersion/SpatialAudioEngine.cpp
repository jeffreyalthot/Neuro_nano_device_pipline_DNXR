#include "SpatialAudioEngine.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

float SpatialAudioEngine::softNormalize(float sample) {
    // This function compresses extreme peaks while preserving spatial detail for comfort.
    const float absSample = std::fabs(sample);
    const float compressed = absSample / (1.0F + absSample);
    return sample >= 0.0F ? compressed : -compressed;
}

std::vector<float> SpatialAudioEngine::synthesizeSpatialMix(const std::vector<SpatialAudioSource>& sources,
                                                            float listenerComfort,
                                                            float immersionIntensity,
                                                            float exitFade) const {
    // This function builds a 4-channel packet: left, right, front-presence, rear-presence.
    float left = 0.0F;
    float right = 0.0F;
    float front = 0.0F;
    float rear = 0.0F;

    const float comfort = std::clamp(listenerComfort, 0.30F, 1.0F);
    const float immersion = std::clamp(immersionIntensity, 0.1F, 1.5F);
    const float fade = std::clamp(exitFade, 0.0F, 1.0F);

    for (const auto& source : sources) {
        const float distanceAtt = 1.0F / (1.0F + std::max(0.1F, source.distanceMeters));
        const float angleRad = source.azimuthDegrees * 3.14159265F / 180.0F;
        const float pan = std::sin(angleRad);
        const float frontRear = std::cos(angleRad);
        const float weighted = source.baseLoudness * distanceAtt * comfort * immersion * fade;

        left += weighted * (1.0F - pan) * 0.5F;
        right += weighted * (1.0F + pan) * 0.5F;
        if (frontRear >= 0.0F) {
            front += weighted * frontRear;
        } else {
            rear += weighted * (-frontRear);
        }
    }

    return {softNormalize(left), softNormalize(right), softNormalize(front), softNormalize(rear)};
}

} // namespace dnxr
