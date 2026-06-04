#!/usr/bin/env python3
import pathlib, re
root=pathlib.Path(__file__).resolve().parents[2]
text=(root/'docs/FULL_DIVE_HAZARD_ANALYSIS.md').read_text()
hazards=sorted(set(re.findall(r'H-\d{3}', text)))
out=root/'qms/generated/RiskReport.md'
out.write_text('# RiskReport\n\nHazards indexed: %d\n\n' % len(hazards) + '\n'.join(hazards) + '\n')
print(out)
