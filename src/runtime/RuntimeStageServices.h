#pragma once

#include "FullDiveSynchronizationEngine.h"
#include "../network/SimulationCryptoTransport.h"
#include "../hardware/NeuralChipHAL.h"
#include "../safety/NeuralIdentityVault.h"
#include "../security/RuntimeSecret.h"

namespace dnxr {

class NanoNodeInterface;
class HostComputerBridge;
class XRHeadsetInterface;
class SignalStreamProcessor;
class FeatureVectorizer;
class StateInferenceEngine;
class NeuralCalibrationEngine;
class CalibrationDriftMonitor;
class ImmersionCore;
class MultisensoryClock;
class XRRenderer;
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

struct RuntimeStageServices {
    NanoNodeInterface& nodes;
    HostComputerBridge& bridge;
    XRHeadsetInterface& headset;
    SignalStreamProcessor& dsp;
    FeatureVectorizer& vectorizer;
    StateInferenceEngine& brain;
    NeuralCalibrationEngine& calibration;
    CalibrationDriftMonitor& driftMonitor;
    ImmersionCore& immersion;
    MultisensoryClock& multisensoryClock;
    XRRenderer& renderer;
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
    DeepImmersionPlatform& deepImmersion;
    FullDiveSynchronizationEngine& fullDiveSynchronization;
    FullDiveSafetyBridge& fullDiveBridge;
    SessionSafetyJournal& safetyJournal;
    SimulationCryptoTransport& crypto;
    RuntimeSecret& neuralTransportSecret;
    NeuralIdentityVault& identityVault;
};

} // namespace dnxr
