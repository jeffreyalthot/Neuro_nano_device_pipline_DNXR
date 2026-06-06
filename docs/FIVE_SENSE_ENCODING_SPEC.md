# Five Sense Encoding Specification

Each modality is encoded as typed neural channel commands containing modality, chip target region, encoding model id, intensity, timing, ramp, safety limit, calibration profile, confidence, and latency budget.

| Modality | Input | Encoding | Limit | Fallback |
| --- | --- | --- | --- | --- |
| Visual | brightness, depth, motion | visual model | flash blocker, motion cap | reality anchor visual |
| Auditory | spatial, tonal, distance | auditory model | skew and volume cap | reduce intensity |
| Touch | pressure, texture, contact | pain-blocked touch model | no sharp spikes | pressure-only |
| Thermal | warmth, cold, gradient | thermal model | burn-risk lockout | neutral |
| Proprioception | joint/body pose | limb position model | body schema guard | anchor/zero drift |
| Vestibular | rotation/acceleration | hard-limited vestibular model | jerk/fall cap | zero vestibular |
| Smell | contextual scent | persistent olfactory model | exposure/adaptation cap | neutral air |
| Taste | discrete taste | gustatory model | no continuous stimulation | forced neutralization |

## 0.5.1 Clinical submodality decomposition

The frame schema separates and bounds the following clinical submodalities:

- Visual: brightness, color, depth, motion flow, peripheral field, focus, contrast, flicker-risk.
- Audio: volume, pitch, spatial position, distance, reverb, occlusion, startle-risk.
- Haptic/thermal: pressure, vibration, texture, contact area, slip, impact, warmth, cold, gradient.
- Vestibular: angular velocity, linear acceleration, tilt, gravity vector.
- Proprioception: joint position, balance, limb ownership, contact prediction.
- Olfactory: onset, persistence, decay, adaptation with a dedicated persistence budget.
- Gustatory: discrete stimulation, persistence, neutralization with a separate dose budget.
- Interoceptive safety: respiration, heart rate, stress, nausea, vertigo, panic, disorientation, stop request, communication loss.

Clinical encoding forbids pain, burn, electric-shock, suffocation, crushing, forced immobilization, fall panic, drowning, claustrophobic lock, fear induction, and unanchored body distortion. Unsafe requests escalate to reality-anchor, decompression, or emergency stop according to severity.
