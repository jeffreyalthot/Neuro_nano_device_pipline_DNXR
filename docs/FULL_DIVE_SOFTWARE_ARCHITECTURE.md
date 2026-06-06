# Full Dive Closed-Loop Neural Immersion Architecture

This release documents a **Full Dive capability path** rather than a clinically proven human-use system. Human stimulation remains locked behind clinical gates and signed evidence.

## Pipeline
1. Acquire neural frame.
2. Validate signal integrity and classify artifacts.
3. Update patient baseline.
4. Decode cognitive/motor state.
5. Predict intended action.
6. Build virtual world update.
7. Build expected perception.
8. Generate typed multisensory `FullDiveNeuralFrame`.
9. Check five-sense coherence.
10. Check body schema coherence.
11. Apply safety envelope, dose limits, latency budget, and reality anchor.
12. Translate to chip packet.
13. Validate device capabilities and hardware cutoff.
14. Dispatch stimulus only if HAL accepts.
15. Read closed-loop neural feedback.
16. Correct perception error gradually.
17. Audit every transition.

## Safety posture
All clinical operations require reality anchoring, no-pain guarantees, locked-in prevention, device attestation, calibration binding, and immediate emergency cutoff.

## 0.5.1 Integrated Full Dive Closed-Loop Runtime

The clinical runtime path is now a single mandatory chain rather than disconnected module inventory:

1. Neural acquisition
2. Signal quality gate
3. Artifact classifier
4. Patient baseline tracker
5. Intent decoder
6. Virtual body model
7. Scene safety contract
8. Expected perception model
9. FullDiveNeuralFrame build
10. Five-sense fusion/coherence
11. Body schema validation
12. FullDiveSafetySupervisor
13. Stimulus translator
14. Device capability limiter
15. HAL dispatch
16. Mandatory signed ACK verification
17. Closed-loop neural feedback
18. Perception correction
19. Audit/finalizer
20. Decompression/reality-anchor path when required

The modern `RuntimePipelineRunner` orders Full Dive coherence and body-schema validation before safety dispatch, then requires HAL ACK and closed-loop feedback before perception correction. The speculative/research orchestrator remains separate from `FullDiveClinicalOrchestrator`, whose policy disables emotion forcing, memory manipulation, subjective time dilation, and subconscious influence for clinical mode.
