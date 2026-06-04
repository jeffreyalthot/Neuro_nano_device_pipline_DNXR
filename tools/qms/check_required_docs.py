#!/usr/bin/env python3
import pathlib, sys
root=pathlib.Path(__file__).resolve().parents[2]
required=['FULL_DIVE_DEFINITION.md','HIL_TEST_PLAN.md','FULL_DIVE_HAZARD_ANALYSIS.md','TRACEABILITY_MATRIX.csv','SOFTWARE_DEVELOPMENT_PLAN.md','CYBERSECURITY_THREAT_MODEL.md','CLINICAL_TEST_READINESS_PACKAGE.md']
missing=[d for d in required if not (root/'docs'/d).exists()]
if missing:
    print('Missing required docs:', ', '.join(missing))
    sys.exit(1)
print('All required readiness docs present')
