# Risk Management File Template (ISO 14971-aligned)

## Risk scoring rubric
- Severity: S1 (minor) .. S5 (catastrophic)
- Probability: P1 (remote) .. P5 (frequent)
- Detectability: D1 (high) .. D5 (low)
- Priority index: S x P x D

## Hazard register
| Hazard ID | Hazard description | Sequence of events | Harm | S | P | D | Initial Risk | Risk Control(s) | Verification | Residual Risk | Accept? |
|---|---|---|---|---|---|---|---|---|---|---|---|
| H-001 | Telemetry invalid accepted | sensor drift -> parser accepts -> unsafe stimulus | physiological overload | 5 | 3 | 3 | 45 | strict validation + emergency latch + kill switch | unit+safety integration tests | TBD | TBD |
| H-002 | Timing overrun > target | CPU spike -> missed loop budget | perceptual instability | 3 | 3 | 2 | 18 | runtime metrics + budget checks + degraded mode | latency tests | TBD | TBD |
| H-003 | Protocol mismatch | version mismatch -> decode ambiguity | wrong command dispatch | 4 | 2 | 3 | 24 | packet version + strict validator | protocol tests | TBD | TBD |

## Residual risk acceptance board
- Members:
- Date:
- Decision:
- Rationale:
