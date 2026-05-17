# Neuralink Clinic Transfer Package (Simulation Build)

## Important status
This codebase is **simulation-only** and is **not clinical-ready**.
It must not be used to drive implants or real patients.

## What is included for external technical review
- Modular software architecture (signal/cognition/immersion/safety/network).
- Automated test suite via CTest (unit + integration smoke).
- Runtime metrics logs (latency/jitter/safety events).
- Safety guard behavior and emergency reason tracking.
- Versioned interop packet format and packet validator.

## Guardrails now enforced in runtime
- `RuntimeController` blocks any attempt to set `DNXR_ALLOW_REAL_HARDWARE=1`.
- Device/bridge/headset runtime configs are forced to simulation mode in this build.

## Missing before any real-world clinical protocol
1. Quality management process (IEC 62304 / ISO 13485)
2. Formal risk management (ISO 14971) and hazard traceability
3. Deterministic real-time proof (worst-case latency under target load)
4. Security hardening and compliance audits
5. Hardware-in-the-loop validation with certified instrumentation
6. Independent pre-clinical and clinical approvals

## Recommended handoff content to clinic reviewers
- Source archive + commit hash history
- Test logs from `ctest --output-on-failure`
- Runtime metric captures under representative simulation scenarios
- Safety event logs and thresholds rationale
- Explicit declaration: simulation scaffold, non-clinical

## Added regulatory preparation artifacts in this repository
- `docs/REGULATORY_QMS_IMPLEMENTATION_PLAN.md`
- `docs/RISK_MANAGEMENT_FILE_TEMPLATE.md`
- `docs/TRACEABILITY_MATRIX.csv`
- `docs/VALIDATION_MASTER_PLAN.md`
- `docs/PRECLINICAL_EXECUTION_CHECKLIST.md`
