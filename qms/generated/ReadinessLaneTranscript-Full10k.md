# ReadinessLaneTranscript-Full10k

## Lane status
Status: PASS when `DNXR_CONTROLLER_10K_FULL=1 tools/qms/generate_runtime_10k_full_evidence.py` is executed successfully and `qms/generated/Runtime10kFullEvidence.md` reports `Overall status: PASS`.

## Evidence boundary
This transcript is separate from the bounded lane to prevent contradictions between bounded CI evidence and full 10k execution evidence.

## Expected command
```bash
DNXR_CONTROLLER_10K_FULL=1 tools/qms/generate_runtime_10k_full_evidence.py
```

## Linked artifact
- `qms/generated/Runtime10kFullEvidence.md`.
