# Runtime10kFullEvidence

## Scope
Clinical-readiness controller-pipeline evidence for an actual 10,000-cycle execution lane forced by `DNXR_CONTROLLER_10K_FULL=1`.

## Results
- Evidence class: full execution
- Target cycles: 10000
- Executed controller cycles: 10000
- Projected cycles: 10000
- State events observed: 10000
- Full controller mode: true
- Fast-readiness bounded mode: false
- Elapsed time: 351911.995 ms
- Average cycle time: 35.191 ms
- p95 cycle time: deterministic aggregate not sampled in this lane; bounded by observed average and controller assertions
- p99 cycle time: deterministic aggregate not sampled in this lane; bounded by observed average and controller assertions
- Dropped frames: 0
- Emergency events: 0
- Memory growth: not measured by this lane; see runtime memory growth tests
- Timeout state: not tripped
- Build ID: 0.4.4-software-evidence-lane
- Operator role: runtime evidence operator
- Reviewer role: independent runtime reviewer - named sign-off required
- Artifact path: qms/generated/Runtime10kFullEvidence.md
- Projected 10k controller milliseconds: 351911.995
- Internal timeout tripped: false
- Overall status: PASS

## Acceptance criteria
- CTest process terminates without external timeout.
- Every executed controller cycle remains nominal.
- Runtime state-event publication is observed for every executed controller cycle.
- The evidence file records whether the lane used bounded fast-readiness mode or full 10k controller mode.
- Bounded projection evidence is not equivalent to full 10k execution evidence.

## Limitations
This is software readiness evidence. It does not replace physical HIL bench timing, target-hardware latency measurement, or named independent sign-off.
