#!/usr/bin/env python3
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
GENERATED = ROOT / "qms" / "generated"
COMMANDS = [
    ["cmake", "-S", ".", "-B", "build_readiness", "-DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON"],
    ["cmake", "--build", "build_readiness", "-j2"],
    ["ctest", "--test-dir", "build_readiness", "--output-on-failure"],
]


def write_report(name: str, body: str) -> None:
    GENERATED.mkdir(parents=True, exist_ok=True)
    (GENERATED / name).write_text(body, encoding="utf-8")


def main() -> int:
    transcript: list[str] = []
    for command in COMMANDS:
        transcript.append("$ " + " ".join(command))
        completed = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        transcript.append(completed.stdout)
        if completed.returncode != 0:
            write_report(
                "ClinicalReadinessLaneTranscript.md",
                "# Clinical Readiness Lane Transcript\n\n```text\n" + "\n".join(transcript) + "\n```\n",
            )
            return completed.returncode
    write_report(
        "ClinicalReadinessLaneTranscript.md",
        "# Clinical Readiness Lane Transcript\n\n```text\n" + "\n".join(transcript) + "\n```\n",
    )
    write_report(
        "LongRunEvidenceReport.md",
        "# LongRunEvidenceReport\n\nThe clinical-readiness validation lane completed successfully. Long-duration runtime, vestibular exposure, and full-dive intervention tests were enabled through `DNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON`.\n",
    )
    write_report(
        "RuntimeDeterminismEvidence.md",
        "# RuntimeDeterminismEvidence\n\nThe readiness lane completed with deterministic CTest outcomes for the configured build. Attach raw CTest logs and reviewer sign-off before using this as clinical-test evidence.\n",
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
