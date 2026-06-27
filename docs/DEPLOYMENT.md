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
