#pragma once

#include <cstdint>

namespace dnxr {

constexpr float DNXR_NEURAL_CHIP_ACTIVE_CEILING_01 = 0.80f;

struct NeuralVisualVector {
    float globalLuminance01 = 0.0f;
    float contrast01 = 0.0f;
    float edgeMap01 = 0.0f;
    float motionFlow01 = 0.0f;
    float depthNear01 = 0.0f;
    float depthMid01 = 0.0f;
    float depthFar01 = 0.0f;
    float objectSaliency01 = 0.0f;
    float navigationCorridor01 = 0.0f;
    float hazardMap01 = 0.0f;
    float avatarBodyVisibility01 = 0.0f;
    float textSymbolPriority01 = 0.0f;
    float phospheneDensity01 = 0.0f;
    float phospheneStability01 = 0.0f;
    float flickerRisk01 = 0.0f;
    float visualFatigueRisk01 = 0.0f;
};

struct NeuralAuditoryVector {
    float loudness01 = 0.0f;
    float lowBand01 = 0.0f;
    float midBand01 = 0.0f;
    float highBand01 = 0.0f;
    float envelopeAttack01 = 0.0f;
    float envelopeSustain01 = 0.0f;
    float spatialLeftRight01 = 0.5f;
    float spatialFrontBack01 = 0.5f;
    float distance01 = 0.0f;
    float speechPriority01 = 0.0f;
    float dangerSoundPriority01 = 0.0f;
    float startleRisk01 = 0.0f;
};

struct NeuralTouchVector {
    float contactPressure01 = 0.0f;
    float contactArea01 = 0.0f;
    float vibrationFlutter01 = 0.0f;
    float vibrationTexture01 = 0.0f;
    float slip01 = 0.0f;
    float hardness01 = 0.0f;
    float softness01 = 0.0f;
    float warmth01 = 0.0f;
    float cold01 = 0.0f;
    float rightHandContact01 = 0.0f;
    float leftHandContact01 = 0.0f;
    float footGroundContact01 = 0.0f;
    float bodyCollision01 = 0.0f;
    float painBlocked01 = 1.0f;
    float burnBlocked01 = 1.0f;
    float crushBlocked01 = 1.0f;
    float shockBlocked01 = 1.0f;
    float somatosensoryConfidence01 = 0.0f;
};

struct NeuralProprioceptionVector {
    float headPose01 = 0.0f;
    float torsoPose01 = 0.0f;
    float rightArmPose01 = 0.0f;
    float leftArmPose01 = 0.0f;
    float rightLegPose01 = 0.0f;
    float leftLegPose01 = 0.0f;
    float handPoseConfidence01 = 0.0f;
    float footPoseConfidence01 = 0.0f;
    float avatarOwnership01 = 0.0f;
    float bodySchemaMismatch01 = 0.0f;
};

struct NeuralVestibularVector {
    float linearAcceleration01 = 0.0f;
    float angularVelocity01 = 0.0f;
    float gravityDirection01 = 0.5f;
    float balanceError01 = 0.0f;
    float motionSicknessRisk01 = 0.0f;
    float visualVestibularMismatch01 = 0.0f;
};

struct NeuralMotorIntentVector {
    float moveForward01 = 0.0f;
    float moveBackward01 = 0.0f;
    float strafeLeft01 = 0.0f;
    float strafeRight01 = 0.0f;
    float turnYaw01 = 0.0f;
    float lookPitch01 = 0.0f;
    float rightArmReach01 = 0.0f;
    float leftArmReach01 = 0.0f;
    float rightHandGrip01 = 0.0f;
    float leftHandGrip01 = 0.0f;
    float gaitIntent01 = 0.0f;
    float postureBalance01 = 0.0f;
    float stopIntent01 = 1.0f;
    float confidence01 = 0.0f;
    float tremorRisk01 = 0.0f;
    float spasmRisk01 = 0.0f;
    float fatigueRisk01 = 0.0f;
};

struct NeuralOlfactoryVector {
    float odorPresence01 = 0.0f;
    float odorIdentityCluster01 = 0.0f;
    float pleasantness01 = 0.0f;
    float freshness01 = 0.0f;
    float smokeWarning01 = 0.0f;
    float chemicalWarning01 = 0.0f;
    float decay01 = 0.0f;
    float habituation01 = 0.0f;
    float respirationPhase01 = 0.0f;
    float nauseaRisk01 = 0.0f;
    float trigeminalContaminationRisk01 = 0.0f;
};

struct NeuralGustatoryVector {
    float sweet01 = 0.0f;
    float salty01 = 0.0f;
    float sour01 = 0.0f;
    float bitter01 = 0.0f;
    float umami01 = 0.0f;
    float metallic01 = 0.0f;
    float mouthfeel01 = 0.0f;
    float temperature01 = 0.0f;
    float pleasantness01 = 0.0f;
    float nauseaRisk01 = 0.0f;
    float swallowingConflictRisk01 = 0.0f;
};

struct NeuralRealityAnchorVector {
    float orientation01 = 1.0f;
    float identityContinuity01 = 1.0f;
    float sessionBoundary01 = 1.0f;
    float externalWorldRecall01 = 1.0f;
    bool required = true;
};

struct NeuralSafetyVector {
    bool simulationOnly = true;
    bool patientStopSignalAlive = true;
    bool emergencyCutoffAvailable = true;
    bool painMappingBlocked = true;
    bool rawSensoryPayloadBlocked = true;
    float aggregateRisk01 = 0.0f;
    float activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
};

struct FullDiveChipFrame {
    std::uint64_t frameId = 0;
    double timestampMs = 0.0;
    NeuralVisualVector visual;
    NeuralAuditoryVector auditory;
    NeuralTouchVector touch;
    NeuralProprioceptionVector proprioception;
    NeuralVestibularVector vestibular;
    NeuralMotorIntentVector motorIntent;
    NeuralOlfactoryVector olfactory;
    NeuralGustatoryVector gustatory;
    NeuralRealityAnchorVector realityAnchor;
    NeuralSafetyVector safety;
};

struct ChipReadFrame {
    std::uint64_t frameId = 0;
    NeuralMotorIntentVector motorIntent;
    float signalQuality01 = 0.0f;
};

struct NeuralSafetyFrame {
    std::uint64_t frameId = 0;
    bool holdActive = true;
    const char* reason = "simulation safety hold";
};

struct ChipHealthStatus {
    bool connected = false;
    bool simulationOnly = true;
    bool safeHoldActive = true;
    bool lastFrameAccepted = false;
    float temperatureC = 37.0f;
};

} // namespace dnxr
