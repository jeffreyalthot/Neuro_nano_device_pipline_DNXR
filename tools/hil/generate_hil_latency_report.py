#!/usr/bin/env python3
import argparse

from hil_lib import QMS_HIL, VERSION, ensure_dirs, scenario_records


def parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Generate versioned HIL latency evidence report.")
    parser.add_argument("--version", default=VERSION, help="software evidence version to stamp on generated artifacts")
    return parser


def main(version: str | None = None):
    version = version or VERSION
    ensure_dirs()
    path = QMS_HIL / "latency_reports" / f"HIL-LATENCY-{version}.md"
    records = scenario_records(version)
    lines = [
        f"# HIL Latency Report {version}",
        "",
        "## Acceptance criterion",
        "Each HIL scenario must keep p99 latency at or below 20.0 ms in the software evidence lane. Physical HIL confirmation remains required for clinical-test promotion.",
        "",
        "| Scenario ID | Average ms | p95 ms | p99 ms | Worst ms | Budget ms | Status |",
        "| --- | --- | --- | --- | --- | --- | --- |",
    ]
    for r in records:
        lines.append(f"| {r.scenario_id} | {r.average_ms:.2f} | {r.p95_ms:.2f} | {r.p99_ms:.2f} | {r.worst_ms:.2f} | 20.00 | {'PASS' if r.p99_ms <= 20.0 else 'FAIL'} |")
    lines += ["", "## Traceability", f"- Session report: qms/hil/session_reports/HIL-SESSION-{version}.md", f"- Risk coverage: qms/hil/risk_coverage/HIL-RISK-COVERAGE-{version}.csv"]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(path)
    return path


if __name__ == "__main__":
    args = parser().parse_args()
    main(args.version)
