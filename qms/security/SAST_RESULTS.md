# SAST and Secret Scan Results

> Confidentiality: Level 3 — NDA controlled.
> Boundary: automated text scan evidence for preclinical due diligence; not a substitute for independent secure-code review.

## Secret scan command
```bash
rg -n --hidden --glob '!/.git/**' --glob '!docs/acquisition/SHA256_MASTER_MANIFEST.txt' --glob '!docs/acquisition/SOURCE_CODE_DISCLOSURE_MANIFEST.csv' --glob '!qms/security/SBOM.*.json' '(api[_-]?key|secret|token|password|private[_-]?key|BEGIN (RSA|EC|OPENSSH) PRIVATE KEY)' .
```

## Current disposition
The repository text scan produced review hits for code identifiers, documentation references, test names, and environment-variable names such as `DNXR_XR_AUTH_TOKEN`. No private-key block or literal production credential was identified in the reviewed hit sample. Final external disclosure still requires a named security reviewer to attach the full scan transcript and sign off any remaining false positives.

## Required follow-up
| ID | Item | Status | Disposition |
| --- | --- | --- | --- |
| SAST-001 | Run dependency-aware SAST toolchain | Open | Tool selection and version to be recorded |
| SAST-002 | Complete full secret-scan triage | Open | Attach full transcript and reviewer signoff |
| SAST-003 | Verify sample `.json.example` signatures cannot authorize runtime | Open | Cross-reference human-use lock tests |
| SAST-004 | Review QMS signature parser and trust-store behavior | Open | Cross-reference cryptographic implementation review |
