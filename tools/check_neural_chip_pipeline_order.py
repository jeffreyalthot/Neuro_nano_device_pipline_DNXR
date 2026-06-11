#!/usr/bin/env python3
"""Textual guardrail for the neural-chip-only pipeline stage order."""
from pathlib import Path
import sys

PIPELINE = Path("src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp")
ORDER = [
    "MotorCortexReadFrameAdapter",
    "MotorIntentDecoder",
    "AvatarKinematicRigController",
    "PerModalityIsolationBroker",
    "PatientModalityPolicyGate",
    "ExperimentalChemicalChannelGate",
    "ClosedLoopPerceptionErrorController",
    "MultisensoryPhaseLockOrchestrator",
    "ModalityExposureLedger",
    "ChipCapabilityContract",
    "PerModalitySafetyEnvelope",
    "ChipRealityAnchorHardChannel",
    "NeuralVectorSlewRateLimiter",
    "FrameLifecycleGuard",
    "NeuralVectorValidator",
    "NeuralChipBus",
]
ALIASES = {
    "MotorIntentDecoder": "motorDecoder_",
    "AvatarKinematicRigController": "avatarRig_",
    "PerModalityIsolationBroker": "isolationBroker_",
    "PatientModalityPolicyGate": "PatientModalityPolicyGate",
    "ExperimentalChemicalChannelGate": "ExperimentalChemicalChannelGate",
    "ClosedLoopPerceptionErrorController": "ClosedLoopPerceptionErrorController",
    "MultisensoryPhaseLockOrchestrator": "MultisensoryPhaseLockOrchestrator",
    "ModalityExposureLedger": "ModalityExposureLedger",
    "ChipCapabilityContract": "ChipCapabilityContract",
    "PerModalitySafetyEnvelope": "PerModalitySafetyEnvelope",
    "ChipRealityAnchorHardChannel": "ChipRealityAnchorHardChannel",
    "NeuralVectorSlewRateLimiter": "NeuralVectorSlewRateLimiter",
    "FrameLifecycleGuard": "FrameLifecycleGuardCommitBoundary",
    "NeuralVectorValidator": "NeuralVectorValidatorCommitBoundary",
    "NeuralChipBus": "NeuralChipBusCommitBoundary",
}

def main() -> int:
    text = PIPELINE.read_text()
    start = text.find("bool NeuralChipOnlyFullDivePipeline::step")
    if start < 0:
        print("missing step()", file=sys.stderr)
        return 1
    end = text.find("const AvatarRigCommandFrame&", start)
    text = text[start:end if end > start else len(text)]
    last = -1
    for stage in ORDER:
        token = ALIASES.get(stage, stage)
        pos = text.find(token)
        if pos < 0:
            print(f"missing pipeline stage: {stage} ({token})", file=sys.stderr)
            return 1
        if pos < last:
            print(f"pipeline stage out of order: {stage}", file=sys.stderr)
            return 1
        last = pos
    forbidden = ["chipBus_.submitSensoryFrame", "chipBus_.submitSafetyHold"]
    for token in forbidden:
        if token in text:
            print(f"direct bus submit bypass detected: {token}", file=sys.stderr)
            return 1
    return 0

if __name__ == "__main__":
    sys.exit(main())
