#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / 'tools'
checks = [
    'check_no_direct_neural_chip_bus_access.py',
    'check_neural_chip_pipeline_order.py',
    'check_no_electrical_stimulation_parameters.py',
    'check_no_human_ready_claims.py',
    'check_no_unused_neural_chip_scaffolds.py',
    'check_reject_with_hold_requires_path_kind.py',
]
for check in checks:
    result = subprocess.run([sys.executable, str(TOOLS / check)], cwd=ROOT)
    if result.returncode != 0:
        sys.exit(result.returncode)

pipeline = (ROOT / 'src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp').read_text(errors='ignore')
replay_h = (ROOT / 'src/neural_chip/NeuralChipReplayRecorder.h').read_text(errors='ignore')
replay_cpp = (ROOT / 'src/neural_chip/NeuralChipReplayRecorder.cpp').read_text(errors='ignore')
required = {
    'NeuralChipPipelineStateMachine': 'stateMachine_',
    'DecisionTraceCoverageValidator': 'traceCoverageValidator_',
    'EmergencyLatchController': 'emergencyLatchController_',
    'PipelinePathKind replay field': 'pipelinePathKind',
}
for label, token in required.items():
    haystack = pipeline if label != 'PipelinePathKind replay field' else replay_h + replay_cpp
    if token not in haystack:
        print(f'missing invariant token: {label} ({token})')
        sys.exit(1)
print('neural chip build invariants passed')
