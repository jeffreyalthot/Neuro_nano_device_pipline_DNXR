# Neuro_nano_device_pipline

A C++ repository for a **simulated** "nano-device ↔ XR headset ↔ host computer" pipeline focused on deep-immersion experimental R&D.

## Structure
See `src/` for the modular architecture: core, devices, network, signal, cognition, calibration, immersion, safety, rendering, and utilities.

## Build
```bash
cmake -S . -B build
cmake --build build
./build/distributed_neural_xr
```

## Important note
Read `TERMS_OF_USE.md` before any use of this project.
