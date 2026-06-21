# Clinical unlock workflow

DistributedNeuralXR remains locked to simulation by default. `DNXR_CLINICAL_TEST_BUILD=ON` only compiles the controlled investigation path; it never unlocks human use by itself.

## Levels

1. **Clinical review unlocked**: signed QMS release, matching build hash, complete QMS documents, verification/validation evidence, and risk file.
2. **Clinical human session unlocked**: review package plus REB/IRB, Health Canada ITA or FDA IDE, signed protocol, physician investigator, clinical site, and operator authorization.
3. **Clinical stimulus dispatch unlocked**: human-session package plus one signed patient session, consent, calibration, prescription, active monitoring, physical and software emergency stops, clinician presence, and unexpired protocol/session.

## Required examples

Copy `clinical_evidence/CLINIC_TEST_UNLOCK.signed.json.example` to a secure deployment location and replace all placeholder values with signed production values. Copy `clinical_sessions/SESSION_UNLOCK.signed.json.example` for each patient session; one file is valid for one subject, protocol, site, build, and expiration window only.

## Status panel

Run:

```bash
python tools/clinical/dnxr_clinic_unlock_check.py --clinic-unlock clinical_evidence/CLINIC_TEST_UNLOCK.signed.json --session-unlock clinical_sessions/SESSION_UNLOCK.signed.json --trust-store qms/trust/qms_trust_store.json --revocations qms/trust/revoked_keys.json --source-root .
```

The command prints current mode, capabilities, blockers, expiration, protocol, site, and session details.
