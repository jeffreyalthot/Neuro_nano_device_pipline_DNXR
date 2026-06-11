#!/usr/bin/env python3
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
violations = []
for path in ROOT.rglob('*.cpp'):
    if '.git' in path.parts or 'build' in path.parts:
        continue
    lines = path.read_text(errors='ignore').splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if 'rejectWithHold' not in line or '::rejectWithHold' in line:
            i += 1
            continue
        call = line
        start = i + 1
        while ';' not in call and i + 1 < len(lines):
            i += 1
            call += '\n' + lines[i]
        if 'PipelinePathKind::' not in call and 'pathClassifier_' not in call and ', path' not in call:
            violations.append(f'{path.relative_to(ROOT)}:{start}: rejectWithHold without PipelinePathKind')
        i += 1

if violations:
    print('\n'.join(violations))
    sys.exit(1)
print('rejectWithHold PipelinePathKind invariant passed')
