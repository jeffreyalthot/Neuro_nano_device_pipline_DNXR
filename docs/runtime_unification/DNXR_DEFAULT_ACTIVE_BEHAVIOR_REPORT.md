# DNXR Default Active Behavior Report

Prompt 01 update, 2026-07-13.

## Startup policy

No argument and syntax-error invocations no longer start a neural cycle. The CLI requires an explicit command. The active command is `run --profile <profile> --config <path>`.

## Influence policy observation

`RuntimeController` still initializes a non-zero `inducedVector` during module initialization. This remains a P0 policy gap for the canonical production profile: the vector must be zeroed or moved into a non-production/research profile before the final runtime can claim canonical active-default neutrality.
