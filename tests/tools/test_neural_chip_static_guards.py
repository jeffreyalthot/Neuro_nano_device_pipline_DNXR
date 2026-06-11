#!/usr/bin/env python3
"""Adversarial mutations proving neural-chip static guards fail closed."""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def run(script: str) -> subprocess.CompletedProcess:
    return subprocess.run(["python3", str(ROOT / "tools" / script)], cwd=ROOT, text=True, capture_output=True)


def assert_fails_with_file(relative_path: str, content: str, script: str) -> None:
    path = ROOT / relative_path
    path.parent.mkdir(parents=True, exist_ok=True)
    existed = path.exists()
    original = path.read_text() if existed else None
    try:
        path.write_text(content)
        result = run(script)
        assert result.returncode != 0, f"{script} unexpectedly accepted {relative_path}\nstdout={result.stdout}\nstderr={result.stderr}"
    finally:
        if existed:
            path.write_text(original)
        else:
            path.unlink(missing_ok=True)


def test_static_guards_reject_adversarial_sources() -> None:
    assert_fails_with_file("src/full_dive/visual_chip/__dnxr_mutation_probe.cpp", '#include "neural_chip/NeuralChipBus.h"\n', "check_no_direct_neural_chip_bus_access.py")
    assert_fails_with_file("src/neural_chip/__dnxr_mutation_probe.cpp", "float microAmp = 1.0f;\n", "check_no_electrical_stimulation_parameters.py")
    assert_fails_with_file("docs/__dnxr_mutation_probe.md", "This build is human ready.\n", "check_no_human_ready_claims.py")
    assert_fails_with_file("tests/__dnxr_mutation_probe.cpp", "void f(){ rejectWithHold(frame, reason, 0.0f, read); }\n", "check_reject_with_hold_requires_path_kind.py")


if __name__ == "__main__":
    test_static_guards_reject_adversarial_sources()
