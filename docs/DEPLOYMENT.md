# Deployment

Debug configure/build:
```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug -DDNXR_BUILD_TESTS=ON
cmake --build build/debug -j
```

Release configure/build:
```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release -DDNXR_BUILD_TESTS=ON
cmake --build build/release -j
```

Run standard/simulation/full-unlocked/diagnostic/replay/benchmark:
```bash
./build/release/distributed_neural_xr --mode standard --headless --cycles=10
./build/release/distributed_neural_xr --mode simulation --headless --cycles=10
./build/release/distributed_neural_xr --full-unlocked --headless --cycles=10
./build/release/distributed_neural_xr --diagnostic --headless --cycles=10
./build/release/distributed_neural_xr --replay --headless --cycles=10
./build/release/distributed_neural_xr --benchmark --headless --cycles=1
```


## Prompt 18 Deployment Boundary

Production deployment must use the canonical BCI runtime only. `DNXR_SIMULATION_ONLY=OFF` builds must not link simulated or HIL chip transports into `distributed_neural_xr`; the Prompt 18 real-binary symbol scan is recorded in `docs/runtime_unification/DNXR_FINAL_RUNTIME_ACCEPTANCE_REPORT.md`. A deployment without hardware attestation, signed evidence, real ACK/telemetry semantics, patient binding, and clinical authorization is expected to fail closed. Chemical channels are off unless explicitly authorized by policy and evidence.
