#include "../src/safety/NeuralStimulusSafetyEnvelope.h"

#include <cassert>

int main() {
    dnxr::SensoryFrameSchema schema;
    schema.channels = {
        {0U, dnxr::SensoryModality::Auditory, "left_audio", 1.0f},
        {1U, dnxr::SensoryModality::Visual, "left_eye", 1.0f},
        {2U, dnxr::SensoryModality::Vestibular, "vestibular_hint", 2.0f},
    };
    assert(schema.valid());
    assert(schema.modalityForChannel(0U) == dnxr::SensoryModality::Auditory);
    assert(schema.modalityForChannel(2U) == dnxr::SensoryModality::Vestibular);

    dnxr::UserNeuralCalibrationProfile profile;
    profile.maxDeltaPerFrame = 1.0f;
    profile.maxVestibularIntensity = 0.4f;
    dnxr::NeuralStimulusSafetyEnvelope envelope(profile);
    envelope.setFrameSchema(schema);

    dnxr::SafetyDecision decision;
    decision.allowRender = true;
    decision.riskScore = 0.1f;
    const auto command = envelope.buildCommand(
        7U,
        {0.2f, 0.2f, 1.0f},
        decision,
        dnxr::StimulusCoveragePolicy::RequiredModalities,
        {dnxr::SensoryModality::Visual, dnxr::SensoryModality::Auditory});

    assert(command.allowStimulusDispatch);
    assert(!command.fiveSenseCoverage);
    assert(command.coverageReason == "coverage_required_modalities");
    assert(command.modalityIntensities[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] <= 0.2f);
    return 0;
}
