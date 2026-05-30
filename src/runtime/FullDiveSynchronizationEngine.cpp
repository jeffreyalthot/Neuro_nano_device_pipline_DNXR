#include "FullDiveSynchronizationEngine.h"

#include <cmath>

namespace neuroxr {

FullDiveSynchronizationEngine::
FullDiveSynchronizationEngine() {

    initialize();
}

FullDiveSynchronizationEngine::
~FullDiveSynchronizationEngine() {

    stop();
}

void FullDiveSynchronizationEngine::
initialize() {

    running_ = false;

    runtimeState_.neuralSynchronization = 1.0f;

    runtimeState_.corticalStability = 1.0f;

    runtimeState_.subjectivePresence = 1.0f;

    runtimeState_.immersionDepth = 0.0f;

    runtimeState_.motorPredictionAccuracy = 1.0f;

    runtimeState_.vestibularStability = 1.0f;

    runtimeState_.emotionalResonance = 0.0f;

    runtimeState_.temporalCoherence = 1.0f;

    runtimeState_.memoryIntegrity = 1.0f;

    runtimeState_.socialBelievability = 1.0f;

    runtimeState_.fullDiveStability = 1.0f;

    runtimeState_.emergencyExitTriggered = false;

    runtimeState_.consciousnessDesynchronization = false;

    runtimeState_.neuralOverloadDetected = false;

    motorPredictionLoop_.initialize(128);

    proprioceptionLayer_.initialize(128);

    oscillationModel_.generateOscillationSignal(
        OscillationBand::DELTA,
        0.5f,
        1.0f);

    oscillationModel_.generateOscillationSignal(
        OscillationBand::THETA,
        0.75f,
        4.0f);

    oscillationModel_.generateOscillationSignal(
        OscillationBand::ALPHA,
        1.0f,
        8.0f);

    oscillationModel_.generateOscillationSignal(
        OscillationBand::BETA,
        0.85f,
        16.0f);

    oscillationModel_.generateOscillationSignal(
        OscillationBand::GAMMA,
        0.65f,
        40.0f);
}

void FullDiveSynchronizationEngine::
start() {

    if(running_)
        return;

    running_ = true;

    runtimeThread_ =
        std::thread(
            &FullDiveSynchronizationEngine::
            runtimeLoop,
            this);
}

void FullDiveSynchronizationEngine::
stop() {

    running_ = false;

    if(runtimeThread_.joinable()) {

        runtimeThread_.join();
    }
}

void FullDiveSynchronizationEngine::
runtimeLoop() {

    auto previous =
        std::chrono::high_resolution_clock::
        now();

    while(running_) {

        auto current =
            std::chrono::high_resolution_clock::
            now();

        float deltaTime =
            std::chrono::duration<float>(
                current -
                previous).count();

        previous = current;

        {
            std::lock_guard<std::mutex>
            guard(runtimeMutex_);

            update(deltaTime);
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }
}

void FullDiveSynchronizationEngine::
update(
    float deltaTime) {

    brainTopology_.simulate(
        deltaTime);

    oscillationModel_.updateOscillation(
        deltaTime);

    motorPredictionLoop_.update(
        deltaTime);

    affectiveEngine_.update(
        deltaTime);

    memoryEngine_.update(
        deltaTime);

    continuityEngine_.update(
        deltaTime);

    timeEngine_.update(
        deltaTime);

    socialEngine_.update(
        deltaTime);

    synchronizeNeuralSystems();

    synchronizePerceptionPipeline();

    synchronizeMotorPrediction();

    synchronizeVestibularFeedback();

    synchronizeEmotionalState();

    synchronizeSubjectiveTime();

    synchronizeMemoryPersistence();

    synchronizeSocialPresence();

    computeRuntimeStability();

    detectNeuralOverload();

    detectConsciousnessDesynchronization();
}

void FullDiveSynchronizationEngine::
synchronizeNeuralSystems() {

    runtimeState_.neuralSynchronization =
        oscillationModel_
        .getState()
        .globalSynchronization;

    runtimeState_.corticalStability =
        brainTopology_
        .computeGlobalCoherence();
}

void FullDiveSynchronizationEngine::
synchronizePerceptionPipeline() {

    runtimeState_.immersionDepth =
        continuityEngine_
        .getState()
        .immersionDepth;
}

void FullDiveSynchronizationEngine::
synchronizeMotorPrediction() {

    runtimeState_.motorPredictionAccuracy =
        motorPredictionLoop_
        .getFrame()
        .globalPredictionAccuracy;
}

void FullDiveSynchronizationEngine::
synchronizeVestibularFeedback() {

    runtimeState_.vestibularStability =
        vestibularEngine_
        .getState()
        .equilibrium;
}

void FullDiveSynchronizationEngine::
synchronizeEmotionalState() {

    runtimeState_.emotionalResonance =
        affectiveEngine_
        .getFrame()
        .affectivePresence;
}

void FullDiveSynchronizationEngine::
synchronizeSubjectiveTime() {

    runtimeState_.temporalCoherence =
        timeEngine_
        .getState()
        .temporalCoherence;
}

void FullDiveSynchronizationEngine::
synchronizeMemoryPersistence() {

    runtimeState_.memoryIntegrity =
        memoryEngine_
        .getState()
        .memoryIntegrity;
}

void FullDiveSynchronizationEngine::
synchronizeSocialPresence() {

    runtimeState_.socialBelievability =
        socialEngine_
        .getState()
        .subconsciousBelievability;
}

void FullDiveSynchronizationEngine::
computeRuntimeStability() {

    runtimeState_.fullDiveStability =

        (runtimeState_.neuralSynchronization * 0.15f)

        +

        (runtimeState_.corticalStability * 0.15f)

        +

        (runtimeState_.subjectivePresence * 0.10f)

        +

        (runtimeState_.immersionDepth * 0.10f)

        +

        (runtimeState_.motorPredictionAccuracy * 0.10f)

        +

        (runtimeState_.vestibularStability * 0.10f)

        +

        (runtimeState_.emotionalResonance * 0.10f)

        +

        (runtimeState_.temporalCoherence * 0.05f)

        +

        (runtimeState_.memoryIntegrity * 0.10f)

        +

        (runtimeState_.socialBelievability * 0.05f);
}

void FullDiveSynchronizationEngine::
detectNeuralOverload() {

    runtimeState_.neuralOverloadDetected =
        runtimeState_.fullDiveStability <
        0.35f;
}

void FullDiveSynchronizationEngine::
detectConsciousnessDesynchronization() {

    runtimeState_.consciousnessDesynchronization =

        runtimeState_.temporalCoherence < 0.25f ||

        runtimeState_.vestibularStability < 0.25f ||

        runtimeState_.motorPredictionAccuracy < 0.25f;
}

void FullDiveSynchronizationEngine::
triggerEmergencyExit() {

    runtimeState_.emergencyExitTriggered =
        true;

    stop();
}

const FullDiveRuntimeState&
FullDiveSynchronizationEngine::
getState() const {

    return runtimeState_;
}

}