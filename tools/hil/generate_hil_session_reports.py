#!/usr/bin/env python3
import argparse
import pathlib

from hil_lib import QMS_HIL, VERSION, build_id, ensure_dirs, scenario_records


def parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate versioned HIL session report evidence.")
    parser.add_argument("--version", default=VERSION, help="software evidence version to stamp on generated artifacts")
    return parser


def main(version: str | None = None) -> pathlib.Path:
    version = version or VERSION
    ensure_dirs()
    path = QMS_HIL / "session_reports" / f"HIL-SESSION-{version}.md"
    records = scenario_records(version)
    covered = sorted({hazard for record in records for hazard in record.hazards})
    lines = [
        f"# HIL Session Report {version}",
        "",
        "## Scope",
        f"This report persists protocol-bound HIL scenario outputs for the {version} evidence lane and links each result to hazards, risk controls, latency statistics, operator role, reviewer role, build ID, and source artifact path.",
        "",
        "## Build metadata",
        f"- Software version: {version}",
        f"- Build ID: {build_id(version)}",
        "- Execution mode: deterministic simulated HIL evidence generation; physical bench execution remains required before clinical-test claims.",
        "- Overall status: " + ("PASS" if all(r.passed for r in records) else "NOT READY"),
        f"- HIL hazard coverage: {len(covered)}/18 covered in generated software evidence",
        "",
        "## Scenario results",
        "| Scenario ID | Hazards | Risk controls | Phantom profile | Faults | Expected outcome | Observed outcome | Pass/fail | Average ms | p95 ms | p99 ms | Worst ms | Recovery cycles | Emergency stop | Operator | Reviewer | Timestamp | Artifact path |",
        "| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |",
    ]
    for r in records:
        lines.append(
            f"| {r.scenario_id} | {'; '.join(r.hazards)} | {'; '.join(r.controls)} | {r.phantom_profile} | {'; '.join(r.injected_faults)} | {r.expected_outcome} | {r.observed_outcome} | {'PASS' if r.passed else 'FAIL'} | {r.average_ms:.2f} | {r.p95_ms:.2f} | {r.p99_ms:.2f} | {r.worst_ms:.2f} | {r.recovery_cycles}/{r.required_recovery_cycles} | {'PASS' if r.emergency_stop_passed else 'NOT REQUIRED'} | {r.operator} | {r.reviewer} | {r.timestamp} | {r.artifact_path} |"
        )
    lines += [
        "",
        "## Clinical-readiness limitations",
        "- These generated records make the HIL evidence chain persistent, but do not replace named independent review, physical phantom calibration records, or country-specific clinical authorization review.",
        "- Reviewer roles are recorded so the sign-off gate can block promotion until named reviewers accept residual risk.",
    ]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(path)
    return path


if __name__ == "__main__":
    args = parser().parse_args()
    main(args.version)
