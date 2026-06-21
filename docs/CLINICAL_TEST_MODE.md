# Clinical test mode

Clinical test mode is a controlled investigation runtime path. It is enabled at build time with `DNXR_CLINICAL_TEST_BUILD=ON`, but runtime capabilities are still decided by signed artifacts and session state.

```bash
cmake -S . -B build_clinic -DDNXR_CLINICAL_TEST_BUILD=ON -DDNXR_SIMULATION_ONLY=OFF
cmake --build build_clinic
```

The human-use gate allows clinical review after QMS validation, human connection/recording/calibration after regulatory and investigator validation, and stimulus dispatch only for a valid signed session with monitoring and emergency stops active.
