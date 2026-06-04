# Change Impact Analysis 0.4.4

## Added or promoted capabilities
- EvidenceRepository promotion path for advanced readiness decisions.
- QMS ReviewGate and SignOffRecord model for unsigned/high-risk blockers.
- Runtime10kFullEvidence artifact for explicit full controller execution evidence.
- ClinicalSoftwareFreezeReport for expert preclinical review preparation.
- Cybersecurity evidence package 0.4.4.

## Impacted readiness areas
- Readiness decisions now depend on signed evidence artifacts rather than scorecard booleans alone.
- Clinical claims remain blocked by physical HIL, phantom qualification, named sign-off, and regulatory/ethics authorization.
- Runtime bounded evidence and full runtime evidence must be reported separately.

## Remaining blockers
- Evidence manifest consistency.
- Full-lane transcript consistency.
- Named sign-off.
- Physical HIL.
- Regulatory/clinical protocol acceptance.
