# Cryptographic Implementation Review

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence, simulation/HIL readiness, and controlled hardware-bench preparation. Not authorized for human use, not clinically validated, and not a certified medical device.


## Review scope
- SHA-256 manifests for source/archive integrity.
- Signed QMS evidence gates for clinical/human-use build paths.
- Revocation/trust-store inputs referenced by session and hardware-bench workflows.
- Release/archive checksums shared through the data room.

## Required reviewer questions
1. Which artifacts are signed, by whom, and under which role authority?
2. Are signatures bound to commit, version, CMake flags, target mode, and evidence hash?
3. Is stale or revoked evidence rejected?
4. Are example signatures clearly non-authorizing?
5. Can a reviewer reproduce SHA-256 values from a clean clone/archive?

## Disposition
Current files provide due-diligence scaffolding and integrity manifests. Independent cryptographic review remains required before relying on any evidence gate for clinical investigation.
