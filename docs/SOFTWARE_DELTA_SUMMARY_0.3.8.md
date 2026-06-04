# Software Delta Summary 0.3.8

## Added/expanded software areas
- Readiness: `ClinicalTestReadinessAssessor`, `PreclinicalReadinessAssessor`, `RegulatoryEvidenceIndex`, and `ReadinessScorecard`.
- Full-dive: capability matrix, readiness assessor, evidence recorder, and session validator.
- HIL: controller, emulator, stimulus load, fault injection, latency instrumentation, hardware phantom profile, evidence recorder, and readiness assessor.
- Security: audit trail, threat event, SBOM manifest, key lifecycle policy, and secure update policy.
- Runtime evidence: latency profiler, jitter analyzer, WCET recorder, no-allocation zone, and determinism report.

## Verification baseline
- Normal compile and active CTest lane are the software regression baseline.
- Long-run readiness evidence remains separate because several long tests are disabled by default or require explicit readiness options.

## Known limitations
- HIL components remain scaffold-level until scenario reports and risk coverage matrices are added.
- Readiness decisions still require artifact existence, reviewer status, and country-specific evidence profiles.
