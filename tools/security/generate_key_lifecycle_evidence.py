#!/usr/bin/env python3
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "key_revocation_scenario_0.4.1.md"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(
        "# Key Revocation Scenario 0.4.1\n\n"
        "## Scope\nGenerated evidence index for key revocation, rotation, and secure-update authorization behavior.\n\n"
        "## Linked tests\n"
        "- tests/dnxr_secret_provider_test.cpp\n"
        "- tests/dnxr_secure_update_policy_test.cpp\n"
        "- tests/dnxr_security_audit_trail_test.cpp\n\n"
        "## Decision\nBlocked pending execution transcript and named cybersecurity reviewer acceptance.\n\n"
        "## Reviewer / status\nReviewer role: cybersecurity reviewer. Status: named sign-off pending.\n",
        encoding="utf-8",
    )
    print(OUT)


if __name__ == "__main__":
    main()
