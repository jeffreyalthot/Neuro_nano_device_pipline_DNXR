# Neural-chip-only pipeline 0.6.3

## Global state

Version 0.6.3 hardens the simulation-only neural-chip pipeline around explicit state transitions. The pipeline distinguishes normal accepted frames, no-op ACK waiting, committed holds, rejected inputs, emergency latch handling, and shutdown no-ops.

## External ACK model

A normal commit moves the state machine into `AwaitingAck`. The only normal path back to `Running` is an explicit simulated ACK injection through `injectSimulatedAck(frameId, quality01)`. `stepDetailed()` does not repair ACK state automatically.

## WaitingForAck no-op

While the ACK deadline has not expired, the state gate returns a no-op wait. No chip-bus submit is performed, replay records are marked `noOp=true`, and session rejected/hold counters are not incremented.

## Hold lifecycle

If the ACK deadline expires or another safety gate requires intervention, the pipeline commits a safety hold through `NeuralChipFrameCommitter`. ACK waiting uses `AwaitingAckWait`; missing ACK timeout uses `AckMissingHold`.

## Replay schema

Replay records now carry `noOp`, `stepOutcome`, `traceCoverageComplete`, and `traceCoverageMissingStage` in addition to path kind, pipeline state, digest, candidate, and committed-or-hold frame evidence.

## Safety case

`ReplayConsistencyVerifier` treats ACK wait and shutdown paths as no-op paths. `PipelineSafetyCaseReportBuilder` counts ACK waits separately from ACK timeout holds so ordinary waiting is not reported as rejection evidence.

## Simulation-only limits

The pipeline remains simulation-only. Runtime configuration keeps stimulus dispatch disabled, raw sensory payloads blocked, pain mapping blocked, and neural-vector ceilings bounded.

## Direct-person-use boundary

This architecture is software evidence for simulation and test harnesses only. It does not authorize direct use on a person, does not define real electrical parameters, and does not permit sensory encoders to access the neural-chip bus directly.
