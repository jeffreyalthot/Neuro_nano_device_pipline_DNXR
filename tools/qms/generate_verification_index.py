#!/usr/bin/env python3
from qms_lib import GENERATED, split_paths, traceability_rows

GENERATED.mkdir(parents=True, exist_ok=True)
rows = traceability_rows()
tests = sorted({test for row in rows for test in split_paths(row.get("TestFiles", ""))})
out = GENERATED / "VerificationIndex.md"
out.write_text("# VerificationIndex\n\n" + "\n".join(f"- {t}" for t in tests) + "\n")
print(out)
