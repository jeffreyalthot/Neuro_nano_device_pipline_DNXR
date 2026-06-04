#!/usr/bin/env python3
import pathlib
root=pathlib.Path(__file__).resolve().parents[2]
required=['CLINICAL_TEST_READINESS_PACKAGE.md','CANADA_ITA_READINESS_CHECKLIST.md','US_IDE_READINESS_CHECKLIST.md','FULL_DIVE_HAZARD_ANALYSIS.md','TRACEABILITY_MATRIX.csv','CYBERSECURITY_THREAT_MODEL.md','SOFTWARE_DEVELOPMENT_PLAN.md']
missing=[d for d in required if not (root/'docs'/d).exists()]
out=root/'qms/generated/ClinicalTestReadinessReport.md'
out.write_text('# ClinicalTestReadinessReport\n\nStatus: %s\n\nMissing: %s\n' % ('Draft-ready for expert review' if not missing else 'Gaps remain', ', '.join(missing) or 'none'))
print(out)
