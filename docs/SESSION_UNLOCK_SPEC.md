# Session unlock specification

`clinical_sessions/SESSION_UNLOCK.signed.json` is a temporary, session-scoped artifact. It must include a pseudonymized subject id, protocol id, site id, operator id, patient consent, calibration, prescription, monitoring state, physical emergency stop verification, software emergency stop verification, clinician presence, session limits, and an Ed25519 signature.

A session unlock cannot be reused after expiration, with another patient, with another protocol, with another site, or with another build. Revoked signing keys immediately block dispatch.
