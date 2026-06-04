#!/usr/bin/env python3
import sys
from qms_lib import evidence_artifact_gaps, test_file_gaps, traceability_rows

rows = traceability_rows()
evidence_gaps = evidence_artifact_gaps(rows)
test_gaps = test_file_gaps(rows)
if evidence_gaps or test_gaps:
    for req, rel in evidence_gaps[:20]:
        print(f"Missing evidence artifact for {req}: {rel}")
    for req, rel in test_gaps[:20]:
        print(f"Missing linked test/tool path for {req}: {rel}")
    sys.exit(1)
print(f"All linked evidence artifacts and test/tool paths exist for {len(rows)} traceability rows")
