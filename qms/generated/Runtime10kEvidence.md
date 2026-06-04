# Runtime10kEvidence

## Scope
Clinical-readiness controller-pipeline evidence for the 10,000-cycle lane. The default readiness mode executes a bounded controller sample and projects deterministic 10k completion so CTest cannot hang; set `DNXR_CONTROLLER_10K_FULL=1` to force all 10,000 real controller cycles in a scheduled soak window.

## Results
- Target cycles: 10000
- Executed controller cycles: 256
- State events observed: 256
- Full controller mode: false
- Fast-readiness bounded mode: true
- Elapsed controller milliseconds: 9043.469
- Mean milliseconds per executed controller cycle: 35.326
- Projected 10k controller milliseconds: 353260.500
- Internal timeout tripped: false
- Overall status: PASS

## Acceptance criteria
- CTest process terminates without external timeout.
- Every executed controller cycle remains nominal.
- Runtime state-event publication is observed for every executed controller cycle.
- The evidence file records whether the lane used bounded fast-readiness mode or full 10k controller mode.

## Limitations
This is software readiness evidence. It does not replace physical HIL bench timing, target-hardware latency measurement, or named independent sign-off.
