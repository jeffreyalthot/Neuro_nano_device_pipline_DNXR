#include "FullDiveNeuralFrame.h"

#include <algorithm>

namespace dnxr {

std::vector<NeuroChannelCommand> FullDiveNeuralFrame::allChannels() const {
    std::vector<NeuroChannelCommand> out;
    const auto append = [&out](const auto& layer) { out.insert(out.end(), layer.channels.begin(), layer.channels.end()); };
    append(visual);
    append(auditory);
    append(somatosensory);
    append(proprioception);
    append(vestibular);
    append(smellTaste.olfactory);
    append(smellTaste.gustatory);
    append(interoceptiveSafety);
    append(realityAnchor);
    return out;
}

float FullDiveNeuralFrame::maxIntensity() const {
    float maxValue = 0.0f;
    for (const auto& channel : allChannels()) {
        maxValue = std::max(maxValue, channel.intensity);
    }
    return maxValue;
}

bool FullDiveNeuralFrame::hasRequiredClinicalAnchors() const {
    return safety.clinicalModeLocked && safety.painSimulationBlocked &&
           interoceptiveSafety.patientStopSignalAlive && realityAnchor.anchorRequired;
}

} // namespace dnxr
