#!/usr/bin/env python3
from __future__ import annotations
import pathlib
import re
import sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
TERMS = [r"first\s+in\s+human", r"clinical\s+test\s+ready", r"human\s+subject\s+ready", r"ready\s+for\s+patient", r"ready\s+for\s+human"]
GUARD = "Not authorized for human use. For medical/regulatory review only. Requires external IDE/ITA/IRB/REB approval before any human-subject activity."
SKIP = {"tools/qms/check_no_overstated_clinical_readiness.py"}

def main() -> int:
    failures = []
    for path in list((ROOT / "docs").glob("*.md")) + list((ROOT / "qms").rglob("*.md")):
        rel = path.relative_to(ROOT).as_posix()
        if rel in SKIP:
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        if any(re.search(term, text, flags=re.IGNORECASE) for term in TERMS) and GUARD not in text:
            failures.append(rel)
    if failures:
        for rel in failures:
            print(f"Missing guard phrase for overstated clinical-readiness language: {rel}", file=sys.stderr)
        return 1
    return 0
if __name__ == "__main__":
    raise SystemExit(main())
