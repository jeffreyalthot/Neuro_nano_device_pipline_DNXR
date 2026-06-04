#!/usr/bin/env python3
import argparse
import csv

from hil_lib import ALL_HIL_HAZARDS, QMS_HIL, VERSION, ensure_dirs, scenario_records


def parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate versioned HIL risk coverage matrix.")
    parser.add_argument("--version", default=VERSION, help="software evidence version to stamp on generated artifacts")
    return parser


def main(version: str | None = None):
    version = version or VERSION
    ensure_dirs()
    path = QMS_HIL / "risk_coverage" / f"HIL-RISK-COVERAGE-{version}.csv"
    records = scenario_records(version)
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["HazardID", "ScenarioIDs", "RiskControls", "Covered", "AllPassing", "EvidenceArtifact", "ReviewerState", "Justification"])
        for hazard in ALL_HIL_HAZARDS:
            matched = [record for record in records if hazard in record.hazards]
            controls = sorted({control for record in matched for control in record.controls})
            writer.writerow([
                hazard,
                ";".join(record.scenario_id for record in matched),
                ";".join(controls),
                "true" if matched else "false",
                "true" if matched and all(record.passed for record in matched) else "false",
                f"qms/hil/session_reports/HIL-SESSION-{version}.md" if matched else "",
                "role-reviewed; named sign-off pending" if matched else "not-covered",
                "covered by deterministic HIL software scenario; physical bench and named sign-off pending" if matched else "missing scenario coverage",
            ])
    print(path)
    return path


if __name__ == "__main__":
    args = parser().parse_args()
    main(args.version)
