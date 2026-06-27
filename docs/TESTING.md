# Testing

Run all tests:
```bash
ctest --test-dir build/debug --output-on-failure
```

Focused Full Unlocked tests:
```bash
ctest --test-dir build/debug -R '^(runtime_mode_test|full_unlocked_runtime_test|pipeline_state_machine_test|safety_decision_test|performance_smoke_test|replay_consistency_test|timebase_sync_test)$' --output-on-failure
```
