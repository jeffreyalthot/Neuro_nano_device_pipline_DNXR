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

## Windows / MinGW troubleshooting
If you get an error like:

`Le point d'entrée de procédure _ZNKSt25__codecvt_utf8_utf16_baselwE10do_unshiftERiPcS2_RS2_ est introuvable`

the executable was usually built with one MinGW/GCC runtime and is started with another `libstdc++-6.dll` at runtime.

Recommended fix:

1. Delete previous build artifacts (`build/`).
2. Ensure a **single** compiler toolchain is used end-to-end (configure, build, and run from the same terminal/environment).
3. Reconfigure and rebuild:

   ```bash
   cmake -S . -B build -G "MinGW Makefiles"
   cmake --build build --clean-first
   ```

4. Check which runtime DLL your executable loads (`libstdc++-6.dll`) and make sure it comes from the same MinGW installation as `g++.exe`.

Optional hardening for local binaries:

```cmake
target_link_options(distributed_neural_xr PRIVATE -static-libstdc++ -static-libgcc)
```

This often avoids runtime conflicts on machines with multiple MinGW installations.

## Important note
Read `TERMS_OF_USE.md` before any use of this project.


## For Give Help For Developpement And Idees Giveway.
Bitcoin address for donation big thanck you so mutch !

1C7HaBc8oZLq7d54ni8E6XNEKF8miLEceY

## Clinical readiness
This repository is a simulation scaffold and is **not** a certified clinical, regulatory, or deterministic real-time implant stack.
See `docs/CLINICAL_READINESS_GAP_ANALYSIS.md` for the gap analysis and roadmap.
For clinic-facing technical transfer material, see `docs/NEURALINK_CLINIC_TRANSFER_PACKAGE.md`.


## Dépannage Windows : erreur d'entrée de procédure introuvable
Si Windows affiche une erreur du type `_ZNKSt25__codecvt_utf8_utf16_baselwE10do_unshift... introuvable`, l'exécutable a généralement été lancé avec une version incompatible de `libstdc++-6.dll` (cas fréquent avec MinGW).

Correctifs appliqués dans ce projet :
- Le runtime C/C++ est lié statiquement par défaut sur Windows (`DNXR_WINDOWS_PORTABLE_RUNTIME=ON`).
- Cela évite la dépendance à une `libstdc++-6.dll` externe potentiellement incompatible.

Rebuild conseillé :
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
```
