#!/usr/bin/env python3
import sys
from qms_lib import placeholder_traceability_rows, traceability_field_gaps, traceability_rows

rows = traceability_rows()
field_gaps = traceability_field_gaps(rows)
placeholder_gaps = placeholder_traceability_rows(rows)
if len(rows) < 150 or field_gaps or placeholder_gaps:
    print(f"Traceability incomplete: rows={len(rows)} field_gaps={field_gaps[:10]} placeholder_gaps={placeholder_gaps[:10]}")
    sys.exit(1)
print(f"Traceability complete for {len(rows)} concrete requirements")
