# Penetration Test Plan

Cybersecurity readiness objective: provide defensible evidence for threat modeling, audit trails, replay/tamper detection, secure updates, key lifecycle controls, vulnerability management, penetration testing, and privacy/data protection.

## Required evidence
- Threat events are recorded in `src/security/SecurityAuditTrail`.
- Replay and packet tamper scenarios have tests.
- Secure updates require signatures and monotonic versions.
- Key lifecycle policy defines rotation and revoked-key rejection.

## Regulatory references
- Health Canada Investigational Testing Authorization: https://www.canada.ca/en/health-canada/services/drugs-health-products/medical-devices/application-information/guidance-documents/investigational-testing-authorizations-guidance.html
- FDA Investigational Device Exemption: https://www.fda.gov/medical-devices/premarket-submissions-selecting-and-preparing-correct-submission/investigational-device-exemption-ide
- 21 CFR Part 812: https://www.ecfr.gov/current/title-21/chapter-I/subchapter-H/part-812
- FDA QMSR / 21 CFR Part 820: https://www.fda.gov/medical-devices/postmarket-requirements-devices/quality-management-system-regulation-qmsr
- FDA Cybersecurity in Medical Devices: https://www.fda.gov/medical-devices/digital-health-center-excellence/cybersecurity
- ISO 13485 overview: https://www.iso.org/standard/59752.html
- IEC 62304 overview: https://www.iso.org/standard/38421.html
- ISO 14971 overview: https://www.iso.org/standard/72704.html
- TCPS 2 Canada: https://ethics.gc.ca/eng/policy-politique_tcps2-eptc2_2022.html
