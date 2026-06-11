#pragma once

#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "full_dive/motor_chip/MotorCortexReadFrameAdapter.h"
#include "full_dive/olfactory_chip/NeuralOlfactoryEncoder.h"
#include "full_dive/safety/ModalityExposureLedger.h"
#include "full_dive/safety/PerModalityIsolationBroker.h"
#include "full_dive/safety/PerModalitySafetyEnvelope.h"
#include "full_dive/safety/ChipRealityAnchorHardChannel.h"
#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include "full_dive/touch_chip/NeuralBodySenseEncoders.h"
#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/ChipCapabilityContract.h"
#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/PatientModalityPolicyGate.h"
#include "neural_chip/PatientSpecificNeuralMap.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"
#include "neural_chip/NeuralVectorValidator.h"

namespace dnxr {

struct NeuralChipOnlyWorldFrame {
    std::uint64_t frameId = 0;
    double timestampMs = 0.0;
    VirtualSceneFrame scene;
    SpatialAudioFrame audio;
    AvatarContactFrame contacts;
    AvatarPoseFrame pose;
    AvatarMotionFrame motion;
    VirtualOdorField odor;
    VirtualTasteFrame taste;
    PatientVisualMap visualMap;
    PatientAuditoryMap auditoryMap;
    PatientSomatosensoryMap touchMap;
    PatientOlfactoryMap olfactoryMap;
    PatientGustatoryMap gustatoryMap;
    VisualSafetyState visualSafety;
    AuditorySafetyState auditorySafety;
    TouchSafetyState touchSafety;
    OlfactorySafetyState olfactorySafety;
    GustatorySafetyState gustatorySafety;
    NeuralRealityAnchorVector realityAnchor;
    PatientSpecificNeuralMap patientMap = PatientSpecificNeuralMap::simulationDefaults();
    NeuralChipLatencyBudget latencyBudget;
};

class NeuralChipOnlyFullDivePipeline {
public:
    explicit NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus);

    bool step(const NeuralChipOnlyWorldFrame& world);
    const AvatarRigCommandFrame& lastRigCommand() const;

private:
    NeuralChipBus& chipBus_;
    VisualCorticalWorldEncoder visualEncoder_{};
    AuditoryTonotopicEncoder auditoryEncoder_{};
    NeuralVirtualEpidermisEncoder epidermisEncoder_{};
    NeuralProprioceptionEncoder proprioceptionEncoder_{};
    NeuralVestibularEncoder vestibularEncoder_{};
    NeuralOlfactoryEncoder olfactoryEncoder_{};
    NeuralGustatoryEncoder gustatoryEncoder_{};
    MotorIntentDecoder motorDecoder_{};
    AvatarKinematicRigController avatarRig_{};
    PerModalityIsolationBroker isolationBroker_{};
    PatientModalityPolicyGate patientPolicyGate_{};
    MultisensoryPhaseLockOrchestrator sync_{};
    ModalityExposureLedger exposureLedger_{};
    ChipCapabilityContract capabilityContract_{};
    PerModalitySafetyEnvelope safetyEnvelope_{};
    ChipRealityAnchorHardChannel realityAnchor_{};
    FrameLifecycleGuard lifecycleGuard_{};
    SafeChipFrameHoldBuffer holdBuffer_{};
    AvatarRigCommandFrame lastRigCommand_{};
};

} // namespace dnxr
