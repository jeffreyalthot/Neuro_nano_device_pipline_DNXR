# Deep Immersion Neural Vector Runtime Specification

## Scope
This document specifies the current software runtime for deep XR immersion powered by **neural chip vectors** only.

## Core Principles
1. Vector-only neural input contract.
2. No EEG-device dependency in runtime interfaces.
3. Safety-first immersion scaling under low-confidence neural streams.
4. Cross-platform portability: Windows, Linux, macOS.

## Runtime Pipeline
1. `NeuralChipSignalAcquisition::ingestFrame` clamps raw neural vectors.
2. `NoiseReductionPipeline::denoise` smooths frame noise.
3. `DeepImmersionPlatform::runInferenceTick` computes cognitive state and diagnostics.
4. `AdaptiveRealityModel::adapt` maps state to XR adaptation (including visual perception controls).
5. `runActionTick` and `runFullBodyControlTick` derive movement and full-body commands.

## Safety Diagnostics
- `neuralSignalQuality` in [0..1]
- `neuralSignalVariance` in [0..1]
- `safetyMode`: `nominal` or `stability_guard`
- `visualHarmonyIndex` in [0..1]
- `intentActionToken` with the decoded XR ontology token
- `intentAmbiguityMargin` in [0..1]
- `intentAmbiguityDetected` and `intentFallbackRecommended`
- `intentConfirmationCount`, `intentExecutionAuthorized`, and `authorizedIntentToken`

When quality/variance are too low, `stability_guard` is activated to reduce aggressiveness and preserve stable immersion. When an intent is ambiguous or below the safe confidence floor, `degraded_fallback` activates non-neural fallback channels and limits action/full-body command gain. Critical actions (`xr.grab`, `xr.compose`, `xr.interrupt`, `xr.cancel`) require two consecutive matching intent ticks before execution; until then, `intent_confirmation_hold` keeps controls gated while diagnostics accumulate confirmation evidence.

## Visual Perception Quality Model
`XRAdaptationFrame` now includes:
- `visualClarity`
- `depthPrecision`
- `motionStability`
- `contrastEnhancement`

A derived harmony score controls shader profile transition for deeper and more coherent visual immersion.


## Neural Intent Schema and Ambiguity Handling
`NeuralIntentProtocol` emits `neuro.intent.embedding.v1.1` by default while preserving compatibility with `neuro.intent.embedding.v1`.

The v1.1 ontology covers the primary XR action tokens:
- `xr.grab`
- `xr.rotate`
- `xr.interrupt`
- `xr.cancel`
- `xr.move`
- `xr.look`
- `xr.compose`

Each decoded vector carries a normalized intensity, confidence, action token, ambiguity margin, and fallback recommendation. If the best two candidate actions are separated by less than the ambiguity threshold, the protocol returns `none`, caps confidence for safety fallback, and recommends non-neural fallback channels before execution.

## Full-Body Neural Control Model
`XRFullBodyCommand` outputs:
- Head: yaw, pitch
- Torso: yaw, lean
- Arms/hands: lift + grip
- Legs/locomotion: stride, forward/lateral movement
- `balanceAssist` for postural stabilization in deep immersion loops.

Temporal smoothing (`smoothFullBodyCommand`) is applied to improve precision and reduce jitter.

## Cross-platform Runtime Adaptation
Build tags:
- `DNXR_TARGET_OS_WINDOWS`
- `DNXR_TARGET_OS_LINUX`
- `DNXR_TARGET_OS_MACOS`

Runtime profile:
- `RuntimePlatformProfile::{Windows,Linux,MacOS,Generic}`

The profile influences control gain calibration to keep control feel consistent across OS targets.


## Virtual Environment View Frame
`VirtualWorldState` now produces a concrete `VirtualViewFrame` for the visible XR environment, not only abstract sensory channels. The view frame contains:
- camera position and head orientation
- field of view, near/far clipping planes, stereo separation, and convergence
- exposure, contrast, saturation, fog, particle density, horizon depth, and frame stability
- scene, biome, and lighting identifiers for the render surface

`ImmersionCore::getLastVirtualViewFrame` exposes the latest view after each neural-chip immersion tick, and `XRRenderer::presentVirtualView` converts it into normalized render channels for presentation. This creates a concrete virtual-environment view path: neural state → world pose/biome → view/projection frame → XR renderer.


## Inside-World Visual Field
`VirtualWorldState` also builds a `VisualFieldFrame` for the field of vision experienced from inside the environment. It models:
- horizontal/vertical field of view and binocular eye offsets
- focal depth, foveal radius, peripheral awareness, peripheral blur, and vignette strength
- near/mid/far depth layers, gaze focus, motion comfort, and retinal stability

`ImmersionCore::getLastVisualFieldFrame` exposes the latest visual field and `XRRenderer::presentVisualField` presents it as normalized channels. This complements `VirtualViewFrame`: the view frame defines the camera/projection surface, while the visual field defines what the immersed user can actually perceive inside that surface.


## Atomic Immersive Render Frame
The runtime now composes sensory channels, `VirtualViewFrame`, and `VisualFieldFrame` into one atomic render packet:
- 11 neural sensory channels
- 16 camera/projection/environment view channels
- 16 inside-world visual-field channels

`ImmersionCore::getLastCompositeRenderFrame` exposes the latest 43-channel packet and frame callbacks now receive this composite packet. `XRRenderer::composeImmersiveRenderFrame` and `presentImmersiveFrame` provide the renderer-side equivalent so the XR surface can present one coherent immersion frame rather than three independent packets. `XRRenderer::validateImmersiveRenderFrame` rejects packets unless they are finite, normalized, exactly layout-compatible with the 43-channel contract, and above the configured frame energy threshold before presentation; dropped-frame counters, rejection reasons, target frame interval, frame time, rolling average, jitter, deadline misses, and the adaptive quality profile (`full_quality`, `timing_recovery`, `drop_recovery`) and recovery-frame countdown are exposed for QoS telemetry. Recovery holds keep foveation/reprojection active for a short stabilization window after drops or timing pressure before returning to full quality. `XRRenderer::getTelemetrySnapshot` aggregates validation, timing, quality, frame counters, energy, and rejection reason into one renderer handoff snapshot for engine-side QoS dashboards, while `getTelemetryVector` exports the same state as 20 named normalized scalar channels for ML/QoS controllers. `evaluateQoSDecision` condenses that telemetry into a stability score plus stable/throttle/fallback recommendations for render-loop policy. `XRRenderer` publishes named composite-channel layout constants for the 11 sensory, 16 view, and 16 visual-field segments so render integrations do not rely on magic indices. `buildAdaptivePresentationFrame` applies the active profile to the composite packet before presentation by reducing render-distance/particle pressure and raising foveation, peripheral blur, motion comfort, and retinal stability during recovery.

## C API Surface
- `dnxr_run_inference`
- `dnxr_run_action`
- `dnxr_run_full_body_action`
- `dnxr_run_adaptation`

These APIs expose neural-driven state, actions, full-body commands, and visual adaptation parameters for external engines.
