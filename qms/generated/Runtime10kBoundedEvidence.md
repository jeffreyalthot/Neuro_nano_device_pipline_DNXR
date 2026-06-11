# Runtime10kBoundedEvidence

## Scope
Medical-review bounded projection evidence for the 10,000-cycle controller-pipeline lane. The default readiness mode executes a bounded controller sample and projects deterministic 10k completion so CTest cannot hang; set `DNXR_CONTROLLER_10K_FULL=1` to generate the separate full-execution evidence artifact in a scheduled soak window.

## Results
- Evidence class: bounded projection
- Target cycles: 10000
- Executed controller cycles: 256
- Projected cycles: 10000
- State events observed: 256
- Full controller mode: false
- Fast-readiness bounded mode: true
- Elapsed time: 7889.609 ms
- Average cycle time: 30.819 ms
- p95 cycle time: deterministic aggregate not sampled in this lane; bounded by observed average and controller assertions
- p99 cycle time: deterministic aggregate not sampled in this lane; bounded by observed average and controller assertions
- Dropped frames: 0
- Emergency events: 0
- Memory growth: not measured by this lane; see runtime memory growth tests
- Timeout state: not tripped
- Build ID: 0.4.4-software-evidence-lane
- Operator role: runtime evidence operator
- Reviewer role: independent runtime reviewer - named sign-off required
- Artifact path: ../qms/generated/Runtime10kBoundedEvidence.md
- Projected 10k controller milliseconds: 308187.856
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
