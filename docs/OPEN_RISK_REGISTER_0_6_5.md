# Open Risk Register 0.6.5

Not authorized for human use. For medical/regulatory review only. Requires external IDE/ITA/IRB/REB approval before any human-subject activity.

| Risk ID | Hazard | Severity | Probability | Control | Evidence | Residual risk | Owner | Status | Release blocker yes/no |
|---|---|---:|---:|---|---|---|---|---|---|
| ORR-065-001 | Absence of external hardware bench validation | High | Medium | Bench/HIL gates require p95/p99 reports and signoff artifacts | qms/generated/HILBenchMetrics_0_6_5.json | Open until external bench review | QMS lead | Open | yes |
| ORR-065-002 | Absence of independent clinical validation | High | Medium | HumanSubjectUseGate blocks human-use decisions | HumanSubjectUseGate | Open | Medical reviewer | Open | yes |
| ORR-065-003 | Absence of IDE/ITA authorization | High | Medium | RegulatoryAuthorizationEvidenceGate checks external artifacts only | qms/signoff placeholders | Open | Regulatory reviewer | Open | yes |
| ORR-065-004 | Absence of medical investigator | High | Medium | Physician investigator signoff required before human-subject activity | qms/signoff/PHYSICIAN_INVESTIGATOR_SIGNOFF.md | Open | Sponsor | Open | yes |
| ORR-065-005 | Absence of material biocompatibility evidence | High | Medium | Hardware scope remains phantom/loopback bench-only | Bench profiles | Open | Hardware reviewer | Open | yes |
| ORR-065-006 | Absence of external ISO 13485 QMS signoff | Medium | Medium | Independent review checklist requires QMS reviewer | qms/design_reviews/DR-0.6.5-independent-review.md | Open | QMS lead | Open | yes |
