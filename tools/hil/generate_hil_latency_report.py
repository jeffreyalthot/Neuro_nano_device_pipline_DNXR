#!/usr/bin/env python3
from hil_lib import QMS_HIL, VERSION, ensure_dirs, scenario_records


def main():
    ensure_dirs()
    path = QMS_HIL / "latency_reports" / f"HIL-LATENCY-{VERSION}.md"
    records = scenario_records()
    lines = [
        "# HIL Latency Report 0.4.1",
        "",
        "## Acceptance criterion",
        "Each HIL scenario must keep p99 latency at or below 20.0 ms in the software evidence lane. Physical HIL confirmation remains required for clinical-test promotion.",
        "",
        "| Scenario ID | Average ms | p95 ms | p99 ms | Worst ms | Budget ms | Status |",
        "| --- | --- | --- | --- | --- | --- | --- |",
    ]
    for r in records:
        lines.append(f"| {r.scenario_id} | {r.average_ms:.2f} | {r.p95_ms:.2f} | {r.p99_ms:.2f} | {r.worst_ms:.2f} | 20.00 | {'PASS' if r.p99_ms <= 20.0 else 'FAIL'} |")
    lines += ["", "## Traceability", f"- Session report: qms/hil/session_reports/HIL-SESSION-{VERSION}.md", f"- Risk coverage: qms/hil/risk_coverage/HIL-RISK-COVERAGE-{VERSION}.csv"]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(path)
    return path


if __name__ == "__main__":
    main()
