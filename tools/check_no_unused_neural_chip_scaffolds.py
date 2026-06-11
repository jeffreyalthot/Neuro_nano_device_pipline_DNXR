#!/usr/bin/env python3
from pathlib import Path
import sys
ROOT = Path(__file__).resolve().parents[1]
pipeline = ROOT/"src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp"
header = ROOT/"src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
text = pipeline.read_text(errors="ignore") + "\n" + header.read_text(errors="ignore")
required = [
    "NeuralChipReplayRecorder",
    "PerModalityRateAndDosePolicy",
    "NeuralChipSessionSupervisor",
    "NeuralChipAckHandshake",
    "MotorIntentConfidenceGate",
    "AvatarSafetyPoseController",
]
missing = [name for name in required if name not in text]
if missing:
    print("Unused neural-chip scaffold(s): " + ", ".join(missing))
    sys.exit(1)
print("All required neural-chip scaffolds are referenced by the pipeline.")
