# Virtual world data ingress for DNXR middleware

`VirtualWorldDataIngress` is the middleware entry point for 3D virtual-world data. It converts a renderer/game/XR-engine-facing `VirtualWorldIngressFrame` into `NeuralChipOnlyWorldFrame`, the typed frame consumed by the secondary spike-vector runtime and by the same modality encoders used for `.ppp` grammar and spike-thinking.

## Source-code entry point

Place virtual-world data adapters here:

- `src/immersion/VirtualWorldDataIngress.h`
- `src/immersion/VirtualWorldDataIngress.cpp`

External engines should populate `VirtualWorldIngressFrame`; they should not write to neural-chip buses, HALs, or spike lanes directly.

## Modalities accepted

The ingress frame separates every sense before neural translation:

- `view` / `visualField`: 3D camera, exposure, contrast, depth layers, gaze/foveal field, peripheral awareness, retinal stability. These values feed `VisualCorticalWorldEncoder` and its 0.7.1 visual compliance decision before any visual channel can be grammar-bound or sent toward the neural-chip HAL.
- `audio`: volume, frequency bands, attack/sustain, left-right/front-back direction, source distance, speech presence, danger priority.
- `touch`: pressure, contact area, vibration, texture, slip, hardness/softness, safe warmth/cold, hand/foot/body contacts.
- `body`: head/torso/limb pose, hand/foot confidence, avatar ownership, body mismatch for proprioception.
- `vestibular`: linear acceleration, angular velocity, gravity direction, balance error, sickness risk, visual-vestibular mismatch.
- `chemical`: odor identity/presence and taste channels; these remain gated by experimental chemical-channel policy.
- `motor`: intended avatar movement metadata for future intent binding; real-body movement must never be forced by this ingress.
- `realityAnchor`: orientation, identity continuity, session boundary, and external-world recall.

## Pipeline position

```text
3D engine / XR scene
  -> VirtualWorldIngressFrame
  -> VirtualWorldDataIngress
  -> NeuralChipOnlyWorldFrame
  -> sensory encoders
  -> PPPGrammarFrameBuilder
  -> MultiLayerSpikeThinkingEngine
  -> safety / HAL / ACK
```

The ingress layer only normalizes and routes virtual-world data. It does not authorize stimulus dispatch. Dispatch remains gated by `.ppp`, safety validation, runtime path lock, HAL capability checks, and neural-chip ACK.
