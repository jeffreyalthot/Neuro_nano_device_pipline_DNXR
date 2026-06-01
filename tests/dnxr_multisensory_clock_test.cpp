#include "../src/immersion/MultisensoryClock.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_multisensory_clock_test: " << message << '\n';
        return false;
    }
    return true;
}

std::vector<dnxr::SensoryChannelPacket> nominalPackets() {
    return {
        {dnxr::SensoryChannelKind::Visual, 7U, 1000.0, 0.80F, 0.95F, 1.0F, "visual_primary"},
        {dnxr::SensoryChannelKind::Audio, 7U, 1001.0, 0.60F, 0.90F, 1.0F, "audio_spatial"},
        {dnxr::SensoryChannelKind::Haptic, 7U, 999.5, 0.40F, 0.85F, 0.8F, "haptic_pressure"},
        {dnxr::SensoryChannelKind::Vestibular, 7U, 1000.5, 0.30F, 0.90F, 0.7F, "vestibular"}
    };
}

} // namespace

int main() {
    dnxr::MultisensoryClock clock;
    const dnxr::MultisensoryClockContract contract{1000.0, 2.0, 7U, 1.0F};

    const auto nominal = clock.align(nominalPackets(), contract);
    if (!require(nominal.synchronized, "nominal packets should be synchronized") ||
        !require(!nominal.degradationApplied, "nominal packets should not degrade") ||
        !require(nominal.alignedFrame.size() == 4U, "alignment should preserve packet count") ||
        !require(nominal.maxObservedPhaseErrorMs <= 1.0, "nominal phase error should be measured")) {
        return 1;
    }

    auto skewed = nominalPackets();
    skewed[1].timestampMs = 1010.0;
    const auto skewedReport = clock.align(skewed, contract);
    if (!require(!skewedReport.synchronized, "large audio skew should desynchronize") ||
        !require(skewedReport.degradationApplied, "large skew should degrade") ||
        !require(skewedReport.reason == "multisensory_sync_phase_error", "large skew should report phase error") ||
        !require(skewedReport.recommendedIntensityScale < 1.0F, "large skew should reduce intensity")) {
        return 1;
    }

    auto sequenceMismatch = nominalPackets();
    sequenceMismatch[2].sequenceId = 8U;
    const auto mismatchReport = clock.align(sequenceMismatch, contract);
    if (!require(!mismatchReport.synchronized, "sequence mismatch should desynchronize") ||
        !require(mismatchReport.reason == "multisensory_sync_sequence_mismatch", "sequence mismatch should be reported") ||
        !require(mismatchReport.recommendedIntensityScale <= 0.50F, "sequence mismatch should strongly reduce intensity")) {
        return 1;
    }

    auto unsafe = nominalPackets();
    unsafe[0].intensity = std::nanf("");
    unsafe[1].intensity = 3.0F;
    unsafe[1].safetyLimit = 0.25F;
    const auto unsafeReport = clock.align(unsafe, contract);
    if (!require(unsafeReport.degradationApplied, "non-finite and clamped packets should degrade") ||
        !require(unsafeReport.alignedFrame[0] == 0.0F, "non-finite intensity should neutralize") ||
        !require(std::abs(unsafeReport.alignedFrame[1]) <= unsafeReport.recommendedIntensityScale, "clamped intensity should respect degradation scale")) {
        return 1;
    }

    const std::vector<float> vectorFrame{0.1F, 0.2F, 0.3F, 0.4F, 0.5F, 0.6F, 0.7F, 0.8F, 0.9F, 1.0F, 0.25F};
    const auto vectorReport = clock.alignVector(vectorFrame, 11U, 1200.0, 2.0);
    if (!require(vectorReport.synchronized, "vector adapter should produce synchronized packets") ||
        !require(vectorReport.alignedFrame == vectorFrame, "vector adapter should preserve safe aligned channels") ||
        !require(dnxr::MultisensoryClock::kindForChannelIndex(10U) == dnxr::SensoryChannelKind::Vestibular,
                 "channel index 10 should map to vestibular")) {
        return 1;
    }

    return 0;
}
