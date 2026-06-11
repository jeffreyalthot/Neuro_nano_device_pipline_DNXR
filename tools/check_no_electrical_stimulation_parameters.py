#!/usr/bin/env python3
from pathlib import Path
import re, sys
ROOT = Path(__file__).resolve().parents[1]
TERMS = ["microAmp", "uA", "mA", "voltage", "pulseWidth", "electrodeCurrent", "stimulationAmplitude", "chargeDensity", "implantCurrent"]
TARGETS = [ROOT/"src/neural_chip", ROOT/"src/full_dive"]
pattern = re.compile(r"\b(" + "|".join(map(re.escape, TERMS)) + r")\b", re.IGNORECASE)
violations = []
for base in TARGETS:
    if not base.exists():
        continue
    for path in base.rglob("*"):
        if not path.is_file() or path.suffix not in {".h", ".hpp", ".cpp", ".cc", ".cxx"}:
            continue
        rel = path.relative_to(ROOT)
        rels = rel.as_posix()
        if rels.startswith("docs/") or rels.startswith("tests/negative/"):
            continue
        if not (rels.startswith("src/neural_chip/") or re.match(r"src/full_dive/[^/]*_chip/", rels) or rels.startswith("src/full_dive/chip_pipeline/")):
            continue
        for i, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            m = pattern.search(line)
            if m:
                violations.append(f"{rel}:{i}: forbidden electrical stimulation parameter '{m.group(1)}'")
if violations:
    print("\n".join(violations))
    sys.exit(1)
print("No electrical stimulation parameters found in neural-chip-only sources.")
