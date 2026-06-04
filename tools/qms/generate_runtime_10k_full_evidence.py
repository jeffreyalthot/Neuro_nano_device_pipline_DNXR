#!/usr/bin/env python3
"""Run the full controller 10k lane and persist Runtime10kFullEvidence.md only after execution."""
import json
import os
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
GENERATED = ROOT / "qms" / "generated"
TARGET = BUILD / "dnxr_runtime_controller_pipeline_10k_cycles_test"
TRANSCRIPT = []


def run(command, env=None):
    completed = subprocess.run(command, cwd=ROOT, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    rendered = "$ " + " ".join(map(str, command))
    print(rendered)
    print(completed.stdout)
    TRANSCRIPT.append(rendered)
    TRANSCRIPT.append(completed.stdout)
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
        GENERATED.mkdir(parents=True, exist_ok=True)
        (GENERATED / "ReadinessLaneTranscript-Full10k.md").write_text("# ReadinessLaneTranscript-Full10k\n\n```text\n" + "\n".join(TRANSCRIPT) + "\n```\n", encoding="utf-8")
        (GENERATED / "ReadinessLaneSummary.json").write_text(json.dumps({"bounded_lane": {"status": "SEE_BOUNDED_TRANSCRIPT", "transcript": "qms/generated/ReadinessLaneTranscript-Bounded.md"}, "full_10k_lane": {"status": "PASS", "transcript": "qms/generated/ReadinessLaneTranscript-Full10k.md", "artifact": "qms/generated/Runtime10kFullEvidence.md"}, "rule": "Full 10k execution is proven only by the full lane transcript and Runtime10kFullEvidence."}, indent=2) + "\n", encoding="utf-8")
        return 0
    GENERATED.mkdir(parents=True, exist_ok=True)
    (GENERATED / "ReadinessLaneTranscript-Full10k.md").write_text("# ReadinessLaneTranscript-Full10k\n\nStatus: FAIL\n\n```text\n" + "\n".join(TRANSCRIPT) + "\n```\n", encoding="utf-8")
    print("Full 10k command finished without producing Runtime10kFullEvidence.md", file=sys.stderr)
    return rc or 1


if __name__ == "__main__":
    sys.exit(main())
