# VE-0.4.1 Normal Lane Verification Evidence

## Verification scope
Normal build, active CTest lane, QMS artifact generation scripts, and generated HIL evidence outputs for the 0.4.1 software evidence update.

## Planned commands
```bash
cmake -S . -B build
cmake --build build -j2
ctest --test-dir build --output-on-failure
python3 tools/hil/run_hil_scenarios.py
```

## Evidence outputs
- qms/hil/session_reports/HIL-SESSION-0.4.1.md
- qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.1.csv
- qms/hil/latency_reports/HIL-LATENCY-0.4.1.md
- qms/generated/HILScenarioEvidenceReport.md

## Decision
Verification record is prepared for execution attachment. It is not a substitute for raw build and test logs.

## Open issues
Attach command transcripts, reviewer decision, and final release trace.

## Signature
Verifier role: QMS software verifier. Signature pending named review.
