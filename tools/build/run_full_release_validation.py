#!/usr/bin/env python3
"""Run reproducible 0.6.5 release validation and write QMS summaries."""
from __future__ import annotations
import datetime as dt
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
GENERATED = ROOT / "qms" / "generated"
COMMANDS = [
    ["cmake", "-S", ".", "-B", "build", "-G", "Ninja", "-DDNXR_BUILD_TESTS=ON", "-DDNXR_ENABLE_CONTROLLER_10K_TEST=ON", "-DDNXR_ENABLE_CONTROLLER_100K_TEST=ON", "-DDNXR_ENABLE_LONG_SIMULATED_TESTS=ON", "-DDNXR_ENABLE_LONG_REAL_TESTS=ON"],
    ["cmake", "--build", "build", "-j"],
    ["ctest", "--test-dir", "build", "--output-on-failure"],
    [sys.executable, "tools/check_neural_chip_build_invariants.py"],
    [sys.executable, "tools/qms/check_required_docs.py"],
    [sys.executable, "tools/qms/check_traceability_completeness.py"],
    [sys.executable, "tools/qms/check_disabled_tests_impact.py"],
    [sys.executable, "tools/hil/run_hil_bench_session.py", "--iterations", "1000"],
    [sys.executable, "tools/generate_neural_chip_evidence_bundle.py"],
]

def write_report(name: str, lines: list[str]) -> None:
    GENERATED.mkdir(parents=True, exist_ok=True)
    (GENERATED / name).write_text("\n".join(lines) + "\n", encoding="utf-8")

def main() -> int:
    results: list[tuple[list[str], int]] = []
    for cmd in COMMANDS:
        proc = subprocess.run(cmd, cwd=ROOT, text=True)
        results.append((cmd, proc.returncode))
        if proc.returncode != 0:
            break
    now = dt.datetime.now(dt.timezone.utc).isoformat()
    lines = ["# Full Build Validation 0.6.5", "", f"Generated: {now}", "", "Human-use remains blocked; this runner only validates software evidence.", ""]
    for cmd, code in results:
        lines.append(f"- `{' '.join(cmd)}` => {code}")
    write_report("FullBuildValidation_0_6_5.md", lines)
    write_report("CTestSummary_0_6_5.md", ["# CTest Summary 0.6.5", "", f"Generated: {now}", "", "See FullBuildValidation_0_6_5.md for command return codes."])
    write_report("StaticChecks_0_6_5.md", ["# Static Checks 0.6.5", "", f"Generated: {now}", "", "Static/QMS checks are included in the release validation command list."])
    return 0 if results and results[-1][1] == 0 and len(results) == len(COMMANDS) else 1

if __name__ == "__main__":
    raise SystemExit(main())
