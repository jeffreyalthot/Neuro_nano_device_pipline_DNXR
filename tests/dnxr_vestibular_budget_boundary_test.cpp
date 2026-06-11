#include "../src/safety/NeuralStimulusSafetyEnvelope.h"

#include <cassert>
#include <cstddef>

int main() {
    dnxr::UserNeuralCalibrationProfile profile;
    profile.maxDeltaPerFrame = 1.0f;
    profile.maxProprioceptiveIntensity = 0.60f;
    profile.maxVestibularIntensity = 0.35f;

    dnxr::NeuralStimulusSafetyEnvelope envelope(profile);
    dnxr::SafetyDecision decision;
    decision.allowRender = true;
    decision.riskScore = 0.1f;
    decision.intensityScale = 1.0f;

    const auto command = envelope.buildCommand(
        1U,
        {0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 1.0f},
        decision);

    assert(command.allowStimulusDispatch);
    assert(command.modalityIntensities.size() == dnxr::NeuralStimulusSafetyEnvelope::modalityCount());
    assert(command.modalityIntensities[static_cast<std::size_t>(dnxr::SensoryModality::Proprioceptive)] <= 0.60f);
    assert(command.modalityIntensities[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] <= 0.35f);
    assert(dnxr::NeuralStimulusSafetyEnvelope::modalityForChannel(9U) == dnxr::SensoryModality::Proprioceptive);
    assert(dnxr::NeuralStimulusSafetyEnvelope::modalityForChannel(11U) == dnxr::SensoryModality::Vestibular);
    return 0;
}
