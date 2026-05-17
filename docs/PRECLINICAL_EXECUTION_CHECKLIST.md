# Preclinical Execution Checklist (Simulation Program)

Date: 2026-05-17
Scope: This checklist prepares simulation evidence before any external preclinical governance review.

## A. Build and baseline integrity
- [ ] Clean configure and build completed.
- [ ] All CTest targets pass.
- [ ] Commit hash and artifact hashes archived.

## B. Safety verification evidence
- [ ] Invalid telemetry triggers emergency latch (`REQ-SAFE-001`).
- [ ] Emergency reason logging retained with timestamps.
- [ ] Safety thresholds and rationale reviewed by Safety Lead.

## C. Timing/performance evidence
- [ ] 20ms loop target configured and validated under nominal load.
- [ ] Runtime metrics exported (avg/max/jitter) across multiple scenarios.
- [ ] Overrun handling behavior documented.

## D. Protocol/interoperability evidence
- [ ] Packet version and length validation tests executed.
- [ ] Negative tests for malformed packet vectors captured.
- [ ] Interop metadata version reference reviewed.

## E. Risk and traceability updates
- [ ] Risk file updated with latest verification outcomes.
- [ ] Traceability matrix updated to reflect implemented/partial requirements.
- [ ] Residual risk board review recorded.

## F. Transfer bundle completeness
- [ ] `NEURALINK_CLINIC_TRANSFER_PACKAGE.md` included.
- [ ] QMS implementation plan included.
- [ ] VMP, risk template, traceability matrix included.
- [ ] Explicit non-clinical simulation declaration included.

## G. Governance stop conditions
Stop and escalate if any of the following occur:
- Any failing safety test.
- Any unresolved high-risk hazard without approved control.
- Missing traceability for critical safety requirements.
- Any attempt to enable real hardware mode in this repository.
