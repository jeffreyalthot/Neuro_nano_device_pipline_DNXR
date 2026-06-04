#!/usr/bin/env python3
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "threat_model_review_0.4.1.md"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(
        "# Threat Model Review 0.4.1\n\n"
        "## Scope\nGenerated review index for cybersecurity threat-model evidence covering secure update, key lifecycle, replay protection, packet tamper detection, audit trail, and runtime safety interlocks.\n\n"
        "## Linked artifacts\n"
        "- docs/CYBERSECURITY_THREAT_MODEL.md\n"
        "- docs/SECURITY_ARCHITECTURE.md\n"
        "- docs/SECURE_UPDATE_PLAN.md\n"
        "- docs/KEY_MANAGEMENT_PLAN.md\n\n"
        "## Decision\nConditional software-QMS evidence only; named cybersecurity review and vulnerability disposition remain required.\n\n"
        "## Reviewer / status\nReviewer role: independent cybersecurity reviewer. Status: named sign-off pending.\n",
        encoding="utf-8",
    )
    print(OUT)


if __name__ == "__main__":
    main()
