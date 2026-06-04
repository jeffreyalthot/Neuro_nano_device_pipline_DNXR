#!/usr/bin/env python3
import pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "threat_model_review_0.4.4.md"

def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text("""# Threat Model Review 0.4.4

## Scope
Review record for replay, tamper, secure update, key lifecycle, audit trail, and runtime safety interactions.

## Disposition
The software threat model is ready for independent cybersecurity review. Named sign-off is required before cybersecurity evidence can unlock clinical readiness.

## Open external evidence
- Target-hardware penetration assessment.
- Secure update chain verification on deployment media.
- Independent vulnerability disposition review.
""", encoding="utf-8")
    print(OUT)
if __name__ == "__main__": main()
