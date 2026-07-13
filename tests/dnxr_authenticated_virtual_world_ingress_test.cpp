#include "immersion/VirtualWorldDataIngress.h"

#include <cassert>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace {

dnxr::CanonicalVirtualWorldFrame validWorld(std::uint64_t sequence, double timestampMs) {
    dnxr::VirtualWorldIngressFrame raw;
    raw.frameId = sequence;
    raw.timestampMs = timestampMs;
    raw.view.exposure = 0.5f;
    raw.view.contrast = 0.5f;
    raw.visualField.nearDepthLayer = 0.2f;
    raw.visualField.midDepthLayer = 0.4f;
    raw.visualField.farDepthLayer = 0.6f;
    raw.visualField.retinalStability = 1.0f;
    raw.audio.volume01 = 0.0f;
    raw.touch.pressure01 = 0.1f;
    raw.touch.texture01 = 0.1f;
    raw.body.avatarOwnership01 = 1.0f;
    raw.body.handConfidence01 = 1.0f;
    raw.body.footConfidence01 = 1.0f;
    raw.vestibular.angularVelocity01 = 0.1f;
    raw.vestibular.gravityDirection01 = 0.5f;
    raw.realityAnchor.orientation01 = 1.0f;
    raw.realityAnchor.identityContinuity01 = 1.0f;
    raw.realityAnchor.sessionBoundary01 = 1.0f;
    raw.realityAnchor.externalWorldRecall01 = 1.0f;
    raw.scene.semanticSceneId = "signed_test_scene";
    raw.scene.coordinateFrame = "world_right_handed_y_up";
    raw.scene.unitSystem = dnxr::VirtualWorldUnitSystem::Normalized01;
    raw.environment.ambientLight01 = 0.4f;
    raw.events.push_back({"contact_begin", 0.5f});
    raw.assets.push_back({"asset:surface", "0123456789abcdef0123456789abcdef", 128, true});
    auto frame = dnxr::makeCanonicalVirtualWorldFrame(raw, "authenticated_test_engine");
    frame.metadata.sessionId = "session-A";
    frame.metadata.sourceId = "local-ipc-test";
    frame.metadata.peerCertificateDigest = "peer-cert-digest";
    frame.metadata.authenticationTag = "signed-frame";
    frame.metadata.hostReceiptTimestampMs = timestampMs;
    frame.metadata.deadlineTimestampMs = timestampMs + 50.0;
    frame.bodyContract.avatarBodyProfileHash = 1;
    frame.bodyContract.patientBodyProfileHash = 1;
    frame.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(frame);
    return frame;
}

dnxr::VirtualWorldDataIngress makeIngress() {
    dnxr::VirtualWorldIngressPolicy policy;
    policy.requireAuthentication = true;
    policy.requireMutualAuthentication = true;
    policy.expectedSessionId = "session-A";
    policy.maxFrameBytes = 4096;
    policy.minFrameIntervalMs = 0.0;
    return dnxr::VirtualWorldDataIngress(policy);
}

} // namespace

int main() {
    {
        auto ingress = makeIngress();
        auto accepted = ingress.ingestCanonical(validWorld(1, 100.0), 100.0);
        assert(accepted.accepted);
        assert(!accepted.holdFrame);
        assert(accepted.snapshot.metadata.sessionId == "session-A");
        assert(accepted.world.scene.nearObstacle01 == 0.2f);
    }
    {
        auto ingress = makeIngress();
        auto unsupported = validWorld(1, 100.0);
        unsupported.metadata.schemaVersion = 99;
        unsupported.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(unsupported);
        const auto result = ingress.ingestCanonical(unsupported, 100.0);
        assert(!result.accepted);
        assert(result.reason == "canonical_world_schema_version_unsupported");
    }
    {
        auto ingress = makeIngress();
        auto first = ingress.ingestCanonical(validWorld(1, 100.0), 100.0);
        assert(first.accepted);
        const auto replay = ingress.ingestCanonical(validWorld(1, 101.0), 101.0);
        assert(!replay.accepted);
        assert(replay.duplicate);
        assert(!replay.usedLastSafeWorld);
    }
    {
        auto ingress = makeIngress();
        auto corrupted = validWorld(1, 100.0);
        corrupted.modalities.audio.volume01 = 0.2f;
        const auto result = ingress.ingestCanonical(corrupted, 100.0);
        assert(!result.accepted);
        assert(result.reason == "canonical_world_authentication_failed");
    }
    {
        auto ingress = makeIngress();
        assert(ingress.ingestCanonical(validWorld(1, 100.0), 100.0).accepted);
        const auto gap = ingress.ingestCanonical(validWorld(3, 101.0), 101.0);
        assert(!gap.accepted);
        assert(gap.gap);
        assert(gap.reason == "canonical_world_sequence_gap");
    }
    {
        auto ingress = makeIngress();
        auto assetMismatch = validWorld(1, 100.0);
        assetMismatch.modalities.assets.front().resolved = false;
        assetMismatch.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(assetMismatch);
        const auto result = ingress.ingestCanonical(assetMismatch, 100.0);
        assert(!result.accepted);
        assert(result.schemaIncompatible);
        assert(result.reason == "canonical_world_asset_unresolved_or_hash_invalid");
    }
    {
        auto ingress = makeIngress();
        auto invalidTransform = validWorld(1, 100.0);
        invalidTransform.modalities.scene.coordinateFrame = "ambiguous_left_handed";
        invalidTransform.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(invalidTransform);
        const auto result = ingress.ingestCanonical(invalidTransform, 100.0);
        assert(!result.accepted);
        assert(result.reason == "canonical_world_coordinate_frame_unsupported");
    }
    {
        auto ingress = makeIngress();
        auto invalidValue = validWorld(1, 100.0);
        invalidValue.modalities.view.exposure = std::nanf("");
        invalidValue.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(invalidValue);
        const auto result = ingress.ingestCanonical(invalidValue, 100.0);
        assert(!result.accepted);
        assert(result.reason == "canonical_world_visual_range_or_finite_invalid");
    }
    {
        dnxr::VirtualWorldIngressPolicy policy;
        policy.requireAuthentication = true;
        policy.expectedSessionId = "session-A";
        policy.maxFrameBytes = 128;
        dnxr::VirtualWorldDataIngress ingress(policy);
        auto oversized = validWorld(1, 100.0);
        oversized.metadata.declaredSizeBytes = 1024;
        oversized.metadata.messageDigest = dnxr::canonicalVirtualWorldDigest(oversized);
        const auto result = ingress.ingestCanonical(oversized, 100.0);
        assert(!result.accepted);
        assert(result.reason == "canonical_world_frame_too_large");
    }
    {
        auto ingress = makeIngress();
        const auto result = ingress.disconnectHold(99, 200.0);
        assert(!result.accepted);
        assert(result.holdFrame);
        assert(result.reason == "canonical_world_ingress_source_disconnected");
    }
    {
        auto frameA = validWorld(7, 700.0);
        auto frameB = validWorld(7, 700.0);
        assert(dnxr::canonicalVirtualWorldDigest(frameA) == dnxr::canonicalVirtualWorldDigest(frameB));
        frameB.modalities.touch.pressure01 = 0.2f;
        assert(dnxr::canonicalVirtualWorldDigest(frameA) != dnxr::canonicalVirtualWorldDigest(frameB));
    }
    return 0;
}
