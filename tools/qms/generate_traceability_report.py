#!/usr/bin/env python3
import csv, pathlib
root=pathlib.Path(__file__).resolve().parents[2]
rows=list(csv.DictReader(open(root/'docs/TRACEABILITY_MATRIX.csv', newline='')))
out=root/'qms/generated/TraceabilityReport.md'
out.write_text('# TraceabilityReport\n\nTotal requirements: %d\n\nCritical gaps are rows missing HazardID, SourceFiles, or TestFiles.\n' % len(rows))
print(out)
