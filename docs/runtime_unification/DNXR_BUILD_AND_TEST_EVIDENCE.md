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
