#!/usr/bin/env python3
import csv, pathlib, sys
root=pathlib.Path(__file__).resolve().parents[2]
rows=list(csv.DictReader(open(root/'docs/TRACEABILITY_MATRIX.csv', newline='')))
missing=[r['RequirementID'] for r in rows if not r.get('HazardID') or not r.get('RiskControlID') or not r.get('SourceFiles') or not r.get('TestFiles')]
if len(rows)<150 or missing:
    print(f'Traceability incomplete: rows={len(rows)} missing={missing[:10]}')
    sys.exit(1)
print(f'Traceability complete for {len(rows)} requirements')
