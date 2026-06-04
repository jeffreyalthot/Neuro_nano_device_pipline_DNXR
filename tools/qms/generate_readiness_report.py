#!/usr/bin/env python3
from qms_lib import GENERATED, disabled_test_impact, evidence_artifact_gaps, high_severity_open_hazards, missing_required_docs, placeholder_traceability_rows, test_file_gaps, traceability_field_gaps, traceability_rows

GENERATED.mkdir(parents=True, exist_ok=True)
rows = traceability_rows()
missing = missing_required_docs()
field_gaps = traceability_field_gaps(rows)
placeholder_gaps = placeholder_traceability_rows(rows)
evidence_gaps = evidence_artifact_gaps(rows)
test_gaps = test_file_gaps(rows)
open_hazards = high_severity_open_hazards()
long_impacts = [item for item in disabled_test_impact() if item["disabled_by_default"]]
blocking = []
if missing: blocking.append("Required documents missing: " + ", ".join(missing))
if field_gaps: blocking.append(f"Traceability rows with missing required fields: {len(field_gaps)}")
if placeholder_gaps: blocking.append(f"Placeholder traceability requirements: {len(placeholder_gaps)}")
if evidence_gaps: blocking.append(f"Missing linked evidence artifacts: {len(evidence_gaps)}")
if test_gaps: blocking.append(f"Missing linked test/tool paths: {len(test_gaps)}")
if open_hazards: blocking.append(f"High-severity hazards require independent review: {len(open_hazards)}")
if long_impacts: blocking.append(f"Critical long/readiness tests disabled in normal lane: {len(long_impacts)}")
status = "Draft structure complete; ClinicalTestReady: false" if blocking else "Evidence structure complete; ClinicalTestReady still requires signed expert review"
out = GENERATED / "ClinicalTestReadinessReport.md"
out.write_text("# ClinicalTestReadinessReport\n\n" + f"Status: {status}\n\n" + "## Major missing evidence / blockers\n" + ("\n".join(f"- {b}" for b in blocking) if blocking else "- No structural blockers detected by automated checks; signed clinical/regulatory review still required.") + "\n\n## Disabled-test impact\n" + "\n".join(f"- {i['test']}: {i['readiness_impact']}" for i in disabled_test_impact()) + "\n")
print(out)
