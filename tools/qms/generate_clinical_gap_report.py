#!/usr/bin/env python3
from qms_lib import GENERATED, disabled_test_impact, high_severity_open_hazards

GENERATED.mkdir(parents=True, exist_ok=True)
blocking = [
    "Signed independent risk review is missing.",
    "Protocol-bound HIL scenario reports are missing.",
    "Canada ITA and US IDE pathway assumptions require qualified regulatory review.",
    "Clinical protocol, investigator package, informed consent, monitoring plan, and adverse-event reporting plan are missing.",
]
warnings = [
    "Normal CTest evidence supports integrated simulation, not clinical readiness.",
    "Cybersecurity package lacks penetration-test and vulnerability scan execution evidence.",
]
out = GENERATED / "ClinicalGapReport.md"
out.write_text("# ClinicalGapReport\n\n## Blocking gaps\n" + "\n".join(f"- {b}" for b in blocking) + "\n\n## Warning gaps\n" + "\n".join(f"- {w}" for w in warnings) + "\n\n## Open high-severity hazards\n" + "\n".join(f"- {h}" for h in high_severity_open_hazards()) + "\n\n## Disabled-test impact\n" + "\n".join(f"- {i['test']}: {i['readiness_impact']}" for i in disabled_test_impact()) + "\n")
print(out)
