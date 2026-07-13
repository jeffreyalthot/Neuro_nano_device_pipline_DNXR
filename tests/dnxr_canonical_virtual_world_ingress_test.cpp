#include "immersion/VirtualWorldDataIngress.h"

#include <cassert>

namespace {
dnxr::CanonicalVirtualWorldFrame validFrame(std::uint64_t sequence, double timestamp) {
    dnxr::VirtualWorldIngressFrame raw;
    raw.frameId = sequence;
    raw.timestampMs = timestamp;
    raw.view.exposure = 0.7f;
    raw.view.contrast = 0.6f;
    raw.visualField.retinalStability = 1.0f;
    raw.audio.volume01 = 0.3f;
    raw.touch.pressure01 = 0.2f;
    raw.body.avatarOwnership01 = 0.9f;
    raw.body.handConfidence01 = 0.8f;
    raw.body.footConfidence01 = 0.8f;
    raw.realityAnchor.orientation01 = 1.0f;
    raw.realityAnchor.identityContinuity01 = 1.0f;
    raw.realityAnchor.sessionBoundary01 = 1.0f;
    raw.realityAnchor.externalWorldRecall01 = 1.0f;
    auto frame = dnxr::makeCanonicalVirtualWorldFrame(raw, "deterministic_engine_test");
    frame.metadata.authenticationTag = "signed-test-frame";
    frame.sceneRisk.allowedInClinicalMode = true;
    frame.bodyContract.avatarBodyProfileHash = 1;
    frame.bodyContract.patientBodyProfileHash = 1;
    return frame;
}
}

int main() {
    dnxr::VirtualWorldIngressPolicy policy;
    policy.requireAuthentication = true;
    policy.chemicalChannelsEnabled = false;
    dnxr::VirtualWorldDataIngress ingress(policy);

    auto accepted = ingress.ingestCanonical(validFrame(1, 100.0), 100.0);
    assert(accepted.accepted);
    assert(!accepted.holdFrame);
    assert(accepted.world.frameId == 1);
    assert(accepted.world.audio.loudness01 == 0.0f); // sanitizer neutralizes disabled research audio before encoders
    assert(accepted.world.odor.presence01 == 0.0f);
    assert(accepted.world.taste.sweet01 == 0.0f);

    auto duplicate = ingress.ingestCanonical(validFrame(1, 101.0), 101.0);
    assert(!duplicate.accepted);
    assert(duplicate.holdFrame);
    assert(duplicate.duplicate);
    assert(duplicate.reason == "canonical_world_duplicate_or_replayed_sequence");
    assert(duplicate.world.odor.presence01 == 0.0f);

    auto chemical = validFrame(2, 102.0);
    chemical.modalities.chemicalPresent = true;
    chemical.modalities.chemical.odorPresence01 = 1.0f;
    auto chemicalRejected = ingress.ingestCanonical(chemical, 102.0);
    assert(!chemicalRejected.accepted);
    assert(chemicalRejected.holdFrame);
    assert(chemicalRejected.reason == "canonical_world_chemical_channels_disabled");

    dnxr::VirtualWorldDataIngress unsafeIngress(policy);
    auto unsafe = validFrame(1, 103.0);
    unsafe.sceneRisk.seizureRisk = 0.9f;
    auto unsafeRejected = unsafeIngress.ingestCanonical(unsafe, 103.0);
    assert(!unsafeRejected.accepted);
    assert(unsafeRejected.reason == "canonical_world_scene_risk_rejected");

    dnxr::VirtualWorldDataIngress missingIngress(policy);
    auto missingAudio = validFrame(1, 104.0);
    missingAudio.modalities.audio.volume01 = 1.0f;
    missingAudio.modalities.audioPresent = false;
    auto missingAccepted = missingIngress.ingestCanonical(missingAudio, 104.0);
    assert(missingAccepted.accepted);
    assert(missingAccepted.world.audio.loudness01 == 0.0f);

    dnxr::VirtualWorldDataIngress staleIngress(policy);
    auto stale = validFrame(1, 105.0);
    auto staleRejected = staleIngress.ingestCanonical(stale, 2000.0);
    assert(!staleRejected.accepted);
    assert(staleRejected.stale);
    assert(staleRejected.reason == "canonical_world_late_for_bci_cycle");

    dnxr::VirtualWorldDataIngress authIngress(policy);
    auto unauthenticated = validFrame(1, 2001.0);
    unauthenticated.metadata.authenticationTag.clear();
    auto authRejected = authIngress.ingestCanonical(unauthenticated, 2001.0);
    assert(!authRejected.accepted);
    assert(authRejected.reason == "canonical_world_authentication_failed");
    return 0;
}
