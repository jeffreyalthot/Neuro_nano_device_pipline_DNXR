# Neuro_nano_device_pipline

A C++17 repository for a **simulated** "nano-device ↔ XR headset ↔ host computer" pipeline focused on deep-immersion experimental R&D.

> **Safety and regulatory status:** this repository is not a certified clinical, regulatory, implant, therapeutic, or deterministic target-hardware stack. It is a software/HIL evidence package for simulation, documentation, QMS preparation, clinical-test acceptance review preparation, and expert preclinical review planning only. It does not authorize human testing.

## Repository structure

- `src/` — modular C++ runtime and evidence/readiness code: core, devices, network, signal, cognition, calibration, immersion, safety, rendering, QMS, readiness, and utilities.
- `tests/` — CTest-registered C++ unit, integration, runtime, HIL, security, and readiness tests.
- `tools/qms/` — generators and checks for traceability, readiness, risk, verification, runtime evidence, and clinical software freeze reports.
- `tools/hil/` — deterministic software HIL scenario, latency, session, and risk-coverage generators.
- `tools/security/` — cybersecurity/SBOM/threat-model evidence generators.
- `qms/` — generated and reviewed QMS artifacts, HIL reports, security reports, release records, review records, risk reviews, verification evidence, and validation evidence.
- `docs/` — clinical readiness, regulatory, SOUP/SBOM, safety, neurorights, and transfer documentation.

## Prerequisites

- CMake 3.16 or newer.
- A C++17 compiler supported by CMake.
- Python 3 for QMS/security/HIL evidence scripts.
- Optional: Ninja or platform build tools if you prefer a non-default CMake generator.

## Normal build, run, and test procedure

Use the normal lane for day-to-day development and verification.

```bash
cmake -S . -B build
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/distributed_neural_xr
```

Expected normal-lane behavior for the 0.4.x software evidence package:

- Normal compilation should complete without errors.
- Normal active CTest tests should pass.
- Long/full controller soak tests are controlled by CMake options and are not all enabled in the normal lane.

## Useful CMake options

```bash
# Default simulation-only guardrails are ON.
cmake -S . -B build -DDNXR_SIMULATION_ONLY=ON

# Enable the extended clinical-readiness validation lane.
cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON
cmake --build build_readiness -j2
ctest --test-dir build_readiness --output-on-failure

# Enable the deterministic controller 10k test target explicitly.
cmake -S . -B build -DDNXR_ENABLE_CONTROLLER_10K_TEST=ON
cmake --build build --target dnxr_runtime_controller_pipeline_10k_cycles_test -j2
```

Important long-lane options:

- `DNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON` enables the readiness validation lane configuration.
- `DNXR_ENABLE_CONTROLLER_10K_TEST=ON` enables the deterministic 10k controller test target.
- `DNXR_ENABLE_CONTROLLER_100K_TEST=ON` enables the deterministic 100k controller readiness test target.
- `DNXR_ENABLE_LONG_REAL_TESTS=ON` enables long real/soak-labelled tests; do not represent these as physical clinical evidence unless actual qualified hardware execution and signed review records exist.

## Runtime 10k evidence procedure

The normal bounded evidence lane may generate `qms/generated/Runtime10kBoundedEvidence.md`. That file is useful for CI/readiness checks but is **not equivalent** to full 10k execution evidence.

To generate full 10k evidence, run the explicit full lane:

```bash
DNXR_CONTROLLER_10K_FULL=1 tools/qms/generate_runtime_10k_full_evidence.py
```

The script refuses to generate `qms/generated/Runtime10kFullEvidence.md` unless `DNXR_CONTROLLER_10K_FULL=1` is set and the full controller test executable actually runs. The generated report must still be reviewed and signed before it can unlock advanced readiness gates.

Runtime lane evidence is intentionally split:

- `qms/generated/ReadinessLaneTranscript-Bounded.md` records bounded/normal readiness evidence.
- `qms/generated/ReadinessLaneTranscript-Full10k.md` records explicit full 10k execution evidence.
- `qms/generated/ReadinessLaneSummary.json` records the bounded/full lane status boundary.

The final reports must never infer full 10k execution from bounded evidence.

## QMS and evidence checks

Run the main generated-document and traceability checks before release review:

```bash
python3 tools/qms/check_required_docs.py
python3 tools/qms/check_traceability_completeness.py
python3 tools/qms/check_placeholder_content.py
python3 tools/qms/check_evidence_artifacts.py
```

Generate or refresh supporting QMS reports:

```bash
python3 tools/qms/generate_traceability_report.py
python3 tools/qms/generate_risk_report.py
python3 tools/qms/generate_verification_index.py
python3 tools/qms/generate_clinical_gap_report.py
python3 tools/qms/generate_clinical_readiness_validation_report.py
python3 tools/qms/generate_clinical_software_freeze_report.py
```

The clinical software freeze report summarizes what is software-complete, what remains open, and which blockers require external evidence such as physical HIL, qualified review, and clinical/regulatory authorization.

### Manifest-driven QMS evidence selection

The 0.4.5 evidence package uses an explicit manifest so QMS generators do **not** guess the current HIL/runtime/security artifact from file modification time.

Primary files:

- `qms/generated/EvidenceIndex.json` — canonical evidence path index.
- `qms/generated/EvidenceManifest_0.4.5.md` — human-readable manifest and selection rule.
- `qms/generated/EvidenceReuseJustification_0.4.5.md` — justification for reusing deterministic software HIL evidence from 0.4.3 when the HIL implementation is unchanged.

Rule for reviewers and contributors:

```text
A QMS/readiness/freeze generator must select artifacts by EvidenceIndex.json key,
not by newest modification time.
```

## HIL software evidence procedure

The deterministic software HIL evidence chain is:

```bash
python3 tools/hil/run_hil_scenarios.py
python3 tools/hil/generate_hil_session_reports.py
python3 tools/hil/generate_hil_latency_report.py
python3 tools/hil/generate_hil_risk_coverage_matrix.py
```

Current HIL evidence locations include:

- `qms/hil/session_reports/HIL-SESSION-0.4.3.md`
- `qms/hil/latency_reports/HIL-LATENCY-0.4.3.md`
- `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.3.csv`

These artifacts are deterministic software HIL evidence only. They do not replace:

- qualified physical HIL bench execution;
- qualified phantom/simulator evidence;
- target-hardware latency measurements;
- raw hardware logs;
- deviation/anomaly records;
- named independent sign-off.

## Cybersecurity evidence procedure

Generate the 0.4.4 cybersecurity evidence package:

```bash
python3 tools/security/generate_sbom_report.py
python3 tools/security/generate_threat_model_review.py
python3 tools/security/generate_vulnerability_scan_report.py
python3 tools/security/generate_key_revocation_evidence.py
python3 tools/security/generate_secure_update_adversarial_report.py
```

Expected artifacts:

- `qms/security/sbom_generated_0.4.4.md`
- `qms/security/threat_model_review_0.4.4.md`
- `qms/security/vulnerability_scan_0.4.4.md`
- `qms/security/key_revocation_scenario_0.4.4.md`
- `qms/security/secure_update_adversarial_test_0.4.4.md`
- `qms/security/vulnerability_disposition_0.4.4.md`

Cybersecurity plans alone cannot unlock clinical readiness. Promotion requires executed evidence, vulnerability disposition, and named cybersecurity review/sign-off.

## Readiness gating model

Advanced readiness decisions must use `EvidenceRepository` plus QMS review/sign-off gates. A `ReadinessScorecard` boolean cannot unlock `ClinicalTestReady` by itself.

The readiness ladder now separates expert clinical-test acceptance review preparation from actual clinical-test readiness:

```text
SimulationOnly
IntegratedSimulation
HILReady
PreclinicalEvidenceComplete
RegulatoryPackageDraftReady
ClinicalSubmissionReady
ClinicalTestAcceptanceReviewReady
ClinicalTestReady
```

`ClinicalTestAcceptanceReviewReady` means the software/QMS package is coherent enough for qualified expert review. `ClinicalTestReady` remains blocked until physical evidence, named sign-offs, regulatory/ethics authorization, and clinical protocol records exist.


Required evidence classes for clinical-test promotion include signed artifacts for:

1. physical HIL session evidence;
2. HIL latency evidence;
3. risk-control verification;
4. full runtime 10k execution;
5. executed cybersecurity evidence;
6. Canada regulatory package;
7. US regulatory package;
8. clinical protocol package;
9. independent sign-off.

High-severity hazard closure is blocked unless the QMS review gate has a complete review record, valid independent sign-off, accepted residual-risk disposition, and executed cybersecurity evidence where applicable.

## Clinical acceptance review package

The 0.4.5 review package collects the documents needed for expert preclinical, physician, cybersecurity, safety, QMS, and regulatory review:

- `docs/CLINICAL_ACCEPTANCE_REVIEW_PACKAGE.md`
- `docs/CANADA_CLINICAL_ACCEPTANCE_REVIEW_PACKAGE.md`
- `docs/US_CLINICAL_ACCEPTANCE_REVIEW_PACKAGE.md`
- `qms/generated/ClinicalAcceptanceReviewIndex.md`

Pending sign-off packets are available under `qms/signoff/`:

- `SO-RISK-0.4.5.md`
- `SO-HIL-0.4.5.md`
- `SO-CYBER-0.4.5.md`
- `SO-SOFTWARE-0.4.5.md`
- `SO-PHYSICIAN-0.4.5.md`
- `SO-REGULATORY-CA-0.4.5.md`
- `SO-REGULATORY-US-0.4.5.md`

Each packet remains `Pending` until a named qualified reviewer records reviewer name, role, decision, accepted residual risk, open issues, signature, and date.

## Clinical-test preparation procedure

No clinical testing is authorized by this repository. The following is a controlled readiness checklist for qualified clinical, regulatory, ethics, and sponsor teams.

### 1. Freeze software evidence

```bash
cmake -S . -B build
cmake --build build -j2
ctest --test-dir build --output-on-failure
python3 tools/qms/generate_clinical_software_freeze_report.py
```

Confirm that open software gaps, non-software gaps, HIL blockers, cybersecurity blockers, and regulatory blockers are explicitly listed in `qms/generated/ClinicalSoftwareFreezeReport.md`.

### 2. Complete preclinical physical evidence

Before any human clinical study planning claim:

- execute physical HIL on qualified bench hardware;
- qualify the phantom/simulator and document configuration control;
- measure p95/p99/worst-case latency on target hardware;
- archive raw logs, operator transcript, deviations, anomalies, and repeatability runs;
- link each run to hazards, risk controls, requirements, and tests;
- obtain named independent safety/risk reviewer sign-off.

### 3. Complete QMS review and sign-off records

Create and maintain review/sign-off records for:

- design review;
- risk review and residual-risk acceptance;
- verification and validation evidence;
- cybersecurity evidence and vulnerability disposition;
- release decision;
- problem reports and open clinical blockers.

Use the code-level QMS model in `src/qms/ReviewRecord.*`, `src/qms/SignOffRecord.*`, and `src/qms/ReviewGate.*` as the software-side gate model.

### 4. Canada investigational testing planning

For Canada, Health Canada describes the Investigational Testing Authorization guidance as covering the ITA review process, post-authorization requirements, required application format/documents, and manufacturer/importer/Health Canada roles and responsibilities: <https://www.canada.ca/en/health-canada/services/drugs-health-products/medical-devices/application-information/guidance-documents/investigational-testing-authorizations-guidance.html>.

Before clinical testing in Canada, qualified owners must prepare and review at least:

- classification and intended-use memo;
- manufacturer/importer responsibilities;
- investigational plan/protocol;
- investigator package;
- informed consent material;
- risk analysis and preclinical evidence package;
- REB workflow and approvals;
- adverse-event/problem reporting procedure;
- post-authorization record-keeping and reporting process.

### 5. United States IDE planning

For the United States, the FDA states that an Investigational Device Exemption allows an investigational device to be used in a clinical study to collect safety and effectiveness data, and that 21 CFR 812 covers device clinical study procedures including applications, sponsor/investigator responsibilities, labeling, records, and reports: <https://www.fda.gov/medical-devices/premarket-submissions-selecting-and-preparing-correct-submission/investigational-device-exemption-ide>.

Before clinical testing in the United States, qualified owners must prepare and review at least:

- IDE/SR-NSR pathway memo;
- investigational plan and protocol;
- IRB package;
- informed consent;
- investigational-use labeling;
- monitoring plan;
- adverse-event and unanticipated-adverse-device-effect reporting process;
- sponsor/investigator responsibility matrix;
- records and reports plan;
- FDA submission/interaction plan when applicable.

### 6. Clinical execution rule

Do not initiate or advertise clinical testing until all required ethics, regulatory, sponsor, investigator, monitoring, informed-consent, labeling, safety, and reporting approvals are complete and documented by qualified parties. This repository can only support evidence preparation; it cannot authorize clinical use.

## Current 0.4.x readiness position

The project has advanced software simulation, deterministic software HIL evidence, manifest-indexed QMS artifacts, ReviewGate/SignOffRecord readiness gating, Runtime10kFullEvidence, clinical software freeze reporting, and a clinical acceptance review package. The honest target state is:

```text
Software/QMS package ready for clinical-test acceptance review.
ClinicalTestReady real-world status: blocked.
```

It remains blocked for clinical-test claims by:

- physical HIL bench evidence;
- phantom qualification;
- named independent review and sign-off;
- full 10k runtime execution evidence when not generated and signed;
- executed cybersecurity evidence review;
- Canada ITA / US IDE qualified regulatory review;
- REB/IRB workflow and authorized clinical protocol execution.

## 0.4.5 acceptance-review checklist

Before presenting the software/QMS package for expert review, refresh and inspect:

```bash
python3 tools/qms/generate_clinical_readiness_validation_report.py
python3 tools/qms/generate_clinical_software_freeze_report.py
python3 tools/qms/check_required_docs.py
python3 tools/qms/check_traceability_completeness.py
python3 tools/qms/check_evidence_artifacts.py
```

Reviewer acceptance should confirm:

- EvidenceIndex paths are correct and stable.
- Bounded and full 10k evidence are not contradictory.
- Reused HIL evidence includes an explicit justification.
- Sign-off packets are ready but still pending until named reviewers sign.
- ClinicalTestReady is not claimed without physical HIL, phantom validation, regulatory/ethics authorization, and protocol approval.

## Windows / MinGW troubleshooting

If you get an error like:

`Le point d'entrée de procédure _ZNKSt25__codecvt_utf8_utf16_baselwE10do_unshiftERiPcS2_RS2_ est introuvable`

the executable was usually built with one MinGW/GCC runtime and started with another `libstdc++-6.dll` at runtime.

Recommended fix:

1. Delete previous build artifacts such as `build/`.
2. Ensure a **single** compiler toolchain is used end-to-end from the same terminal/environment.
3. Reconfigure and rebuild:

   ```bash
   cmake -S . -B build -G "MinGW Makefiles"
   cmake --build build --clean-first
   ```

4. Check which runtime DLL your executable loads and make sure it comes from the same MinGW installation as `g++.exe`.

The project links the C/C++ runtime statically by default on Windows via `DNXR_WINDOWS_PORTABLE_RUNTIME=ON`.

## Important note

Read `TERMS_OF_USE.md` before any use of this project.

## Development and ideas

For development discussions, open issues or proposals with reproducible build/test commands, evidence artifacts, and safety/regulatory limitations clearly identified.

## Neural-Chip Deep Immersion (Vector-only) — 2026 update

This runtime is explicitly **neural-chip vector driven** end-to-end in the software contract.

Key runtime capabilities:

- Neural vector inference with safety diagnostics.
- XR adaptation with visual perception tuning.
- Precision action and full-body control from neural vectors.
- Platform-aware behavior for Windows/Linux/macOS via runtime platform profiles and compile tags.

See `docs/DEEP_IMMERSION_NEURAL_VECTOR_RUNTIME_SPEC.md` for complete technical documentation.
See `docs/NEURAL_CHIP_XR_SOFTWARE_IMPROVEMENT_PLAN.md` for the software improvement backlog.

## Controlled clinical investigation unlock path

This repository supports a locked-by-default clinical investigation path. `DNXR_CLINICAL_TEST_BUILD=ON` compiles the path but does not unlock human use. Runtime unlock requires signed QMS, regulatory/ethics, protocol, site, operator, physician investigator, hardware bench, and session artifacts. See `docs/CLINICAL_UNLOCK_WORKFLOW.md`, `docs/CLINICAL_TEST_MODE.md`, `docs/SESSION_UNLOCK_SPEC.md`, and `docs/CLINIC_OPERATOR_GUIDE.md`.

Use the non-Qt status panel:

```bash
python tools/clinical/dnxr_clinic_unlock_check.py --clinic-unlock clinical_evidence/CLINIC_TEST_UNLOCK.signed.json --session-unlock clinical_sessions/SESSION_UNLOCK.signed.json --trust-store qms/trust/qms_trust_store.json --revocations qms/trust/revoked_keys.json --source-root .
```

## Full Unlocked controlled runtime lane

This repository now includes a controlled **Full Unlocked** software lane for development, simulation, replay, diagnostics, and benchmark work. It exposes modules and diagnostics without removing critical emergency, physiological, signed-evidence, or human-use safety gates.

### Configure and build

```bash
# Debug
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug -DDNXR_BUILD_TESTS=ON
cmake --build build/debug -j

# Release
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DDNXR_BUILD_TESTS=ON
cmake --build build/release -j
```

### Execute modes

```bash
./build/release/distributed_neural_xr --mode standard --headless --cycles=10
./build/release/distributed_neural_xr --mode simulation --headless --cycles=10
./build/release/distributed_neural_xr --full-unlocked --headless --cycles=10
./build/release/distributed_neural_xr --diagnostic --headless --cycles=10
./build/release/distributed_neural_xr --replay --headless --cycles=10
./build/release/distributed_neural_xr --benchmark --headless --cycles=1
```

### Focused validation and benchmark commands

```bash
ctest --test-dir build/debug -R '^(runtime_mode_test|full_unlocked_runtime_test|pipeline_state_machine_test|safety_decision_test|performance_smoke_test|replay_consistency_test|timebase_sync_test)$' --output-on-failure
./build/release/distributed_neural_xr --benchmark --headless --cycles=1
```

See also:

- `docs/FULL_UNLOCKED_MODE.md`
- `docs/ARCHITECTURE.md`
- `docs/PERFORMANCE.md`
- `docs/SAFETY_MODEL.md`
- `docs/TESTING.md`
- `docs/DEPLOYMENT.md`
- `docs/RESEARCH_SPECIALIZATION_REPORT.md`
- `docs/VALIDATION_REPORT.md`


## Canonical Runtime Status (Prompt 18)

DNXR now documents and tests the canonical runtime as a 100% BCI middleware path: structured virtual-world frames enter the runtime, neural-chip feedback/ACK returns from the chip, and headset/VR device dependencies are not required for acquisition, inference, encoding, dispatch, motor decoding, safety, or closed-loop feedback. Observer display support is optional, read-only, and must not influence safety decisions or chip frames.

Real backend operation still requires an external neural-chip protocol implementation, hardware attestation, signed deployment evidence, patient-specific binding/calibration data, and QMS/readiness artifacts. The project is not validated for human use. Chemical channels remain disabled by default. Missing hardware/proof must fail closed rather than falling back to simulation or automatic ACK.
