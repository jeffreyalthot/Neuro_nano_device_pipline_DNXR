#include "full_dive/safety/PerModalityIsolationBroker.h"

namespace dnxr {

FullDiveChipFrame PerModalityIsolationBroker::assemble(NeuralVisualVector visual,
                                                        NeuralAuditoryVector auditory,
                                                        NeuralTouchVector touch,
                                                        NeuralProprioceptionVector proprioception,
                                                        NeuralVestibularVector vestibular,
                                                        NeuralMotorIntentVector motor,
                                                        NeuralOlfactoryVector olfactory,
                                                        NeuralGustatoryVector gustatory,
                                                        std::uint64_t frameId,
                                                        double timestampMs) const {
    FullDiveChipFrame frame;
    frame.frameId = frameId;
    frame.timestampMs = timestampMs;
    frame.visual = visual;
    frame.auditory = auditory;
    frame.touch = touch;
    frame.proprioception = proprioception;
    frame.vestibular = vestibular;
    frame.motorIntent = motor;
    frame.olfactory = olfactory;
    frame.gustatory = gustatory;
    frame.safety.simulationOnly = true;
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.safety.painMappingBlocked = true;
    frame.safety.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    return frame;
}

} // namespace dnxr
