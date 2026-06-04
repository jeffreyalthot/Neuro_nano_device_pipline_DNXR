# Clinical Test Acceptance Dossier

Version target: 0.4.6 acceptance-review candidate
Generated purpose: distinguish expert clinical-test acceptance review readiness from authorization to begin human clinical testing.

## Automated verdict

| Question | Verdict |
| --- | --- |
| Ready for expert clinical-test acceptance review? | YES, when the software/QMS scorecard is complete and reviewed evidence exists for software HIL, full runtime evidence, cybersecurity package, sign-off packets, and explicit external blockers. |
| Ready to begin human clinical testing? | NO for the current package posture. |

## Required acceptance-review evidence

The `ClinicalTestAcceptanceReviewGate` expects reviewed or signed evidence artifacts for:

- `software_hil_session`
- `runtime_10k_full`
- `cybersecurity_package`
- `qms_signoff_packets`
- `clinical_external_blockers`

These artifacts support expert review preparation only. They do not authorize or imply human clinical testing.

## Required clinical-test evidence

The same gate keeps clinical testing blocked unless signed evidence exists for:

- `physical_hil_session`
- `physical_hil_raw_logs`
- `phantom_qualification_report`
- `target_hardware_latency_report`
- `risk_control_verification`
- `runtime_10k_full`
- `cybersecurity_executed`
- `clinical_protocol_package`
- `reb_irb_approval`
- `physician_signoff`
- `canada_ita_submission_package`
- `us_ide_submission_package`
- `informed_consent_package`
- `monitoring_plan`
- `adverse_event_reporting_plan`
- `investigator_package`
- `investigational_labeling`
- `residual_risk_acceptance`

## Current blockers to human clinical testing

- Physical HIL session evidence and raw logs are not present as signed artifacts.
- Phantom qualification and target-hardware latency evidence are not present as signed artifacts.
- Named physician, regulatory, and residual-risk sign-offs remain pending.
- Clinical protocol, investigator package, informed consent, monitoring plan, adverse-event reporting plan, and investigational labeling are not signed.
- REB/IRB approval and Canada ITA / US IDE submission-package evidence are not signed.

## Intended use of this dossier

This dossier is a QMS control document for 0.4.6 planning. It should be regenerated or updated after physical HIL execution, named reviews, ethics approvals, and regulatory submission records are available.
