#!/usr/bin/env python3
import csv
from hil_lib import QMS_HIL, VERSION, ensure_dirs, scenario_records


def main():
    ensure_dirs()
    path = QMS_HIL / "risk_coverage" / f"HIL-RISK-COVERAGE-{VERSION}.csv"
    records = scenario_records()
    hazards = sorted({hazard for record in records for hazard in record.hazards})
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["HazardID", "ScenarioIDs", "RiskControls", "Covered", "AllPassing", "EvidenceArtifact", "ReviewerState"])
        for hazard in hazards:
            matched = [record for record in records if hazard in record.hazards]
            controls = sorted({control for record in matched for control in record.controls})
            writer.writerow([
                hazard,
                ";".join(record.scenario_id for record in matched),
                ";".join(controls),
                "true" if matched else "false",
                "true" if matched and all(record.passed for record in matched) else "false",
                f"qms/hil/session_reports/HIL-SESSION-{VERSION}.md",
                "role-reviewed; named sign-off pending",
            ])
    print(path)
    return path


if __name__ == "__main__":
    main()
