#pragma once

#include "RuntimeStageContext.h"
#include "../hardware/NeuralChipHAL.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class BciAcquisitionClockDomain {
    Unknown,
    DeviceMonotonic,
    HostMonotonicSynchronized,
    RecordedNonHumanFixture
};

enum class BciAcquisitionQualityFlag : std::uint32_t {
    None = 0U,
    Dropout = 1U << 0U,
    Overflow = 1U << 1U,
    Corrupt = 1U << 2U,
    TopologyMismatch = 1U << 3U,
    Replay = 1U << 4U,
    Stale = 1U << 5U,
    FutureTimestamp = 1U << 6U,
    ClockSkew = 1U << 7U,
    CadenceViolation = 1U << 8U,
    LengthMismatch = 1U << 9U,
    Unauthenticated = 1U << 10U,
    Unattested = 1U << 11U,
    NonExternal = 1U << 12U,
    SessionMismatch = 1U << 13U,
    Gap = 1U << 14U,
    Duplicate = 1U << 15U
};

constexpr std::uint32_t operator|(BciAcquisitionQualityFlag lhs, BciAcquisitionQualityFlag rhs) noexcept {
    return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs);
}

struct BciSampleLayout {
    std::uint32_t channelCount = 0;
    std::uint32_t samplesPerChannel = 1;
    std::uint32_t stride = 1;
    const char* sampleType = "float32_normalized_nonhuman";
};

struct BciAcquisitionFrame {
    std::string deviceIdentity;
    std::string sessionId;
    std::uint64_t sequence = 0;
    std::int64_t hardwareTimestampNs = 0;
    std::int64_t hostTimestampNs = 0;
    BciAcquisitionClockDomain clockDomain = BciAcquisitionClockDomain::Unknown;
    BciSampleLayout sampleLayout{};
    std::uint64_t channelTopologyDigest = 0;
    std::uint32_t qualityFlags = 0U;
    std::uint32_t dropoutCount = 0U;
    std::uint32_t overflowCount = 0U;
    std::uint64_t packetDigest = 0;
    std::uint64_t authenticationTag = 0;
    bool external = false;
    bool attested = false;
    bool authenticated = false;
    bool recordedNonHumanFixture = false;
    std::vector<float> samples;
};

struct BciAcquisitionReadResult {
    BciAcquisitionFrame frame{};
    bool valid = false;
    bool hold = true;
    std::string failureReason = "not_read";
};

struct BciAcquisitionValidationPolicy {
    std::uint32_t expectedChannelCount = 0U;
    std::int64_t maxAgeNs = 1000000000LL;
    std::int64_t maxFutureNs = 50000000LL;
    std::int64_t maxClockSkewNs = 250000000LL;
    std::int64_t minCadenceNs = 1000000LL;
    std::int64_t maxCadenceNs = 100000000LL;
    bool requireExternal = true;
    bool requireAttested = true;
    bool requireAuthenticated = true;
};

class IBciAcquisitionSource {
public:
    virtual ~IBciAcquisitionSource() = default;
    virtual BciAcquisitionReadResult readBciAcquisition(const RuntimeStageContext& context) noexcept = 0;
};

class BciAcquisitionValidator {
public:
    explicit BciAcquisitionValidator(BciAcquisitionValidationPolicy policy = {}) noexcept;
    BciAcquisitionReadResult validate(BciAcquisitionFrame frame) noexcept;
    void resetForReconnect() noexcept;

private:
    BciAcquisitionValidationPolicy policy_{};
    bool haveLast_ = false;
    std::string sessionId_{};
    std::uint64_t lastSequence_ = 0;
    std::int64_t lastHardwareTimestampNs_ = 0;
    std::uint64_t topologyDigest_ = 0;
};

class ExternalHalBciAcquisitionSource final : public IBciAcquisitionSource {
public:
    ExternalHalBciAcquisitionSource(INeuralChipHAL& hal,
                                    std::string sessionId,
                                    bool allowSimulationNonHumanFixture,
                                    BciAcquisitionValidationPolicy policy = {}) noexcept;
    BciAcquisitionReadResult readBciAcquisition(const RuntimeStageContext& context) noexcept override;

private:
    INeuralChipHAL& hal_;
    std::string sessionId_;
    bool allowSimulationNonHumanFixture_ = false;
    BciAcquisitionValidator validator_;
};

template <std::size_t Capacity>
class BciAcquisitionRingBuffer {
public:
    static_assert(Capacity > 0, "BciAcquisitionRingBuffer capacity must be non-zero");

    bool push(const BciAcquisitionFrame& frame) noexcept {
        if (size_ == Capacity) {
            buffer_[tail_] = frame;
            tail_ = (tail_ + 1U) % Capacity;
            head_ = tail_;
            ++dropped_;
            return false;
        }
        buffer_[tail_] = frame;
        tail_ = (tail_ + 1U) % Capacity;
        ++size_;
        return true;
    }

    bool pop(BciAcquisitionFrame& frame) noexcept {
        if (size_ == 0U) {
            return false;
        }
        frame = buffer_[head_];
        head_ = (head_ + 1U) % Capacity;
        --size_;
        return true;
    }

    std::size_t size() const noexcept { return size_; }
    std::uint64_t droppedCount() const noexcept { return dropped_; }

private:
    std::array<BciAcquisitionFrame, Capacity> buffer_{};
    std::size_t head_ = 0U;
    std::size_t tail_ = 0U;
    std::size_t size_ = 0U;
    std::uint64_t dropped_ = 0U;
};

std::uint64_t bciAcquisitionDigest(const BciAcquisitionFrame& frame) noexcept;

} // namespace dnxr
