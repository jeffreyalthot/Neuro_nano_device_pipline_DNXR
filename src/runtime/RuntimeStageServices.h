#pragma once

#include "FullDiveSynchronizationEngine.h"
#include "../core/RuntimeCompositionRoot.h"
#include "../hardware/NeuralChipHAL.h"
#include "IBciAcquisitionSource.h"
#include "../immersion/VirtualWorldDataIngress.h"
#include "../safety/NeuralIdentityVault.h"
#include "../security/RuntimeSecret.h"

namespace dnxr {

class NanoNodeInterface;
class HostComputerBridge;
class IBiometricSource;
class SignalStreamProcessor;
class FeatureVectorizer;
class StateInferenceEngine;
class NeuralCalibrationEngine;
class CalibrationDriftMonitor;
class MultisensoryClock;
class ObserverMirrorSink;
class PhysiologicalGuard;
class EmergencyShutdown;
class CognitiveReversibilitySystem;
class RealityAnchorSystem;
class EnergyThermalOptimizer;
class SafetyGovernor;
class SafetyStateMachine;
class IndependentWatchdog;
class NeuralStimulusSafetyEnvelope;
class DeepImmersionPlatform;
class FullDiveSafetyBridge;
class SessionSafetyJournal;
class PerceivedBodyPoseEstimator;
class VirtualBodyPoseProvider;

struct RuntimeStageServices {
    NanoNodeInterface& nodes;
    HostComputerBridge& bridge;
    IBiometricSource& biometricSource;
    SignalStreamProcessor& dsp;
    FeatureVectorizer& vectorizer;
    StateInferenceEngine& brain;
    NeuralCalibrationEngine& calibration;
    CalibrationDriftMonitor& driftMonitor;
    MultisensoryClock& multisensoryClock;
    ObserverMirrorSink& observerMirror;
    PhysiologicalGuard& guard;
    EmergencyShutdown& emergency;
    CognitiveReversibilitySystem& reversibility;
    RealityAnchorSystem& realityAnchor;
    EnergyThermalOptimizer& thermalOptimizer;
    SafetyGovernor& safetyGovernor;
    SafetyStateMachine& safetyStateMachine;
    IndependentWatchdog& watchdog;
    NeuralStimulusSafetyEnvelope& stimulusEnvelope;
    INeuralChipHAL& neuralChipHal;
    IWorldIngress& worldIngress;
    IMotorWorldEgress& motorWorldEgress;
    IBciAcquisitionSource& bciAcquisitionSource;
    DeepImmersionPlatform& deepImmersion;
    FullDiveSynchronizationEngine& fullDiveSynchronization;
    FullDiveSafetyBridge& fullDiveBridge;
    SessionSafetyJournal& safetyJournal;
    INeuralPacketProtector& packetProtector;
    RuntimeSecret& neuralTransportSecret;
    NeuralIdentityVault& identityVault;
    PerceivedBodyPoseEstimator* perceivedBodyPoseEstimator = nullptr;
    VirtualBodyPoseProvider* virtualBodyPoseProvider = nullptr;
};

} // namespace dnxr
