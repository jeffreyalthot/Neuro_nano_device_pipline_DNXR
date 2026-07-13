
## Prompt 04 external dependency posture — 2026-07-13

No new external runtime dependency was added. The real chip transport remains intentionally unavailable in the canonical graph until a certified external driver/ACK/feedback binding is provided; the canonical transaction holds instead of fabricating submit/ACK/feedback.

## Prompt 05 dependency update — 2026-07-13

No new external dependency was added. The runner facade and render-stage execution source are no longer production runtime-core dependencies. The real backend remains closed without certified hardware transport/ACK/feedback.

## Prompt 06 acquisition dependencies — 2026-07-13

No headset, HMD, gaze, generic signal provider, or simulation signal source is allowed in the canonical acquisition boundary. Real mode requires `ExternalHalBciAcquisitionSource` backed by an external HAL/driver with attested identity and authenticated packets; absent evidence fails closed.

## Prompt 07 external world ingress dependency
- Real runtime requires a certified IPC/network world-engine driver implementing `IWorldIngress` with mutual authentication and session binding. Until supplied, the production composition uses a fail-closed unavailable source.

## Prompt 08 model dependency
- Real mode still requires an immutable signed motor-intent model artifact loader. The current canonical identity is deterministic/static and suitable for architecture/testing only; real activation remains blocked by hardware/readiness gates.

## Prompt 10 visual external dependencies

The visual BCI service requires only authenticated structured world-frame metadata and signed patient/topology/capability artifacts. It does not require OpenXR, SteamVR, HMD telemetry, screen capture, stereoscopic display, or `XRRenderer` output.

## Prompt 11 auditory external dependencies

The auditory BCI service requires authenticated structured world audio metadata and signed patient/topology/capability evidence. It does not require an audio device, DAC, ALSA/PulseAudio/CoreAudio/WASAPI, HMD audio endpoint, `SensoryRenderer`, or physical speaker output.
