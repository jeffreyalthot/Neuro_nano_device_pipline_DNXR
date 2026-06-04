#!/usr/bin/env python3
import csv, pathlib
root=pathlib.Path(__file__).resolve().parents[2]
rows=list(csv.DictReader(open(root/'docs/TRACEABILITY_MATRIX.csv', newline='')))
tests=sorted({r['TestFiles'] for r in rows if r.get('TestFiles')})
out=root/'qms/generated/VerificationIndex.md'
out.write_text('# VerificationIndex\n\n' + '\n'.join(f'- {t}' for t in tests) + '\n')
print(out)
