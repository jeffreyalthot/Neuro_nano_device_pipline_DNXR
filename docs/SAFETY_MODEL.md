# Safety Model

Full Unlocked exposes safety decisions; it does not remove critical protections. `SafetyDecision` records the module, rule, expected value, received value, severity, action, reproduction hint, and correction. `EmergencyLatchController` requires an operator id and reset reason before clearing a latch.
