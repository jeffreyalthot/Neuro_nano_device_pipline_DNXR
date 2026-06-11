#!/usr/bin/env python3
"""Generate a local neural-chip-only evidence bundle."""
from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
import json
import subprocess

ROOT = Path(__file__).resolve().parents[1]

STATIC_CHECKS = [
    "check_no_direct_neural_chip_bus_access.py",
    "check_neural_chip_pipeline_order.py",
    "check_no_electrical_stimulation_parameters.py",
    "check_no_human_ready_claims.py",
    "check_no_unused_neural_chip_scaffolds.py",
    "check_reject_with_hold_requires_path_kind.py",
    "check_neural_chip_build_invariants.py",
]


def run(cmd: list[str]) -> str:
    result = subprocess.run(cmd, cwd=ROOT, text=True, capture_output=True)
    return f"$ {' '.join(cmd)}\nexit={result.returncode}\n{result.stdout}{result.stderr}"


def main() -> int:
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    out = ROOT / "evidence" / "neural_chip" / stamp
    out.mkdir(parents=True, exist_ok=True)
    (out / "static_checks.txt").write_text("\n".join(run(["python3", str(ROOT / "tools" / check)]) for check in STATIC_CHECKS))
    (out / "cmake_config.txt").write_text(run(["cmake", "-S", ".", "-B", "build", "-DDNXR_BUILD_TESTS=ON"]))
    (out / "ctest_summary.txt").write_text(run(["ctest", "--test-dir", "build", "-R", "neural_chip|pipeline", "--output-on-failure"]))
    (out / "safety_case_report.json").write_text(json.dumps({"generated": stamp, "source": "pipeline safety case records exported by tests"}, indent=2) + "\n")
    (out / "replay_consistency_report.json").write_text(json.dumps({"generated": stamp, "verifier": "ReplayConsistencyVerifier"}, indent=2) + "\n")
    (out / "session_digest.txt").write_text("Session digest evidence is emitted by NeuralChipSessionDigest-backed replay records.\n")
    (out / "forbidden_claims_report.txt").write_text(run(["python3", str(ROOT / "tools" / "check_no_human_ready_claims.py")]))
    print(out.relative_to(ROOT))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
