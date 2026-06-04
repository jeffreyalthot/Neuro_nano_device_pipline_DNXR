#!/usr/bin/env python3
"""Run the full controller 10k lane and persist Runtime10kFullEvidence.md only after execution."""
import os
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
TARGET = BUILD / "dnxr_runtime_controller_pipeline_10k_cycles_test"


def run(command, env=None):
    completed = subprocess.run(command, cwd=ROOT, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print("$ " + " ".join(map(str, command)))
    print(completed.stdout)
    return completed.returncode


def main() -> int:
    if os.environ.get("DNXR_CONTROLLER_10K_FULL") not in {"1", "true", "TRUE", "on", "ON"}:
        print("Refusing to generate Runtime10kFullEvidence.md: set DNXR_CONTROLLER_10K_FULL=1 for the actual full 10k execution lane.", file=sys.stderr)
        return 2
    if run(["cmake", "-S", ".", "-B", str(BUILD), "-DDNXR_ENABLE_CONTROLLER_10K_TEST=ON"]) != 0:
        return 1
    if run(["cmake", "--build", str(BUILD), "--target", "dnxr_runtime_controller_pipeline_10k_cycles_test", "-j2"]) != 0:
        return 1
    env = os.environ.copy()
    env["DNXR_CONTROLLER_10K_FULL"] = "1"
    env.setdefault("DNXR_CONTROLLER_10K_PROGRESS", "1")
    rc = run([str(TARGET)], env=env)
    evidence = ROOT / "qms" / "generated" / "Runtime10kFullEvidence.md"
    if rc == 0 and evidence.exists():
        print(evidence)
        return 0
    print("Full 10k command finished without producing Runtime10kFullEvidence.md", file=sys.stderr)
    return rc or 1


if __name__ == "__main__":
    sys.exit(main())
