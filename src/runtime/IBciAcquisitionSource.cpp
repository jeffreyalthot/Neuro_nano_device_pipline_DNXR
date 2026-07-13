#include "IBciAcquisitionSource.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace dnxr {
namespace {
constexpr std::uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr std::uint64_t kFnvPrime = 1099511628211ULL;

std::uint64_t mix(std::uint64_t hash, std::uint64_t value) noexcept {
    hash ^= value;
    hash *= kFnvPrime;
    return hash;
}

std::uint64_t hashString(std::uint64_t hash, const std::string& text) noexcept {
    for (unsigned char ch : text) {
        hash = mix(hash, ch);
    }
    return hash;
}

bool fail(BciAcquisitionReadResult& result, BciAcquisitionQualityFlag flag, const char* reason) noexcept {
    result.frame.qualityFlags |= static_cast<std::uint32_t>(flag);
    result.valid = false;
    result.hold = true;
    result.failureReason = reason;
    return false;
}

std::int64_t secondsToNs(double seconds) noexcept {
    if (!std::isfinite(seconds) || seconds <= 0.0) {
        return 0;
    }
    return static_cast<std::int64_t>(seconds * 1000000000.0);
}
} // namespace

std::uint64_t bciAcquisitionDigest(const BciAcquisitionFrame& frame) noexcept {
    std::uint64_t hash = kFnvOffset;
    hash = hashString(hash, frame.deviceIdentity);
    hash = hashString(hash, frame.sessionId);
    hash = mix(hash, frame.sequence);
    hash = mix(hash, static_cast<std::uint64_t>(frame.hardwareTimestampNs));
    hash = mix(hash, static_cast<std::uint64_t>(frame.hostTimestampNs));
    hash = mix(hash, frame.channelTopologyDigest);
    hash = mix(hash, frame.sampleLayout.channelCount);
    hash = mix(hash, frame.sampleLayout.samplesPerChannel);
    hash = mix(hash, frame.sampleLayout.stride);
    hash = mix(hash, frame.dropoutCount);
    hash = mix(hash, frame.overflowCount);
    for (float sample : frame.samples) {
        const auto scaled = static_cast<std::int64_t>(sample * 1000000.0f);
        hash = mix(hash, static_cast<std::uint64_t>(scaled));
    }
    return hash;
}

BciAcquisitionValidator::BciAcquisitionValidator(BciAcquisitionValidationPolicy policy) noexcept
    : policy_(policy) {}

void BciAcquisitionValidator::resetForReconnect() noexcept {
    haveLast_ = false;
    sessionId_.clear();
    lastSequence_ = 0;
    lastHardwareTimestampNs_ = 0;
    topologyDigest_ = 0;
}

BciAcquisitionReadResult BciAcquisitionValidator::validate(BciAcquisitionFrame frame) noexcept {
    BciAcquisitionReadResult result;
    result.frame = std::move(frame);
    result.valid = false;
    result.hold = true;

    if (policy_.requireExternal && !result.frame.external) {
        fail(result, BciAcquisitionQualityFlag::NonExternal, "bci_acquisition_not_external");
        return result;
    }
    if (policy_.requireAttested && !result.frame.attested) {
        fail(result, BciAcquisitionQualityFlag::Unattested, "bci_acquisition_device_not_attested");
        return result;
    }
    if (policy_.requireAuthenticated && !result.frame.authenticated) {
        fail(result, BciAcquisitionQualityFlag::Unauthenticated, "bci_acquisition_packet_not_authenticated");
        return result;
    }
    if (result.frame.sessionId.empty()) {
        fail(result, BciAcquisitionQualityFlag::SessionMismatch, "bci_acquisition_missing_session");
        return result;
    }
    if (result.frame.samples.empty() || result.frame.sampleLayout.channelCount == 0U) {
        fail(result, BciAcquisitionQualityFlag::LengthMismatch, "bci_acquisition_empty_samples");
        return result;
    }
    if (policy_.expectedChannelCount != 0U && result.frame.sampleLayout.channelCount != policy_.expectedChannelCount) {
        fail(result, BciAcquisitionQualityFlag::LengthMismatch, "bci_acquisition_channel_count_mismatch");
        return result;
    }
    const std::uint64_t expectedLength = static_cast<std::uint64_t>(result.frame.sampleLayout.channelCount) *
                                         static_cast<std::uint64_t>(std::max(1U, result.frame.sampleLayout.samplesPerChannel));
    if (result.frame.samples.size() != expectedLength) {
        fail(result, BciAcquisitionQualityFlag::LengthMismatch, "bci_acquisition_sample_length_mismatch");
        return result;
    }
    if (result.frame.dropoutCount > 0U || (result.frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Dropout)) != 0U) {
        fail(result, BciAcquisitionQualityFlag::Dropout, "bci_acquisition_dropout_detected");
        return result;
    }
    if (result.frame.overflowCount > 0U || (result.frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Overflow)) != 0U) {
        fail(result, BciAcquisitionQualityFlag::Overflow, "bci_acquisition_overflow_detected");
        return result;
    }
    if ((result.frame.qualityFlags & static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Corrupt)) != 0U) {
        fail(result, BciAcquisitionQualityFlag::Corrupt, "bci_acquisition_corrupt_packet");
        return result;
    }
    const std::uint64_t computedDigest = bciAcquisitionDigest(result.frame);
    if (result.frame.packetDigest != computedDigest || result.frame.authenticationTag == 0U) {
        fail(result, BciAcquisitionQualityFlag::Corrupt, "bci_acquisition_packet_digest_mismatch");
        return result;
    }
    const std::int64_t skew = result.frame.hostTimestampNs - result.frame.hardwareTimestampNs;
    if (skew > policy_.maxAgeNs) {
        fail(result, BciAcquisitionQualityFlag::Stale, "bci_acquisition_stale_timestamp");
        return result;
    }
    if (-skew > policy_.maxFutureNs) {
        fail(result, BciAcquisitionQualityFlag::FutureTimestamp, "bci_acquisition_future_timestamp");
        return result;
    }
    if (std::llabs(skew) > policy_.maxClockSkewNs) {
        fail(result, BciAcquisitionQualityFlag::ClockSkew, "bci_acquisition_clock_skew_exceeded");
        return result;
    }

    if (haveLast_) {
        if (result.frame.sessionId != sessionId_) {
            fail(result, BciAcquisitionQualityFlag::SessionMismatch, "bci_acquisition_session_mismatch");
            return result;
        }
        if (result.frame.channelTopologyDigest != topologyDigest_) {
            fail(result, BciAcquisitionQualityFlag::TopologyMismatch, "bci_acquisition_topology_mismatch");
            return result;
        }
        if (result.frame.sequence == lastSequence_) {
            fail(result, BciAcquisitionQualityFlag::Duplicate, "bci_acquisition_duplicate_sequence");
            return result;
        }
        if (result.frame.sequence < lastSequence_) {
            fail(result, BciAcquisitionQualityFlag::Replay, "bci_acquisition_replay_sequence");
            return result;
        }
        if (result.frame.sequence > lastSequence_ + 1U) {
            fail(result, BciAcquisitionQualityFlag::Gap, "bci_acquisition_sequence_gap");
            return result;
        }
        const std::int64_t cadence = result.frame.hardwareTimestampNs - lastHardwareTimestampNs_;
        if (cadence <= 0) {
            fail(result, BciAcquisitionQualityFlag::Stale, "bci_acquisition_non_monotonic_hardware_time");
            return result;
        }
        if (cadence < policy_.minCadenceNs || cadence > policy_.maxCadenceNs) {
            fail(result, BciAcquisitionQualityFlag::CadenceViolation, "bci_acquisition_cadence_violation");
            return result;
        }
    }

    haveLast_ = true;
    sessionId_ = result.frame.sessionId;
    lastSequence_ = result.frame.sequence;
    lastHardwareTimestampNs_ = result.frame.hardwareTimestampNs;
    topologyDigest_ = result.frame.channelTopologyDigest;
    result.valid = true;
    result.hold = false;
    result.failureReason.clear();
    return result;
}

ExternalHalBciAcquisitionSource::ExternalHalBciAcquisitionSource(INeuralChipHAL& hal,
                                                                 std::string sessionId,
                                                                 bool allowSimulationNonHumanFixture,
                                                                 BciAcquisitionValidationPolicy policy) noexcept
    : hal_(hal),
      sessionId_(std::move(sessionId)),
      allowSimulationNonHumanFixture_(allowSimulationNonHumanFixture),
      validator_(policy) {}

BciAcquisitionReadResult ExternalHalBciAcquisitionSource::readBciAcquisition(const RuntimeStageContext& context) noexcept {
    const NeuralChipFrame halFrame = hal_.readFrame();
    const DeviceIdentity identity = hal_.deviceIdentity();
    const NeuralChipStatus status = hal_.status();
    BciAcquisitionFrame frame;
    frame.deviceIdentity = identity.deviceIdHash.empty() ? "unknown_device_identity" : identity.deviceIdHash;
    frame.sessionId = identity.clinicalSessionBinding.empty() ? sessionId_ : identity.clinicalSessionBinding;
    frame.sequence = halFrame.sequenceId;
    frame.hardwareTimestampNs = halFrame.monotonicTimestampMs * 1000000LL;
    frame.hostTimestampNs = secondsToNs(context.timestamp);
    if (frame.hostTimestampNs == 0) {
        frame.hostTimestampNs = frame.hardwareTimestampNs;
    }
    frame.clockDomain = halFrame.simulation ? BciAcquisitionClockDomain::RecordedNonHumanFixture : BciAcquisitionClockDomain::DeviceMonotonic;
    frame.samples = halFrame.channels;
    frame.sampleLayout.channelCount = static_cast<std::uint32_t>(halFrame.channels.size());
    frame.sampleLayout.samplesPerChannel = 1U;
    frame.sampleLayout.stride = 1U;
    frame.channelTopologyDigest = mix(kFnvOffset, frame.sampleLayout.channelCount);
    frame.dropoutCount = halFrame.channels.empty() ? 1U : 0U;
    frame.overflowCount = 0U;
    frame.external = !halFrame.simulation || allowSimulationNonHumanFixture_;
    frame.attested = (!status.simulation && identity.signedIdentity) || allowSimulationNonHumanFixture_;
    frame.authenticated = (!identity.certificateHash.empty() && identity.signedIdentity) || allowSimulationNonHumanFixture_;
    frame.recordedNonHumanFixture = halFrame.simulation;
    if (halFrame.signalQuality <= 0.0f) {
        frame.qualityFlags |= static_cast<std::uint32_t>(BciAcquisitionQualityFlag::Corrupt);
    }
    frame.packetDigest = bciAcquisitionDigest(frame);
    frame.authenticationTag = frame.authenticated ? mix(frame.packetDigest, 0xA77E57EDULL) : 0U;
    return validator_.validate(std::move(frame));
}

} // namespace dnxr
