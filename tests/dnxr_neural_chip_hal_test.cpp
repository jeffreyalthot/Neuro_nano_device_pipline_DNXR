#include "../src/hardware/NeuralChipHAL.h"
#include "../src/safety/NeuralStimulusSafetyEnvelope.h"
#include "../src/utils/TimeSync.h"

#include <cassert>
#include <stdexcept>

int main() {
    dnxr::TimeSync::reset();

    dnxr::SimulatedNeuralChipHAL hal(16);
    assert(!hal.status().connected);

    hal.connect("simulated_chip_unit_test");
    assert(hal.status().connected);
    assert(hal.status().simulation);

    const auto frame = hal.readFrame();
    assert(frame.sequenceId == 1U);
    assert(frame.channels.size() == 16U);
    const auto capabilities = hal.capabilities();
    assert(capabilities.electrodeCount == 16U);
    assert(capabilities.wireCount == 1U);
    assert(capabilities.maxStimulusChannels == 16U);
    assert(capabilities.maxSafeRateHz >= dnxr::DNXR_NEURAL_CHIP_TARGET_FRAME_RATE_HZ);
    assert(frame.signalQuality > 0.0F);
    assert(frame.monotonicTimestampMs > 0);

    dnxr::NeuralStimulusSafetyEnvelope envelope(
        dnxr::UserNeuralCalibrationProfile{"unit_calibration", 0.5F, 0.45F, 0.4F, 0.2F, 0.3F, 0.15F});
    dnxr::SafetyDecision decision{};
    decision.reason = "nominal";
    decision.riskScore = 0.2F;
    decision.intensityScale = 0.8F;
    decision.allowRender = true;

    dnxr::SafeNeuralStimulusCommand command =
        envelope.buildCommand(frame.sequenceId, {1.0F, -1.0F, 0.3F, 0.9F, 0.1F, 0.8F, 0.1F, 0.6F, 0.6F, 0.7F, 0.5F, 0.5F}, decision);
    assert(command.allowStimulusDispatch);
    assert(command.boundedChannels[0] <= 0.15F);
    assert(command.modalityIntensities.size() == dnxr::NeuralStimulusSafetyEnvelope::modalityCount());
    assert(command.fiveSenseCoverage);
    assert(command.aggregateSensoryLoad > 0.0F);
    assert(command.calibrationProfileId == "unit_calibration");
    assert(envelope.lastModalityBudget().hasFiveSenseCoverage);
    assert(envelope.lastModalityBudget().channelCount[
        static_cast<std::size_t>(dnxr::SensoryModality::Visual)] == 3U);
    assert(dnxr::NeuralStimulusSafetyEnvelope::modalityForChannel(8U) ==
           dnxr::SensoryModality::ChemicalTasteSmell);
    assert(dnxr::NeuralStimulusSafetyEnvelope::modalityForChannel(9U) ==
           dnxr::SensoryModality::Proprioceptive);
    assert(hal.writeSafeStimulus(command));
    assert(hal.lastAcceptedStimulus().sequenceId == frame.sequenceId);

    dnxr::SimulatedNeuralChipHAL limitedHal(4);
    limitedHal.connect("limited_topology_chip");
    assert(!limitedHal.writeSafeStimulus(command));
    assert(limitedHal.lastAck().packetRejected);
    assert(limitedHal.lastAck().rejectedChannelCount > 0U);

    decision.riskScore = 0.95F;
    command = envelope.buildCommand(frame.sequenceId + 1U, {0.1F, 0.2F}, decision);
    assert(!command.allowStimulusDispatch);
    assert(!command.fiveSenseCoverage);
    assert(!hal.writeSafeStimulus(command));

    command.allowStimulusDispatch = true;
    command.riskScore = 0.2F;
    command.modalityIntensities.clear();
    assert(!hal.writeSafeStimulus(command));

    hal.emergencyStop("unit_test_stop");
    assert(hal.status().emergencyLatched);
    assert(!hal.status().connected);
    assert(hal.readFrame().channels.empty());

    bool rejected = false;
    try {
        dnxr::SimulatedNeuralChipHAL locked(4, dnxr::NeuralChipRuntimeMode::ClinicalLocked);
        locked.connect("locked_chip");
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    return 0;
}
