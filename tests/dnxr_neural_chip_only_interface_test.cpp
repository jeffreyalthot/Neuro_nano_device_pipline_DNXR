#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/motor_chip/MotorIntentDecoder.h"
#include "full_dive/olfactory_chip/NeuralOlfactoryEncoder.h"
#include "full_dive/safety/PerModalitySafetyEnvelope.h"
#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include "full_dive/touch_chip/NeuralBodySenseEncoders.h"
#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "neural_chip/NeuralChipBus.h"

#include <cassert>

int main() {
    dnxr::FullDiveChipFrame frame;
    frame.frameId = 42;
    frame.timestampMs = 123.0;

    dnxr::VirtualSceneFrame scene;
    scene.luminance01 = 1.0f;
    scene.hazard01 = 0.9f;
    scene.avatarBody01 = 0.7f;
    frame.visual = dnxr::VisualCorticalWorldEncoder{}.encode(scene, {}, {});

    dnxr::SpatialAudioFrame audio;
    audio.loudness01 = 0.95f;
    audio.speech01 = 0.5f;
    frame.auditory = dnxr::AuditoryTonotopicEncoder{}.encode(audio, {}, {});

    dnxr::AvatarContactFrame contact;
    contact.pressure01 = 0.9f;
    contact.requestsPain = true;
    frame.touch = dnxr::NeuralVirtualEpidermisEncoder{}.encode(contact, {}, {});

    dnxr::AvatarPoseFrame pose;
    pose.ownership01 = 1.0f;
    pose.handConfidence01 = 0.9f;
    frame.proprioception = dnxr::NeuralProprioceptionEncoder{}.encode(pose);

    dnxr::AvatarMotionFrame motion;
    motion.visualVestibularMismatch01 = 0.9f;
    motion.angularVelocity01 = 0.8f;
    frame.vestibular = dnxr::NeuralVestibularEncoder{}.encode(motion);

    dnxr::MotorCortexReadFrame motor;
    motor.forward01 = 0.6f;
    motor.stop01 = 0.0f;
    motor.confidence01 = 0.8f;
    frame.motorIntent = dnxr::MotorIntentDecoder{}.decode(motor);

    dnxr::PatientOlfactoryMap olfactoryMap;
    olfactoryMap.experimentalChannelEnabled = true;
    dnxr::VirtualOdorField odor;
    odor.presence01 = 0.8f;
    frame.olfactory = dnxr::NeuralOlfactoryEncoder{}.encode(odor, olfactoryMap, {});

    dnxr::PatientGustatoryMap gustatoryMap;
    gustatoryMap.experimentalChannelEnabled = true;
    dnxr::VirtualTasteFrame taste;
    taste.sweet01 = 0.7f;
    frame.gustatory = dnxr::NeuralGustatoryEncoder{}.encode(taste, gustatoryMap, {});

    dnxr::MultisensoryPhaseLockOrchestrator sync;
    dnxr::PerModalitySafetyEnvelope safety;
    const auto synchronized = sync.synchronize(frame);
    const auto safe = safety.apply(synchronized);

    assert(safety.validates(safe));
    assert(safe.visual.globalLuminance01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    assert(safe.auditory.loudness01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    assert(safe.touch.contactPressure01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    assert(safe.touch.painBlocked01 == 1.0f);
    assert(safe.olfactory.odorPresence01 <= 0.20f);
    assert(safe.gustatory.sweet01 <= 0.20f);
    assert(safe.vestibular.angularVelocity01 == 0.0f);

    dnxr::NeuralChipBus bus;
    assert(bus.submitSensoryFrame(safe));
    assert(bus.getHealthStatus().simulationOnly);
    assert(!bus.getHealthStatus().safeHoldActive);
    assert(bus.readNeuralActivity().motorIntent.moveForward01 == 0.6f);

    dnxr::FullDiveChipFrame unsafe = safe;
    unsafe.safety.rawSensoryPayloadBlocked = false;
    assert(!bus.submitSensoryFrame(unsafe));
    assert(bus.getHealthStatus().safeHoldActive);
    return 0;
}
