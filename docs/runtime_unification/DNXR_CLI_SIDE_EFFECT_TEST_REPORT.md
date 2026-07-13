# DNXR CLI Side Effect Test Report

Prompt 01 forensic rebaseline update, 2026-07-13.

## Executable evidence

- `ctest --test-dir build -R 'dnxr_cli_side_effect_contract_test' --output-on-failure` passed 1/1 tests.
- `./build/distributed_neural_xr --help` returned code 0 and printed usage only.

## Verified contract

`--help` and `--version` are parsed by `RuntimeCli` before `RuntimeApplication` or `RuntimeController` construction. Missing commands and syntax errors return code 2. Active startup requires `run --profile <profile> --config <path>`. `validate-config --config <path>` loads configuration for validation only and does not construct the runtime controller.

## Open gaps

The production executable still links observer/test adapter libraries in the simulation build because `RuntimeController` still owns observer renderer references and simulation packet protection. This is retained as an explicit P0 gap for Prompt 21/22 rather than hidden by a failed link.
