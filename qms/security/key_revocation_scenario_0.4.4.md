# Key Revocation Scenario Evidence 0.4.4

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
