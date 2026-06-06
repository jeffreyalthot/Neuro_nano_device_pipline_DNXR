#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class FullDiveModality {
    Visual,
    Auditory,
    Somatosensory,
    Proprioceptive,
    Vestibular,
    Olfactory,
    Gustatory,
    InteroceptiveSafety,
    RealityAnchor
};

struct NeuroChannelCommand {
    FullDiveModality modality = FullDiveModality::Visual;
    std::string channelId;
    std::string targetRegion;
    std::string encodingModelId;
    float intensity = 0.0f;
    float frequencyHz = 0.0f;
    float phase = 0.0f;
    std::uint32_t durationMs = 0;
    std::uint32_t rampUpMs = 0;
    std::uint32_t rampDownMs = 0;
    float safetyLimit = 0.0f;
    std::string calibrationProfileId;
    float confidence = 0.0f;
    float latencyBudgetMs = 0.0f;
};

struct VisualNeuralLayer { std::vector<NeuroChannelCommand> channels; };
struct AuditoryNeuralLayer { std::vector<NeuroChannelCommand> channels; };
struct SomatosensoryLayer { std::vector<NeuroChannelCommand> channels; };
struct ProprioceptiveLayer { std::vector<NeuroChannelCommand> channels; };
struct VestibularLayer { std::vector<NeuroChannelCommand> channels; };
struct OlfactoryLayer { std::vector<NeuroChannelCommand> channels; };
struct GustatoryLayer { std::vector<NeuroChannelCommand> channels; };
struct ChemosensoryLayer { OlfactoryLayer olfactory; GustatoryLayer gustatory; };
struct InteroceptiveSafetyLayer { std::vector<NeuroChannelCommand> channels; bool patientStopSignalAlive = true; };
struct RealityAnchorLayer { std::vector<NeuroChannelCommand> channels; bool anchorRequired = true; float anchorIntensity = 1.0f; };

struct SafetyMetadata {
    bool clinicalModeLocked = true;
    bool painSimulationBlocked = true;
    bool emergencyCutoffAvailable = false;
    float aggregateRisk = 1.0f;
    std::string patientCalibrationHash;
    std::string commandHash;
    std::string previousCommandHash;
};

struct FullDiveNeuralFrame {
    std::uint64_t sequenceId = 0;
    std::uint64_t monotonicTimestampMs = 0;
    VisualNeuralLayer visual;
    AuditoryNeuralLayer auditory;
    SomatosensoryLayer somatosensory;
    ProprioceptiveLayer proprioception;
    VestibularLayer vestibular;
    ChemosensoryLayer smellTaste;
    InteroceptiveSafetyLayer interoceptiveSafety;
    RealityAnchorLayer realityAnchor;
    SafetyMetadata safety;

    std::vector<NeuroChannelCommand> allChannels() const;
    float maxIntensity() const;
    bool hasRequiredClinicalAnchors() const;
};

} // namespace dnxr
