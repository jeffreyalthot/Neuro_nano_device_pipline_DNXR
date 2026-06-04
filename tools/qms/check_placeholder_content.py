#!/usr/bin/env python3
import sys
from qms_lib import DOCS, PLACEHOLDER_PATTERNS, placeholder_traceability_rows, traceability_rows

bad_docs = []
from qms_lib import REQUIRED_DOCS
for doc in REQUIRED_DOCS:
    path = DOCS / doc
    if path.suffix != ".md" or not path.exists():
        continue
    text = path.read_text(encoding="utf-8")
    hits = [pattern.pattern for pattern in PLACEHOLDER_PATTERNS if pattern.search(text)]
    if hits:
        bad_docs.append((path.name, hits))
trace_gaps = placeholder_traceability_rows(traceability_rows())
if bad_docs or trace_gaps:
    for doc, hits in bad_docs[:20]:
        print(f"Placeholder content in {doc}: {hits}")
    if trace_gaps:
        print("Placeholder traceability requirements:", ", ".join(trace_gaps[:20]))
    sys.exit(1)
print("No placeholder readiness content detected in markdown docs or traceability requirements")
