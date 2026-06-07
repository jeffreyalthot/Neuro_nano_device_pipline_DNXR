#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <initializer_list>

namespace dnxr {

template <typename T, std::size_t Capacity>
class FixedVector {
public:
    using value_type = T;
    using iterator = typename std::array<T, Capacity>::iterator;
    using const_iterator = typename std::array<T, Capacity>::const_iterator;

    FixedVector() = default;
    FixedVector(std::initializer_list<T> values) { for (const auto& value : values) push_back(value); }
    FixedVector& operator=(std::initializer_list<T> values) { clear(); for (const auto& value : values) push_back(value); return *this; }

    constexpr std::size_t capacity() const noexcept { return Capacity; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == Capacity; }
    void reserve(std::size_t requested) { if (requested > Capacity) throw std::length_error("FixedVector reserve capacity exceeded"); }

    T& operator[](std::size_t index) noexcept { return storage_[index]; }
    const T& operator[](std::size_t index) const noexcept { return storage_[index]; }

    T& at(std::size_t index) {
        if (index >= size_) throw std::out_of_range("FixedVector index out of range");
        return storage_[index];
    }
    const T& at(std::size_t index) const {
        if (index >= size_) throw std::out_of_range("FixedVector index out of range");
        return storage_[index];
    }

    iterator begin() noexcept { return storage_.begin(); }
    iterator end() noexcept { return storage_.begin() + static_cast<std::ptrdiff_t>(size_); }
    const_iterator begin() const noexcept { return storage_.begin(); }
    const_iterator end() const noexcept { return storage_.begin() + static_cast<std::ptrdiff_t>(size_); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    void clear() noexcept { size_ = 0; }
    bool try_push_back(const T& value) {
        if (full()) return false;
        storage_[size_++] = value;
        return true;
    }
    void push_back(const T& value) {
        if (!try_push_back(value)) throw std::length_error("FixedVector capacity exceeded");
    }

private:
    std::array<T, Capacity> storage_{};
    std::size_t size_ = 0;
};

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

constexpr std::size_t MAX_VISUAL_CHANNELS = 16;
constexpr std::size_t MAX_AUDITORY_CHANNELS = 12;
constexpr std::size_t MAX_HAPTIC_CHANNELS = 16;
constexpr std::size_t MAX_PROPRIOCEPTIVE_CHANNELS = 12;
constexpr std::size_t MAX_VESTIBULAR_CHANNELS = 8;
constexpr std::size_t MAX_OLFACTORY_CHANNELS = 8;
constexpr std::size_t MAX_GUSTATORY_CHANNELS = 8;
constexpr std::size_t MAX_INTEROCEPTIVE_CHANNELS = 8;
constexpr std::size_t MAX_REALITY_ANCHOR_CHANNELS = 8;

struct VisualSubchannelLimits {
    float maxBrightness = 0.70f;
    float maxContrast = 0.65f;
    float maxMotionSpeed = 0.45f;
    float maxDepthShift = 0.25f;
    float maxFlickerHz = 3.0f;
    float maxPeripheralInstability = 0.20f;
    float maxFrameToFrameDelta = 0.12f;
};
struct VisualNeuralLayer { FixedVector<NeuroChannelCommand, MAX_VISUAL_CHANNELS> channels; VisualSubchannelLimits limits; };
struct AuditorySubchannelLimits { float maxVolumeDelta=0.10f,maxSpatialShiftPerFrame=0.08f,maxBinauralConflict=0.05f,visualLatencySkewLimitMs=20.0f; bool antiStartleLimiter=true; };
struct AuditoryNeuralLayer { FixedVector<NeuroChannelCommand, MAX_AUDITORY_CHANNELS> channels; AuditorySubchannelLimits limits; };
struct HapticSubchannelLimits { bool noPainMapping=true,noSharpSpike=true,noCrushingSensation=true,noBurnSensation=true,noElectricShockPerception=true,impactToSoftPressureOnly=true; };
struct SomatosensoryLayer { FixedVector<NeuroChannelCommand, MAX_HAPTIC_CHANNELS> channels; HapticSubchannelLimits limits; };
struct ProprioceptiveLayer { FixedVector<NeuroChannelCommand, MAX_PROPRIOCEPTIVE_CHANNELS> channels; bool bodySchemaAnchorOnMismatch=true; };
struct VestibularLayer { FixedVector<NeuroChannelCommand, MAX_VESTIBULAR_CHANNELS> channels; bool lowerPriorityThanSafety=true; bool blockMotionOnVisualConflict=true; bool fallSensationForbidden=true; };
struct OlfactoryLayer { FixedVector<NeuroChannelCommand, MAX_OLFACTORY_CHANNELS> channels; float persistenceBudget=0.25f; };
struct GustatoryLayer { FixedVector<NeuroChannelCommand, MAX_GUSTATORY_CHANNELS> channels; float discreteDoseBudget=0.20f; };
struct ChemosensoryLayer { OlfactoryLayer olfactory; GustatoryLayer gustatory; };
struct InteroceptiveSafetyLayer { FixedVector<NeuroChannelCommand, MAX_INTEROCEPTIVE_CHANNELS> channels; bool patientStopSignalAlive = true; };
struct RealityAnchorLayer { FixedVector<NeuroChannelCommand, MAX_REALITY_ANCHOR_CHANNELS> channels; bool anchorRequired = true; float anchorIntensity = 1.0f; };

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

#if DNXR_ENABLE_RESEARCH_SPECULATION
    std::vector<NeuroChannelCommand> allChannels() const;
#endif
    template <typename Visitor>
    void forEachChannel(Visitor&& visitor) const {
        for (const auto& c : visual.channels) visitor(c);
        for (const auto& c : auditory.channels) visitor(c);
        for (const auto& c : somatosensory.channels) visitor(c);
        for (const auto& c : proprioception.channels) visitor(c);
        for (const auto& c : vestibular.channels) visitor(c);
        for (const auto& c : smellTaste.olfactory.channels) visitor(c);
        for (const auto& c : smellTaste.gustatory.channels) visitor(c);
        for (const auto& c : interoceptiveSafety.channels) visitor(c);
        for (const auto& c : realityAnchor.channels) visitor(c);
    }
    template <typename Visitor>
    void forEachMutableChannel(Visitor&& visitor) {
        for (auto& c : visual.channels) visitor(c);
        for (auto& c : auditory.channels) visitor(c);
        for (auto& c : somatosensory.channels) visitor(c);
        for (auto& c : proprioception.channels) visitor(c);
        for (auto& c : vestibular.channels) visitor(c);
        for (auto& c : smellTaste.olfactory.channels) visitor(c);
        for (auto& c : smellTaste.gustatory.channels) visitor(c);
        for (auto& c : interoceptiveSafety.channels) visitor(c);
        for (auto& c : realityAnchor.channels) visitor(c);
    }
    float maxIntensity() const;
    bool hasRequiredClinicalAnchors() const;
};

} // namespace dnxr
