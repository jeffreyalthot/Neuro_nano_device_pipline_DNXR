# DNXR Session Bootstrap Ownership Manifest

Prompt 03 update, 2026-07-13.

## New ownership boundary

`SessionBootstrapValidator` validates configuration, profile class, backend attestation evidence, transport declaration, world ingress, timing settings, role counters, and shutdown order before a session can be opened. It returns either an immutable `SessionBootstrapSnapshot` or a structured `SessionBootstrapError`; it does not partially construct runtime state.

`CanonicalRuntimeHost` is the new single-owner graph boundary for the canonical runtime. It accepts an already validated `SessionBootstrapSnapshot` and an owned `RuntimeComposition`, locks the backend mode for the session, and rejects duplicate owner/counter topology.

## Instance ownership counters

A valid bootstrap snapshot must contain exactly one:

- session owner
- monotonic clock
- committer
- ACK handshake
- emergency latch
- state machine

`dnxr_session_bootstrap_validator_test` proves that duplicate owners fail the invariant.

## Backend lock

`SessionBackendLock` is irreversible after session creation: it permits the same backend mode and rejects real/simulation switching. The Prompt 03 test validates that a simulation session cannot be reconfigured to `HardwareBenchNoHuman` after lock.

## Destruction order

The canonical host records this shutdown order in the snapshot:

1. `stop_dispatch`
2. `enter_hold_or_decompression`
3. `drain_ack_feedback`
4. `stop_ingress`
5. `flush_audit`
6. `destroy_transport`

## Real-build profile boundary evidence

The `dnxr_session_bootstrap_validator_test` target is linked directly to `dnxr_runtime_core`, not the broad `dnxr_core` aggregate. In a `DNXR_SIMULATION_ONLY=OFF` build, this target built and ran without requiring observer or simulation adapter targets.

## Remaining migration gap

`RuntimeController` still contains legacy stage construction and compatibility-runner execution. `RuntimeApplication` now performs bootstrap validation before constructing `RuntimeController`, but full ownership transfer to `CanonicalRuntimeHost` remains a Prompt 04/05 migration item.
