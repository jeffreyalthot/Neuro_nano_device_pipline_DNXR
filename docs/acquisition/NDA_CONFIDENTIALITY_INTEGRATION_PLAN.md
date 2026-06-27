# NDA Confidentiality Integration Plan

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Boundary: preclinical R&D due-diligence materials; not clinical validation or human-use authorization.


## Disclosure workflow
1. Share Level 0/1 summaries before NDA only.
2. Execute NDA before any Level 2 technical dossier disclosure.
3. Approve named reviewers before Level 3 source/QMS/test evidence access.
4. Require separate owner/counsel authorization for Level 4 IP, valuation, commercial, or chain-of-title files.
5. Record every artifact, checksum, recipient, access date, and destruction/return outcome.

## Package controls
| Control | Requirement | Record |
| --- | --- | --- |
| Access authorization | Named reviewer, organization, purpose, NDA reference | `DATA_ROOM_ACCESS_LOG.csv` |
| Integrity | SHA-256 hash for archive and files | `SHA256_MASTER_MANIFEST.txt` |
| Scope | Data-room folder, confidentiality level, disclosure gate | `DATA_ROOM_STRUCTURE.md` |
| Closure | Return/destruction certification | `POST_REVIEW_DESTRUCTION_CERTIFICATE.md` |
| Follow-up | Written questions and supplemental artifacts | `REVIEW_RESPONSE_PACKAGE.md` |

## Human-use caveat
NDA disclosure may include documentation of unlock workflows for review, but it is not authorization to enable human-use, clinical trial, or patient-connected modes.
