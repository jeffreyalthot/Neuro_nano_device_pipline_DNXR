# DNXR Build and Test Evidence

## Prompt 01 baseline — 2026-07-13

### Pre-modification state

- `git status --short` was executed before artifact creation and returned no tracked/untracked output.
- Repository inventory command counted 1,598 non-git files and 1,223 files under `src/` plus `tests/`.

### Build command

```bash
cmake --preset simulation && cmake --build build/simulation --target distributed_neural_xr dnxr_runtime_closed_loop_feedback_stage_test dnxr_neural_chip_session_supervisor_test dnxr_virtual_world_data_ingress_test -j2
```

Result: PASS. CMake configured the `simulation` preset and built `dnxr_core`, `distributed_neural_xr`, and three representative runtime/chip/world tests successfully.

### Test command

```bash
ctest --test-dir build/simulation -R 'dnxr_runtime_closed_loop_feedback_stage_test|dnxr_neural_chip_session_supervisor_test|dnxr_virtual_world_data_ingress_test' --output-on-failure
```

Result: PASS. 3/3 tests passed:

- `dnxr_virtual_world_data_ingress_test`
- `dnxr_runtime_closed_loop_feedback_stage_test`
- `dnxr_neural_chip_session_supervisor_test`

### Scope limitation

This prompt intentionally did not modify runtime architecture. The build is a baseline evidence capture for later consolidation work, not proof that P0 architectural gaps are resolved.

## Prompt 02 canonical architecture contract — 2026-07-13

Commands executed after adding canonical contracts and static architecture tests:

```bash
cmake --build build/simulation --target distributed_neural_xr dnxr_canonical_runtime_architecture_test dnxr_runtime_path_lock_test -j2
```

```bash
ctest --test-dir build/simulation -R 'dnxr_canonical_runtime_architecture_test|dnxr_runtime_path_lock_test' --output-on-failure
```

Results are recorded in the final response for this prompt.

Prompt 02 result: PASS. `distributed_neural_xr`, `dnxr_canonical_runtime_architecture_test`, and `dnxr_runtime_path_lock_test` built successfully; 2/2 selected tests passed.

## Prompt 03 fusion skeleton — 2026-07-13

```bash
cmake --preset simulation && cmake --build build/simulation --target distributed_neural_xr dnxr_canonical_runtime_architecture_test dnxr_runtime_path_lock_test dnxr_canonical_runtime_single_owner_test -j2
```

Result: PASS. The main executable and the three focused canonical/backend tests built successfully.

```bash
ctest --test-dir build/simulation -R 'dnxr_canonical_runtime_architecture_test|dnxr_runtime_path_lock_test|dnxr_canonical_runtime_single_owner_test' --output-on-failure
```

Result: PASS. 3/3 selected tests passed.

## 2026-07-13 Prompt 04 evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_runtime_render_stage_test dnxr_runtime_acquisition_stage_test dnxr_config_loader_test -j2` — passed after updating stage/config tests for the BCI contracts.
- `cmake --build build --target dnxr_canonical_runtime_headset_independence_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(canonical_runtime_headset_independence|runtime_render_stage|runtime_acquisition_stage|config_loader|runtime_pipeline_runner)_test" --output-on-failure` — passed, 5/5 tests.
- Re-run after observer watchdog rename: `cmake --build build --target distributed_neural_xr dnxr_canonical_runtime_headset_independence_test -j2` — passed.
- Re-run after observer watchdog rename: `ctest --test-dir build -R "dnxr_(canonical_runtime_headset_independence|runtime_render_stage|runtime_acquisition_stage|config_loader|runtime_pipeline_runner)_test" --output-on-failure` — passed, 5/5 tests.

## 2026-07-13 Prompt 05 evidence
- `cmake --build build --target distributed_neural_xr dnxr_runtime_acquisition_stage_test dnxr_runtime_pipeline_runner_test dnxr_runtime_render_stage_test dnxr_config_loader_test dnxr_canonical_runtime_headset_independence_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(canonical_runtime_headset_independence|runtime_render_stage|runtime_acquisition_stage|config_loader|runtime_pipeline_runner)_test" --output-on-failure` — passed, 5/5 tests.
- `rg -n "services\\.neuralChipHal\\.readFrame|nodes_->readNeuralNodeSignals|readNeuralNodeSignals\\(\\)|NullNeuralSignalSource|protect\\(frame.rawNeuralSignals|unprotect\\(encrypted" src/runtime src/core tests/dnxr_canonical_runtime_headset_independence_test.cpp tests/dnxr_runtime_acquisition_stage_test.cpp` — no canonical service double-read/null-source matches; remaining match is legacy callback wiring outside the service-only canonical path.

## 2026-07-13 Prompt 06 evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_canonical_virtual_world_ingress_test dnxr_virtual_world_data_ingress_test dnxr_world_frame_sanitizer_test dnxr_sensor_encoder_output_contract_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(canonical_virtual_world_ingress|virtual_world_data_ingress|world_frame_sanitizer|sensor_encoder_output_contract)_test" --output-on-failure` — passed, 4/4 tests.
- `rg -n "RawSensoryPayload|raw.*payload|pixel|pcm|OpenXR|SteamVR|XRHeadsetInterface" src/immersion/VirtualWorldDataIngress.* tests/dnxr_canonical_virtual_world_ingress_test.cpp` — no matches in canonical world ingress/test files.

## 2026-07-13 Prompt 07 visual BCI spike path evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_visual_cortical_world_encoder_test dnxr_canonical_visual_bci_spike_path_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(visual_cortical_world_encoder|canonical_visual_bci_spike_path)_test" --output-on-failure` — passed, 2/2 tests.
- `rg -n "XRRenderer|XRHeadsetInterface|OpenXR|SteamVR|HMD|eye tracker" tests/dnxr_canonical_visual_bci_spike_path_test.cpp src/full_dive/visual_chip src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h` — passed with no matches in the canonical visual path files/test.

## 2026-07-13 Prompt 08 multimodal BCI spike path evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_engine_spike_core_test dnxr_canonical_visual_bci_spike_path_test dnxr_canonical_multimodal_bci_spike_path_test -j2` — passed after reconfiguring the build tree for the new test target.
- `ctest --test-dir build -R "dnxr_(engine_spike_core|canonical_visual_bci_spike_path|canonical_multimodal_bci_spike_path)_test" --output-on-failure` — passed, 3/3 tests.
- `rg -n "XRRenderer|XRHeadsetInterface|OpenXR|SteamVR|HMD|eye tracker" tests/dnxr_canonical_multimodal_bci_spike_path_test.cpp src/full_dive/spike_bridge/EngineSpikeCore.cpp src/full_dive/auditory_chip src/full_dive/touch_chip src/full_dive/olfactory_chip src/full_dive/gustatory_chip` — passed with no matches in the multimodal canonical test and sense encoder/priority-spike files.

## 2026-07-13 Prompt 09 spike mapping and PPP evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_ppp_grammar_serialization_test dnxr_stimulus_by_spike_runtime_test dnxr_engine_spike_core_test dnxr_canonical_multimodal_bci_spike_path_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(ppp_grammar_serialization|stimulus_by_spike_runtime|engine_spike_core|canonical_multimodal_bci_spike_path)_test" --output-on-failure` — passed, 4/4 tests.
- `rg -n "writeSafeStimulus|sensoryFrame.*write|FullDiveNeuralFrame.*write|XRRenderer|XRHeadsetInterface|OpenXR|SteamVR|HMD|eye tracker" src/full_dive/spike_bridge tests/full_dive/dnxr_ppp_grammar_serialization_test.cpp tests/dnxr_canonical_multimodal_bci_spike_path_test.cpp` — passed with no matches, proving the touched spike bridge/test path has no direct legacy write or headset/render dependency.

## 2026-07-13 Prompt 10 transport/HAL/ACK evidence
- `cmake -S . -B build -DDNXR_BUILD_TESTS=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_neural_chip_transport_lifecycle_test dnxr_neural_chip_ack_required_test dnxr_runtime_path_lock_test -j2` — passed.
- `ctest --test-dir build -R "dnxr_(neural_chip_transport_lifecycle|neural_chip_ack_required|runtime_path_lock)_test" --output-on-failure` — passed, 3/3 tests.
- `rg -n "authorized_real_transport_dispatch|NullNeuralTransport|class INeuralTransport|autoAck" src/hardware src/core tests/dnxr_neural_chip_transport_lifecycle_test.cpp` — passed with no matches; real transport no longer fabricates authorized dispatch, the empty transport shim is removed, and no auto-ACK token appears in the touched transport scope.

## Prompt 11 build and test evidence

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_safety_pipeline_test dnxr_safety_state_machine_test dnxr_integrated_safety_lifecycle_decision_test -j2` — passed.
- `ctest --test-dir build --output-on-failure -R 'dnxr_(safety_pipeline|safety_state_machine|integrated_safety_lifecycle_decision)_test'` — initially failed because the legacy safety state-machine test still expected `bootSelfTestPassed()` to enter `Nominal`; test was migrated to the canonical EvidenceCheck/Connecting/Calibrating/Ready/Running gate sequence.
- `ctest --test-dir build --output-on-failure -R 'dnxr_(safety_pipeline|safety_state_machine|integrated_safety_lifecycle_decision)_test'` — passed after migration; 3/3 tests passed.

## Prompt 12 build and test evidence

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_canonical_closed_loop_motor_world_feedback_test dnxr_neural_chip_only_pipeline_test dnxr_closed_loop_pipeline_integration_test -j2` — passed.
- `ctest --test-dir build --output-on-failure -R 'dnxr_(canonical_closed_loop_motor_world_feedback|neural_chip_only_pipeline|closed_loop_pipeline_integration)_test'` — initially failed because the new regression expected low-confidence motor intent to normal-dispatch; the assertion was corrected to require safety hold/safety pose.
- `ctest --test-dir build --output-on-failure -R 'dnxr_(canonical_closed_loop_motor_world_feedback|neural_chip_only_pipeline|closed_loop_pipeline_integration)_test'` — passed after correction; 3/3 tests passed.
- `rg -n "XRRenderer|XRHeadsetInterface|OpenXR|SteamVR|HMD|eye tracker|runFullBodyControlTick|ImmersionCore|XRFullBodyCommand" src/full_dive/chip_pipeline src/full_dive/motor_chip` — passed with no matches, confirming the touched canonical motor/feedback path has no renderer/headset/body-command fallback dependency.

## Prompt 13 build and test evidence

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build --target distributed_neural_xr dnxr_runtime_disposition_manifest_audit_test -j2` — passed.
- `ctest --test-dir build --output-on-failure -R 'dnxr_runtime_disposition_manifest_audit_test'` — initially failed because `ClinicalPacketSigner` had no pairwise public-type preservation row; 40 pairwise symbol rows were added and the test passed.
- `./build/dnxr_runtime_disposition_manifest_audit_test` — passed and reported coverage for 920 source files, 477 headers, and 1168 public type declarations.
- `cmake -S . -B build_research -DCMAKE_BUILD_TYPE=Debug -DDNXR_ENABLE_RESEARCH_SPECULATION=ON -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` — passed.
- `cmake --build build_research --target distributed_neural_xr dnxr_runtime_disposition_manifest_audit_test -j2` — passed with research/speculative modules enabled.
- `ctest --test-dir build_research --output-on-failure -R 'dnxr_runtime_disposition_manifest_audit_test'` — passed in the research-enabled build.

## Prompt 14 — legacy/concurrent runtime removal evidence

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` passed.
- `cmake --build build --target distributed_neural_xr dnxr_runtime_pipeline_legacy_input_test dnxr_runtime_pipeline_runner_test dnxr_runtime_single_pipeline_structure_test dnxr_runtime_disposition_manifest_audit_test -j2` passed after migrating the runner test away from `LegacyRunInput` and correcting the structure test to use runtime assertions.
- `ctest --test-dir build --output-on-failure -R 'dnxr_runtime_(pipeline_legacy_input|pipeline_runner|single_pipeline_structure|disposition_manifest_audit)_test'` passed: 4/4 tests.
- `cmake -S . -B build_real -DCMAKE_BUILD_TYPE=Debug -DDNXR_SIMULATION_ONLY=OFF -DDNXR_ENABLE_LONG_SIMULATED_TESTS=OFF -DDNXR_ENABLE_EXTENDED_VV_TESTS=OFF` passed.
- `cmake --build build_real --target distributed_neural_xr dnxr_runtime_single_pipeline_structure_test -j2` passed after excluding simulation crypto/observer renderer from the real source graph and guarding real-runtime observer renderer members.
- `ctest --test-dir build_real --output-on-failure -R 'dnxr_runtime_single_pipeline_structure_test'` passed: 1/1 test.
- `if nm -C build_real/distributed_neural_xr | rg 'XRHeadsetInterface|XRRenderer|SimulatedSignalProvider|NeuralSignalSimulator|BiometricSimulator|ScenarioRunner|SimulationCryptoTransport|NeuralCryptoTransport|HILController|runLegacy|LegacyRunInput|SecondarySimulation|PrimaryReal|autoAck'; then echo 'Forbidden runtime symbol found' >&2; exit 1; fi` passed with no forbidden symbols.
