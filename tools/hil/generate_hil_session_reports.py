#!/usr/bin/env python3
import pathlib

from hil_lib import BUILD_ID, QMS_HIL, VERSION, ensure_dirs, scenario_records


def main() -> pathlib.Path:
    ensure_dirs()
    path = QMS_HIL / "session_reports" / f"HIL-SESSION-{VERSION}.md"
    records = scenario_records()
    lines = [
        "# HIL Session Report 0.4.1",
        "",
        "## Scope",
        "This report persists protocol-bound HIL scenario outputs for the 0.4.1 evidence lane and links each result to hazards, risk controls, latency statistics, operator role, reviewer role, build ID, and source artifact path.",
        "",
        "## Build metadata",
        f"- Software version: {VERSION}",
        f"- Build ID: {BUILD_ID}",
        "- Execution mode: deterministic simulated HIL evidence generation; physical bench execution remains required before clinical-test claims.",
        "- Overall status: " + ("PASS" if all(r.passed for r in records) else "NOT READY"),
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
    import pathlib
    main()
