# Full-Dive Evidence Summary

## Purpose
Summarize capability evidence for full-dive functions and identify blockers before any preclinical or clinical-readiness claim.

## Scope
Covers full-dive capability matrix, session validator, readiness assessor, evidence recorder, runtime full-dive stage, sensory exposure controls, emergency exit, reality anchor, decompression, and temporal coherence components.

## Current status
- Evidence classification: IntegratedSimulation with partial HIL scaffold.
- PreclinicalEvidenceComplete: false.
- ClinicalEvidenceAvailable: false.

## Required evidence
- Capability-by-capability evidence records for visual, auditory, haptic, proprioceptive, vestibular, motor prediction, reality anchor, emergency exit, cognitive reversibility, temporal coherence, calibration stability, sensory exposure control, and decompression.
- Session validation evidence showing rejection of unsafe exposure, missing emergency exit, missing reality anchor, and unstable calibration scenarios.
- HIL scenario evidence for vestibular overload, latency overrun, emergency exit, decompression failure, and reality-anchor failure.

## Available evidence
- `FullDiveCapabilityMatrix` tracks capabilities and evidence levels.
- `FullDiveReadinessAssessor` blocks readiness escalation when required capabilities lack evidence.
- `FullDiveSessionValidator` validates session constraints.
- `FullDiveEvidenceRecorder` records evidence entries for capability-level review.
- Unit tests cover capability matrix, readiness assessor, session validator, bridge intervention, and intervention window logic.

## Missing evidence
- No clinical evidence is present.
- No preclinical non-simulated evidence package is present.
- HIL scenario-to-capability coverage is not yet exported.
- Long vestibular and intervention-window tests are not part of the normal validation lane.

## Linked artifacts
- `docs/FULL_DIVE_DEFINITION.md`
- `docs/FULL_DIVE_LIMITATIONS.md`
- `docs/FULL_DIVE_VALIDATION_PROTOCOL.md`
- `docs/FULL_DIVE_HAZARD_ANALYSIS.md`
- `docs/TRACEABILITY_MATRIX.csv`

## Linked tests
- `dnxr_full_dive_capability_matrix_test`
- `dnxr_full_dive_readiness_assessor_test`
- `dnxr_full_dive_session_validator_test`
- `dnxr_full_dive_bridge_intervention_test`
- `dnxr_full_dive_intervention_window_test`
- Long-run lane: `dnxr_vestibular_exposure_budget_long_test`, `dnxr_full_dive_intervention_window_long_test`

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.
