#!/usr/bin/env python3
import re
from qms_lib import DOCS, GENERATED, high_severity_open_hazards

GENERATED.mkdir(parents=True, exist_ok=True)
text = (DOCS / "FULL_DIVE_HAZARD_ANALYSIS.md").read_text(encoding="utf-8")
hazards = sorted(set(re.findall(r"H-\d{3}", text)))
open_high = high_severity_open_hazards()
out = GENERATED / "RiskReport.md"
out.write_text("# RiskReport\n\n" + f"Hazards indexed: {len(hazards)}\n\n" + f"High-severity hazards requiring independent review: {len(open_high)}\n\n" + "## Hazard IDs\n" + "\n".join(f"- {h}" for h in hazards) + "\n")
print(out)
