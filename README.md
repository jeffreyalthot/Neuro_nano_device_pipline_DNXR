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

## Multi-device compatibility (sensor/emitter BCI)
The neural device interface is configurable for multiple BCI sensor/emitter ecosystems
(for example vendor profiles like `neuralink`, `blackrock`, `openbci`, or any custom backend).

Set runtime values through environment variables:

- `DNXR_NANO_VENDOR` (default: `generic`)
- `DNXR_NANO_SIGNAL_PROTOCOL` (default: `neural-stream-v1`)
- `DNXR_NANO_ENDPOINT`
- `DNXR_NANO_AUTH_TOKEN`
- `DNXR_NANO_ACCESS_PORTS` (default: `usb,bluetooth,wireless,ethernet,wifi,cellular,nfc,satellite,proprietary`)


## For Give Help For Developpement And Idees Giveway.
Bitcoin address for donation big thanck you so mutch !

1C7HaBc8oZLq7d54ni8E6XNEKF8miLEceY
