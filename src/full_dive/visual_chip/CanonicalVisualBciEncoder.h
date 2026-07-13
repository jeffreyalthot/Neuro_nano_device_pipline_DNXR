#pragma once

#include "full_dive/visual_chip/PhospheneFrameStabilizer.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "full_dive/visual_chip/VisualSceneSemanticExtractor.h"
#include "full_dive/visual_chip/VisualVectorToSpikeAdapter.h"
#include "immersion/VirtualWorldDataIngress.h"

#include <array>
#include <cstdint>

namespace dnxr {

enum class CanonicalVisualBciPhase : std::uint8_t {
    SceneValidation = 0,
    ProjectionReference = 1,
    SemanticFeatureExtraction = 2,
    SpatialTemporalEncoding = 3,
    PatientCapabilityMapping = 4,
    SafetyNormalization = 5,
    SpikeCandidateGeneration = 6,
    Count = 7
};

struct CanonicalVisualPhaseTrace {
    CanonicalVisualBciPhase phase = CanonicalVisualBciPhase::SceneValidation;
    const char* name = "scene_validation";
    const char* reason = "not_run";
    std::uint32_t budgetUs = 0;
    bool accepted = false;
    float metric01 = 0.0f;
};

struct CanonicalVisualBciProfile {
    PatientVisualMap patientMap{};
    VisualSafetyState safety{};
    ChipCapabilities capabilities{};
    bool visualModalityAllowed = false;
    bool signedPatientMap = false;
    bool signedTopology = false;
    bool signedCapability = false;
    std::uint64_t patientMapDigest = 0;
    std::uint64_t topologyDigest = 0;
    std::uint64_t capabilityDigest = 0;
};

struct CanonicalVisualBciResult {
    bool accepted = false;
    bool holdRequired = true;
    const char* reason = "visual_bci_not_run";
    std::uint64_t frameId = 0;
    std::uint64_t traceDigest = 0;
    VisualSceneSemantics semantics{};
    NeuralVisualVector visual{};
    ModalityVectorPacket spikeCandidates{};
    std::array<CanonicalVisualPhaseTrace, static_cast<std::size_t>(CanonicalVisualBciPhase::Count)> phases{};
    std::uint8_t executedPhaseCount = 0;
};

class CanonicalVisualBciEncoder {
public:
    CanonicalVisualBciResult encode(const CanonicalVirtualWorldFrame& world,
                                    const CanonicalVisualBciProfile& profile) const;

private:
    static VirtualSceneFrame sceneFromWorld(const CanonicalVirtualWorldFrame& world) noexcept;
    static bool metadataValid(const CanonicalVirtualWorldFrame& world, const char** reason) noexcept;
    static bool profileValid(const CanonicalVisualBciProfile& profile, const char** reason) noexcept;
    static bool capabilityAllowsVisual(const ChipCapabilities& capabilities) noexcept;
    static std::uint64_t mix(std::uint64_t seed, std::uint64_t value) noexcept;
    static std::uint64_t mixFloat(std::uint64_t seed, float value) noexcept;
    static float c01(float value) noexcept;
};

} // namespace dnxr
