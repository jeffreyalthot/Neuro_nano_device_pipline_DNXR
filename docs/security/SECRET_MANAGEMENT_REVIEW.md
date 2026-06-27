# Secret Management Review

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence, simulation/HIL readiness, and controlled hardware-bench preparation. Not authorized for human use, not clinically validated, and not a certified medical device.


## Objective
Demonstrate that due-diligence disclosures do not expose production secrets and that sample credentials cannot be confused with deployable keys.

## Reviewed surfaces
| Surface | Expected state | Review action | Disposition |
| --- | --- | --- | --- |
| `.env` files | No real secrets committed | Search for env-style assignments before release | Open until final scan attached |
| `*.json.example` clinical artifacts | Examples only, non-production | Verify labels and placeholder signatures | Must not authorize runtime human use |
| QMS signature paths | Signed evidence required for gated modes | Confirm existence and verification path | Gate remains mandatory |
| Data-room exports | NDA controlled | Generate package checksums and access logs | Required before disclosure |
| Build logs | No tokens in compiler output | Redact paths/secrets if needed | Required before external sharing |

## Required release checks
```bash
rg -n --hidden --glob '!/.git/**' '(api[_-]?key|secret|token|password|private[_-]?key|BEGIN (RSA|EC|OPENSSH) PRIVATE KEY)' .
python3 tools/qms/check_evidence_artifacts.py
```

## Human-use lock note
Secret management must not introduce a shortcut around the QMS signature verifier or external clinical authorization requirements. A possession of an example key, placeholder file, or data-room archive is not authorization to run human-use mode.
