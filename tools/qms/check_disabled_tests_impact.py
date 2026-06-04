#!/usr/bin/env python3
from qms_lib import disabled_test_impact

impacts = disabled_test_impact()
print("Clinical-readiness disabled-test impact:")
for item in impacts:
    state = "disabled by default" if item["disabled_by_default"] else "not detected as disabled by default"
    print(f"- {item['test']}: {state}; {item['readiness_impact']}")
