# Regulatory Claim Guard

DNXR remains a simulation-only software/HIL readiness platform until physical HIL, preclinical, clinical, independent review, ethics, and applicable regulatory authorization evidence are present and signed.

## Allowed wording at current evidence levels

Use these claims when evidence is limited to simulation or software/HIL readiness:

- `simulation-only`
- `preclinical software readiness package`
- `not for human use`
- `not clinically validated`
- `requires authorized investigational testing`

## Blocked claims without higher evidence

The `RegulatoryClaimGuard` blocks release/report claims that exceed the evidence context:

| Claim | Minimum evidence enforced |
| --- | --- |
| `HumanFullDiveImmersion` | Qualified physical HIL, complete preclinical evidence, human clinical evidence, and capability matrix evidence at `ClinicalEvidenceAvailable`. |
| `ClinicalTestReady` | `ClinicalTestReady` readiness plus signed investigational authorization evidence. |
| `SafeForHumanUse` | Human clinical evidence, applicable human-use authorization, and recorded medical approval/clearance. |
| `TherapeuticUse` | Therapeutic clinical evidence and medical approval/clearance for the intended use. |
| `ImplantReady` | Complete preclinical evidence, clinical evidence, and implant approval/clearance. |
| `MedicalApproved` | Recorded approval/clearance for the stated intended use. |

## Evidence repository enrichment

When an `EvidenceRepository` is provided, the guard only promotes the evidence context from signed artifacts. Physical HIL/preclinical status requires signed `physical_hil_session`, `hil_latency_report`, `risk_control_verification`, and `runtime_10k_full` evidence. Investigational authorization requires signed `clinical_protocol_package`, `canada_regulatory_package`, `us_regulatory_package`, and `independent_signoff` evidence. Human-use and therapeutic claims remain blocked until signed `clinical_evidence` and `medical_approval` artifacts are present.

## Traceability

This control is traced as `REQ-REG-CLAIM-001` in the traceability matrix and verified by `dnxr_regulatory_claim_guard_test`.
