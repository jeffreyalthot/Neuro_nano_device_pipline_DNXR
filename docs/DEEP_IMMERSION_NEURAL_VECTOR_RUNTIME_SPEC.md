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

When quality/variance are too low, `stability_guard` is activated to reduce aggressiveness and preserve stable immersion.

## Visual Perception Quality Model
`XRAdaptationFrame` now includes:
- `visualClarity`
- `depthPrecision`
- `motionStability`
- `contrastEnhancement`

A derived harmony score controls shader profile transition for deeper and more coherent visual immersion.

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

## C API Surface
- `dnxr_run_inference`
- `dnxr_run_action`
- `dnxr_run_full_body_action`
- `dnxr_run_adaptation`

These APIs expose neural-driven state, actions, full-body commands, and visual adaptation parameters for external engines.
