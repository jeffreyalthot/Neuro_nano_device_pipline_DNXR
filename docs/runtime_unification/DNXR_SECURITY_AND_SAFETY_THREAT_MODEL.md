
## Prompt 04 threat-model update — 2026-07-13

The direct graph removes the whole-cycle compatibility adapter as a causal path risk. New mitigations include construction-time registry validation, per-cycle bitset stage order enforcement, no render owner in the executable graph, cumulative phase digesting without raw neural payload logging, and fail-closed transport hold before any uncertified chip commit.

## Prompt 05 threat-model update — 2026-07-13

Removing `RuntimePipelineRunner.cpp` and `RuntimeRenderStage.cpp` from runtime core reduces duplicate-cycle and render-influence attack surface. The real binary symbol scan is now a required guard against reintroducing `RuntimePipelineRunner`, `RuntimeRenderStage::execute`, legacy run inputs, compatibility adapters, or auto-ACK semantics.

## Prompt 06 acquisition threat-model update — 2026-07-13

BCI acquisition now rejects replay/duplicate/gap, stale/future/skew/cadence violations, corrupted digests, unauthenticated/unattested/non-external packets, topology/session mismatches, dropout/overflow, and sample length mismatches. The biometric source remains telemetry-only and cannot become a neural acquisition or command source.

## Prompt 07 threats and mitigations
- Replay, duplicate, corrupt digest, unresolved asset, ambiguous units, invalid coordinate frame, NaN/Inf/range violations, oversized payload, stale/future/deadline misses, and session mix-up are rejected with explicit hold.
- Motor-world egress is intentionally separate and cannot satisfy neural ACK/feedback provenance.

## Prompt 08 threats and mitigations
- Low confidence, corrupt/stale/non-external acquisition, non-finite features, unsigned model manifests, and adversarial saturation are neutralized to hold/stop intent.
- Motor egress cannot be reused as sensory stimulation, ACK, or feedback evidence.
