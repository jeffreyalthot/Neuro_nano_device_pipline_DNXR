#pragma once

#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/auditory_chip/AuditoryVectorToSpikeAdapter.h"
#include "immersion/SpatialAudioEngine.h"
#include "immersion/VirtualWorldDataIngress.h"

#include <array>
#include <cstdint>

namespace dnxr {

enum class CanonicalAuditoryBciPhase : std::uint8_t {
    MediaValidation = 0,
    DeterministicSegmentation = 1,
    SemanticSpatialization = 2,
    TonotopicEnvelopeMapping = 3,
    PatientCapabilityMapping = 4,
    SafetyPhaseNormalization = 5,
    SpikeCandidateGeneration = 6,
    Count = 7
};

struct CanonicalAuditoryPhaseTrace {
    CanonicalAuditoryBciPhase phase = CanonicalAuditoryBciPhase::MediaValidation;
    const char* name = "media_validation";
    const char* reason = "not_run";
    std::uint32_t budgetUs = 0;
    bool accepted = false;
    float metric01 = 0.0f;
};

struct CanonicalAuditoryBciProfile {
    PatientAuditoryMap patientMap{};
    AuditorySafetyState safety{};
    ChipCapabilities capabilities{};
    bool auditoryModalityAllowed = false;
    bool signedPatientMap = false;
    bool signedTopology = false;
    bool signedCapability = false;
    bool phaseLockVerified = false;
    bool isolationVerified = false;
    bool dosePolicyVerified = false;
    std::uint64_t patientMapDigest = 0;
    std::uint64_t topologyDigest = 0;
    std::uint64_t capabilityDigest = 0;
};

struct CanonicalAuditoryBciResult {
    bool accepted = false;
    bool holdRequired = true;
    bool repaired = false;
    const char* reason = "auditory_bci_not_run";
    std::uint64_t frameId = 0;
    std::uint64_t traceDigest = 0;
    SpatialAudioFrame spatialFrame{};
    NeuralAuditoryVector auditory{};
    ModalityVectorPacket spikeCandidates{};
    std::array<CanonicalAuditoryPhaseTrace, static_cast<std::size_t>(CanonicalAuditoryBciPhase::Count)> phases{};
    std::uint8_t executedPhaseCount = 0;
};

class CanonicalAuditoryBciEncoder {
public:
    CanonicalAuditoryBciResult encode(const CanonicalVirtualWorldFrame& world,
                                      const CanonicalAuditoryBciProfile& profile) const;

private:
    static bool mediaMetadataValid(const CanonicalVirtualWorldFrame& world, const char** reason) noexcept;
    static bool profileValid(const CanonicalAuditoryBciProfile& profile, const char** reason) noexcept;
    static bool capabilityAllowsAuditory(const ChipCapabilities& capabilities) noexcept;
    static SpatialAudioFrame spatialize(const CanonicalVirtualWorldFrame& world) noexcept;
    static std::uint64_t mix(std::uint64_t seed, std::uint64_t value) noexcept;
    static std::uint64_t mixFloat(std::uint64_t seed, float value) noexcept;
    static float c01(float value) noexcept;
};

} // namespace dnxr
