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

## 0.5.2 hard-realtime closed-loop clinical runtime chain

The clinical runtime path is now required to preserve this measured order without bypasses:

1. Neural acquisition.
2. Signal quality gate and artifact-aware signal processing.
3. Multi-window intent decoding / inference.
4. Patient baseline and telemetry normalization.
5. Runtime body pose extraction from neural, headset/body-sensor, and proprioceptive sources.
6. Virtual body pose provision from the XR body model.
7. Scene and virtual-body contract validation.
8. Typed five-sense neural frame construction.
9. Five-sense fusion and coherence validation.
10. Modality phase lock before any neural stimulation.
11. Body schema validation using non-default perceived/avatar poses.
12. FullDiveSafetySupervisor re-check.
13. Device capability negotiation from INeuralChipHAL.
14. NeuralStimulusTranslator bounded command generation.
15. NeuralChipSafetySupervisor validation.
16. HAL dispatch.
17. Signed ACK verification.
18. Closed-loop per-modality feedback reading.
19. Expected perception comparison and perception divergence detection.
20. Bounded perception correction.
21. RuntimePostCorrectionSafetyStage re-validation.
22. Audit, frame finalization, and decompression if instability persists.

The former research-oriented FullDiveOrchestrator is excluded from clinical builds unless `DNXR_ENABLE_RESEARCH_SPECULATION=ON`; the clinical path is owned by FullDiveClinicalOrchestrator and the runtime stages above.
