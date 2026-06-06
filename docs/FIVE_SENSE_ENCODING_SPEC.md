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
