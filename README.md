# Neuro_nano_device_pipline

Référentiel C++ d’un pipeline **simulé** "nano-device ↔ casque XR ↔ ordinateur" pour immersion profonde expérimentale (R&D conceptuelle).

## Arborescence
Voir `src/` pour l’architecture modulaire complète: core, devices, network, signal, cognition, calibration, immersion, safety, rendering, utils.

## Build
```bash
cmake -S . -B build
cmake --build build
./build/distributed_neural_xr
```

## Note importante
Consultez `TERMS_OF_USE.md` avant toute utilisation.
