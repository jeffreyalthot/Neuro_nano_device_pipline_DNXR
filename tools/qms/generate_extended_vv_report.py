#!/usr/bin/env python3
from qms_lib import GENERATED, READINESS_LANE_COMMANDS, disabled_test_impact

GENERATED.mkdir(parents=True, exist_ok=True)
out = GENERATED / "ExtendedVVReport.md"
out.write_text("# ExtendedVVReport\n\n## Clinical-readiness validation lane\n" + "\n".join(f"```bash\n{cmd}\n```" for cmd in READINESS_LANE_COMMANDS) + "\n\n## Critical tests covered by this lane\n" + "\n".join(f"- {i['test']}" for i in disabled_test_impact()) + "\n\n## Acceptance rule\nAll enabled tests must pass, generated readiness/gap reports must be attached, and any skipped long test must have a documented clinical-readiness impact assessment.\n")
print(out)
