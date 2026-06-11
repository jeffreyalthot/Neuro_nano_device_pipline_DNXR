# R&D Technical Dossier 0.6.5

## 1. Architecture logicielle
Neural-chip-only pipeline with state machine, explicit ACK, replay schema v2, evidence bundle, QMS controls, and traceability.

## 2. Simulation/HIL/bench objective
This is a Medical Review Candidate / Preclinical R&D Package for simulation, phantom loopback, HIL, and bench review.

## 3. Non-human-use statement
Not authorized for human use. For medical/regulatory review only. Requires external IDE/ITA/IRB/REB approval before any human-subject activity.

## 4. Pipeline neural-chip-only
All release decisions retain simulationOnly=true, benchOnly=true, and neural stimulus dispatch disabled.

## 5. State machine
WaitingForAck remains an explicit no-op unless externally acknowledged; emergency latch recovery requires audited reset.

## 6. External ACK
Valid ACK evidence is external and explicit; automatic recovery is not a release criterion.

## 7. Replay schema v2
Replay artifacts support reproducible software evidence and consistency review.

## 8. QMS evidence
QMS documents, risk files, design review checklists, and release records are referenced for independent review.

## 9. Traceability matrix
TraceabilityCoverageGate checks requirement, hazard, risk control, source, test, evidence, verification, status, and reviewer/signoff coverage.

## 10. Risk controls
Open risks remain tracked in the 0.6.5 open risk register.

## 11. HIL/bench metrics p95/p99
HILBenchMetrics_0_6_5.json is the target artifact for ack, commit, hold, latch, replay, dropped-frame, timeout, fallback, and transition metrics.

## 12. CTest summary
CTestSummary_0_6_5.md is produced by the full release validation runner.

## 13. Static checks
StaticChecks_0_6_5.md is produced by the full release validation runner.

## 14. Open gaps
External hardware bench validation, independent medical/regulatory review, signed external QMS review, and authorized investigation packages remain open unless separately provided.

## 15. Release decision
Medical Review Candidate / Preclinical R&D Package; human-use remains blocked unless independent regulatory, ethics, physician, and hardware approvals are present.
