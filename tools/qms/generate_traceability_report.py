#!/usr/bin/env python3
from collections import Counter
from qms_lib import GENERATED, placeholder_traceability_rows, traceability_field_gaps, traceability_rows

GENERATED.mkdir(parents=True, exist_ok=True)
rows = traceability_rows()
by_subsystem = Counter(row.get("Subsystem", "<missing>") for row in rows)
field_gaps = traceability_field_gaps(rows)
placeholder_gaps = placeholder_traceability_rows(rows)
out = GENERATED / "TraceabilityReport.md"
out.write_text("# TraceabilityReport\n\n" + f"Total requirements: {len(rows)}\n\n" + f"Rows with missing required fields: {len(field_gaps)}\n\n" + f"Placeholder requirements detected: {len(placeholder_gaps)}\n\n" + "## Requirements by subsystem\n" + "\n".join(f"- {k}: {v}" for k, v in sorted(by_subsystem.items())) + "\n")
print(out)
