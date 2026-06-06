#include "FullDiveNeuralFrame.h"

#include <algorithm>

namespace dnxr {

std::vector<NeuroChannelCommand> FullDiveNeuralFrame::allChannels() const {
    std::vector<NeuroChannelCommand> out;
    forEachChannel([&out](const NeuroChannelCommand& channel) { out.push_back(channel); });
    return out;
}

float FullDiveNeuralFrame::maxIntensity() const {
    float maxValue = 0.0f;
    forEachChannel([&maxValue](const NeuroChannelCommand& channel) {
        maxValue = std::max(maxValue, channel.intensity);
    });
    return maxValue;
}

bool FullDiveNeuralFrame::hasRequiredClinicalAnchors() const {
    return safety.clinicalModeLocked &&
           safety.painSimulationBlocked &&
           safety.emergencyCutoffAvailable &&
           interoceptiveSafety.patientStopSignalAlive &&
           realityAnchor.anchorRequired &&
           realityAnchor.anchorIntensity > 0.0f;
}

} // namespace dnxr
