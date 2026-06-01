#include "MultisensoryClock.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {

float clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

double finiteOrDefault(double value, double fallback) {
    return std::isfinite(value) ? value : fallback;
}

float finiteOrDefault(float value, float fallback) {
    return std::isfinite(value) ? value : fallback;
}

} // namespace

MultisensoryAlignmentReport MultisensoryClock::align(
    const std::vector<SensoryChannelPacket>& packets,
    const MultisensoryClockContract& contract) const {

    MultisensoryAlignmentReport report;
    report.alignedFrame.reserve(packets.size());

    if (packets.empty()) {
        report.synchronized = false;
        report.degradationApplied = true;
        report.confidence = 0.0f;
        report.recommendedIntensityScale = 0.0f;
        report.reason = "multisensory_sync_empty_frame";
        return report;
    }

    const double targetTimestampMs = finiteOrDefault(contract.targetTimestampMs, 0.0);
    const double maxPhaseErrorMs = std::max(0.1, finiteOrDefault(contract.maxPhaseErrorMs, 2.0));
    const float maxSafeIntensity = clamp01(finiteOrDefault(contract.maxSafeIntensity, 1.0f));

    float confidenceSum = 0.0f;
    bool sequenceMismatch = false;
    bool nonFiniteOrClamped = false;

    for (const auto& packet : packets) {
        const double packetTimestampMs = finiteOrDefault(packet.timestampMs, targetTimestampMs);
        const double phaseErrorMs = std::abs(packetTimestampMs - targetTimestampMs);
        report.maxObservedPhaseErrorMs = std::max(report.maxObservedPhaseErrorMs, phaseErrorMs);

        if (packet.sequenceId != contract.expectedSequenceId) {
            sequenceMismatch = true;
        }

        const float confidence = sanitizeConfidence(packet.confidence);
        confidenceSum += confidence;

        const float safetyLimit = std::min(clamp01(finiteOrDefault(packet.safetyLimit, 0.0f)), maxSafeIntensity);
        const float sanitizedIntensity = sanitizeIntensity(packet.intensity, safetyLimit);
        if (!std::isfinite(packet.intensity) || sanitizedIntensity != packet.intensity) {
            nonFiniteOrClamped = true;
        }
        report.alignedFrame.push_back(sanitizedIntensity * confidence);
    }

    report.confidence = clamp01(confidenceSum / static_cast<float>(packets.size()));
    const bool phaseViolation = report.maxObservedPhaseErrorMs > maxPhaseErrorMs;
    report.synchronized = !phaseViolation && !sequenceMismatch && report.confidence >= 0.60f;

    if (phaseViolation || sequenceMismatch || report.confidence < 0.60f || nonFiniteOrClamped) {
        report.degradationApplied = true;
        const float phaseScale = phaseViolation
            ? clamp01(static_cast<float>(maxPhaseErrorMs / std::max(report.maxObservedPhaseErrorMs, 0.1)))
            : 1.0f;
        const float sequenceScale = sequenceMismatch ? 0.50f : 1.0f;
        report.recommendedIntensityScale = clamp01(std::min({phaseScale, sequenceScale, report.confidence}));
        for (float& value : report.alignedFrame) {
            value = std::clamp(value * report.recommendedIntensityScale, -report.recommendedIntensityScale, report.recommendedIntensityScale);
        }

        if (sequenceMismatch) {
            report.reason = "multisensory_sync_sequence_mismatch";
        } else if (phaseViolation) {
            report.reason = "multisensory_sync_phase_error";
        } else if (report.confidence < 0.60f) {
            report.reason = "multisensory_sync_low_confidence";
        } else {
            report.reason = "multisensory_sync_safety_clamp";
        }
    }

    return report;
}

MultisensoryAlignmentReport MultisensoryClock::alignVector(
    const std::vector<float>& frame,
    std::uint64_t sequenceId,
    double timestampMs,
    double maxPhaseErrorMs) const {

    std::vector<SensoryChannelPacket> packets;
    packets.reserve(frame.size());
    for (std::size_t i = 0; i < frame.size(); ++i) {
        packets.push_back(SensoryChannelPacket{
            kindForChannelIndex(i),
            sequenceId,
            timestampMs,
            frame[i],
            1.0f,
            1.0f,
            kindName(kindForChannelIndex(i))});
    }

    return align(
        packets,
        MultisensoryClockContract{timestampMs, maxPhaseErrorMs, sequenceId, 1.0f});
}

SensoryChannelKind MultisensoryClock::kindForChannelIndex(std::size_t index) {
    switch (index) {
        case 0:
        case 1:
        case 2:
            return SensoryChannelKind::Visual;
        case 3:
        case 4:
            return SensoryChannelKind::Audio;
        case 5:
        case 6:
            return SensoryChannelKind::Haptic;
        case 7:
            return SensoryChannelKind::Olfactory;
        case 8:
            return SensoryChannelKind::Gustatory;
        case 9:
            return SensoryChannelKind::Proprioceptive;
        case 10:
            return SensoryChannelKind::Vestibular;
        default:
            return SensoryChannelKind::Unknown;
    }
}

std::string MultisensoryClock::kindName(SensoryChannelKind kind) {
    switch (kind) {
        case SensoryChannelKind::Visual:
            return "visual";
        case SensoryChannelKind::Audio:
            return "audio";
        case SensoryChannelKind::Haptic:
            return "haptic";
        case SensoryChannelKind::Olfactory:
            return "olfactory";
        case SensoryChannelKind::Gustatory:
            return "gustatory";
        case SensoryChannelKind::Proprioceptive:
            return "proprioceptive";
        case SensoryChannelKind::Vestibular:
            return "vestibular";
        case SensoryChannelKind::Unknown:
            return "unknown";
    }
    return "unknown";
}

float MultisensoryClock::sanitizeIntensity(float value, float safetyLimit) const {
    const float safeLimit = clamp01(finiteOrDefault(safetyLimit, 0.0f));
    return std::clamp(finiteOrDefault(value, 0.0f), -safeLimit, safeLimit);
}

float MultisensoryClock::sanitizeConfidence(float value) const {
    return clamp01(finiteOrDefault(value, 0.0f));
}

} // namespace dnxr
