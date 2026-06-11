#!/usr/bin/env python3
from pathlib import Path
import re, sys
ROOT = Path(__file__).resolve().parents[1]
PHRASES = ["human ready", "clinical ready", "safe for human", "approved for human", "real neural stimulation", "patient treatment"]
NEGATIONS = ["not human ready", "not approved", "simulation-only", "not clinical ready", "not safe for human"]
violations = []
for path in list((ROOT/"src").rglob("*")) + list((ROOT/"docs").rglob("*") if (ROOT/"docs").exists() else []):
    if not path.is_file() or path.suffix.lower() not in {".h", ".hpp", ".cpp", ".cc", ".cxx", ".md", ".txt", ".py", ".json"}:
        continue
    rel = path.relative_to(ROOT)
    for i, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
        low = line.lower()
        if any(n in low for n in NEGATIONS) or "forbidden claim" in low or "forbidden claims" in low or "without signed external authorization" in low:
            continue
        for phrase in PHRASES:
            if phrase in low:
                violations.append(f"{rel}:{i}: forbidden human-readiness claim '{phrase}'")
if violations:
    print("\n".join(violations))
    sys.exit(1)
print("No forbidden human-readiness claims found.")
