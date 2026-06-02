#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include "../config/SafetyProfileLoader.h"

#include "../neuro/BrainStateTopology.h"
#include "../neural/NeuralOscillationModel.h"
#include "../neural/MotorPredictionLoop.h"

#include "../immersion/NeuralPerceptionSynthesizer.h"
#include "../immersion/VestibularEmulationEngine.h"
#include "../immersion/SyntheticProprioceptionLayer.h"

#include "../cognition/SubjectiveContinuityEngine.h"
#include "../cognition/PersistentNeuroCognitiveTwin.h"

#include "../memory/ImmersiveMemoryPersistenceEngine.h"

#include "../affective/AffectiveRealityEngine.h"

#include "../social/SocialPresenceEngine.h"

#include "../time/SubjectiveTimeEngine.h"

namespace dnxr {

struct FullDiveRuntimeState {

    float neuralSynchronization;

    float corticalStability;

    float subjectivePresence;

    float immersionDepth;

    float motorPredictionAccuracy;

    float vestibularStability;

    float emotionalResonance;

    float temporalCoherence;

    float memoryIntegrity;

    float socialBelievability;

    float fullDiveStability;

    bool emergencyExitTriggered;

    bool consciousnessDesynchronization;

    bool neuralOverloadDetected;
};

struct FullDiveRuntimeInput;

enum class FullDiveUpdateMode {
    SimulationOnly,
    RuntimeDriven,
    Hybrid
};

class FullDiveSynchronizationEngine {
public:

    FullDiveSynchronizationEngine();

    ~FullDiveSynchronizationEngine();

    void initialize();

    void start();

    void stop();

    void update(
        float deltaTime);

    void updateFromRuntime(const FullDiveRuntimeInput& input);

    void setUpdateMode(FullDiveUpdateMode mode);
    FullDiveUpdateMode updateMode() const;

    void configureSafetyThresholds(const SafetyRuntimeProfile& profile);

    void synchronizeNeuralSystems();

    void synchronizePerceptionPipeline();

    void synchronizeMotorPrediction();

    void synchronizeVestibularFeedback();

    void synchronizeEmotionalState();

    void synchronizeSubjectiveTime();

    void synchronizeMemoryPersistence();

    void synchronizeSocialPresence();

    void computeRuntimeStability();

    void detectNeuralOverload();

    void detectConsciousnessDesynchronization();

    void triggerEmergencyExit();

    FullDiveRuntimeState
    getState() const;

private:

    void runtimeLoop();
    void updateInternalSimulationLocked(float deltaTime);
    void applyRuntimeInputLocked(const FullDiveRuntimeInput& input);
    void computeRuntimeStabilityLocked();
    void detectNeuralOverloadLocked();
    void detectConsciousnessDesynchronizationLocked();

private:

    std::atomic<bool> running_;

    std::thread runtimeThread_;

    mutable std::mutex runtimeMutex_;

    BrainStateTopology
    brainTopology_;

    NeuralOscillationModel
    oscillationModel_;

    MotorPredictionLoop
    motorPredictionLoop_;

    NeuralPerceptionSynthesizer
    perceptionSynthesizer_;

    VestibularEmulationEngine
    vestibularEngine_;

    SyntheticProprioceptionLayer
    proprioceptionLayer_;

    SubjectiveContinuityEngine
    continuityEngine_;

    PersistentNeuroCognitiveTwin
    neuroCognitiveTwin_;

    ImmersiveMemoryPersistenceEngine
    memoryEngine_;

    AffectiveRealityEngine
    affectiveEngine_;

    SocialPresenceEngine
    socialEngine_;

    SubjectiveTimeEngine
    timeEngine_;

    FullDiveRuntimeState
    runtimeState_;

    SafetyRuntimeProfile safetyProfile_;
    FullDiveUpdateMode updateMode_ = FullDiveUpdateMode::Hybrid;
};

} // namespace dnxr
