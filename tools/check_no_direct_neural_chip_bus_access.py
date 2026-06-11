#!/usr/bin/env python3
"""Reject direct NeuralChipBus includes from sensory encoder packages."""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
FORBIDDEN_DIRS = [
    ROOT / "src/full_dive/visual_chip",
    ROOT / "src/full_dive/auditory_chip",
    ROOT / "src/full_dive/touch_chip",
    ROOT / "src/full_dive/olfactory_chip",
    ROOT / "src/full_dive/gustatory_chip",
]
PATTERNS = (
    '#include "neural_chip/NeuralChipBus.h"',
    '#include <neural_chip/NeuralChipBus.h>',
)
violations = []
for directory in FORBIDDEN_DIRS:
    if not directory.exists():
        continue
    for path in directory.rglob("*"):
        if path.suffix not in {".h", ".hpp", ".cpp", ".cc", ".cxx"}:
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        if any(pattern in text for pattern in PATTERNS):
            violations.append(path.relative_to(ROOT).as_posix())

if violations:
    print("Direct NeuralChipBus access is forbidden in sensory encoders:")
    for violation in violations:
        print(f" - {violation}")
    sys.exit(1)

print("No direct NeuralChipBus access from sensory encoders.")
