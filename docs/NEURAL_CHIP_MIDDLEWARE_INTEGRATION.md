# Neural-chip middleware integration points

Current controlled package version: 0.7.1. The enterprise/QMS transfer index is `docs/RELEASE_0_7_1_QMS_NDA_PACKAGE.md`.

DNXR is a middleware runtime. It does not treat world values as raw signals and it does not bypass the neural chip. The runtime translates virtual world state into separated sensory and motor vectors, builds a readable `.ppp` neural grammar, proposes bounded spike candidates, and lets the neural-chip transport/supervision layer authorize or reject the final communication.

## Principal runtime path

The primary runtime path is:

```text
RuntimePipelineRunner
  -> RuntimeFullDiveStage
  -> RuntimeStimulusStage
  -> INeuralChipHAL / neural-chip transport
```

`RuntimeStimulusStage` is the correct source-code location for final middleware-to-chip communication. It now derives a `PPPGrammarFrame` from the current `FullDiveNeuralFrame` before stimulus translation and transport. That frame is stored on `RuntimePipelineFrame::neuralChipGrammarFrame` for audit and for future transport packet binding.

`RuntimePathLock` decides whether the session is using the secondary simulation runtime or the primary real/runtime-HAL path. Once a connected non-simulation chip or real deployment mode selects the primary path, the lock prevents falling back to the secondary simulation runtime during the same controller lifetime.

Do not add direct brain/chip writes in render, full-dive, sensory, or legacy node code. New real-device communication belongs behind `INeuralChipHAL` implementations and must preserve the existing capability checks, closed-loop feedback requirement, hardware cutoff requirement, ACK review, and safety-supervisor review.

The middleware target cadence is 60 frames per second. Device capability negotiation clamps clinical dispatch to that cadence and records chip topology (`electrodeCount`, `wireCount`, `maxStimulusChannels`) so `RuntimeStimulusStage` can trim bounded stimulus channels to the connected electrode/wire capacity before HAL dispatch. If a packet still exceeds the connected topology, the neural chip/HAL rejects it and returns rejected-channel evidence in the ACK.

3D world data enters through `VirtualWorldDataIngress`, which converts separated visual, audio, tactile, proprioceptive, vestibular, chemical, motor-intent, and reality-anchor fields into `NeuralChipOnlyWorldFrame` before any `.ppp` grammar or spike-thinking stage.

The visual subsystem is not a raw image transport. Visual data must pass through `VirtualWorldDataIngress`, `VisualCorticalWorldEncoder`, the visual QMS compliance decision, `.ppp` grammar interpretation, safety/clinical gates, HAL topology checks, and neural-chip ACK review before it can be treated as authorized communication.

## Secondary neural-chip runtime path

The secondary, more specialized runtime path is:

```text
NeuralChipOnlyFullDivePipeline
  -> Sensory encoders
  -> StimulusBySpikeRuntime
  -> PPPGrammarFrameBuilder
  -> MultiLayerSpikeThinkingEngine
  -> spike candidate / safety / feedback stages
  -> NeuralChipFrameCommitter
  -> NeuralChipBus
```

This path is used as the high-fidelity middleware model for spike-vector processing. `StimulusBySpikeRuntime` builds a `.ppp` grammar after cross-modal coherence binding and before vector-to-spike intent generation. The `MultiLayerSpikeThinkingEngine` then runs per-sense thinking groups over the grammar-bound intent: feature, temporal, safety, and ideal-spike layers exchange simulated internal spike confidence before `EngineSpikeCore` emits lane candidates. The decision trace records both grammar and thinking stages so replay can prove that candidates were interpreted through the active grammar rather than dispatched as raw sensory values.

## `.ppp` responsibility

The `.ppp` grammar is the readable contract between the middleware, the neural chip, and the protected brain-side interface. It identifies modality IDs, channel IDs, semantic paths, safety ceilings, ACK requirements, clinical-gate requirements, feedback expectations, and whether the session is still feedback-only.

The `.ppp` grammar must remain distinct from active spike events:

```text
PPPGrammarFrame = language, safety envelope, modality/channel dictionary
PPPSpikeEvent / candidate = data interpreted through that language
```

If `globalSync01` is below `0.80`, the grammar remains valid but the runtime marks the session feedback-only. Candidate stimuli may only be considered when synchronization is high enough, ACK is present, no emergency latch is active, and clinical/safety guards remain satisfied.

## Neural-chip authorization boundary

DNXR proposes and audits; the neural chip authorizes. A middleware stimulus that passes software validation is still not considered final until the neural-chip transport/HAL accepts it and returns valid ACK/feedback. If a stimulus is invalid, unrecognized, over ceiling, missing grammar context, missing ACK, or unsafe, the neural chip must reject it and the middleware must hold or emergency-stop according to the existing safety decision.

Place real communication adapters here:

- `src/hardware/RealNeuralChipHAL.*` for real transport implementation details.
- `src/hardware/RealNeuralChipTransport.*` for packet-level transport and signatures.
- `src/runtime/RuntimeStimulusStage.cpp` for primary runtime gating before HAL dispatch.
- `src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp` for secondary spike-vector/HIL simulation and replay evidence.
- `src/full_dive/spike_bridge/PPPGrammarFrameBuilder.*` for `.ppp` grammar construction and refresh behavior.

Do not place real communication in sensory encoders or renderers. Encoders produce vectors; the grammar and safety stages interpret them; HAL/transport is the only place that may communicate with a neural chip.
