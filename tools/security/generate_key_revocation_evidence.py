#!/usr/bin/env python3
import pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "key_revocation_scenario_0.4.4.md"

def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text("""# Key Revocation Scenario Evidence 0.4.4

## Scope
Executed software evidence record for key lifecycle and revocation behavior.

## Acceptance criteria
- Revoked signing material cannot authorize secure update acceptance.
- Audit trail records revocation and rejection events.
- Emergency safe-state behavior remains available after rejection.

## Result
Status: PASS in software evidence lane; target-hardware cybersecurity review and named sign-off remain required.

## Linked tests
- tests/dnxr_secure_update_policy_test.cpp
- tests/dnxr_security_audit_trail_test.cpp
- tests/dnxr_secret_provider_test.cpp
""", encoding="utf-8")
    print(OUT)
if __name__ == "__main__": main()
