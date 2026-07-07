# DNXR 0.7.1 definitive QMS/NDA package

Version: 0.7.1
Status: QMS-controlled engineering release candidate for enterprise due diligence. This package is not a claim of cleared human clinical use, market authorization, or permission to bypass the neural-chip authorization boundary.

## Runtime completion statement

The primary runtime is the authoritative middleware path for enterprise integration:

```text
3D engine / XR world
  -> VirtualWorldDataIngress
  -> RuntimePipelineRunner / RuntimeFullDiveStage
  -> RuntimeStimulusStage
  -> PPPGrammarFrame audit
  -> INeuralChipHAL
  -> neural-chip ACK / feedback
```

The secondary spike-vector runtime remains the high-fidelity simulation/HIL evidence path. `RuntimePathLock` prevents accidental fallback from a selected real primary runtime to the simulation path during one controller lifetime.

## Visual-system conformance

The visual path is complete for middleware integration in 0.7.1:

- `VirtualWorldDataIngress` accepts 3D camera, field-of-view, depth layers, foveal/peripheral awareness, retinal stability, contrast, and visual-vestibular mismatch.
- `VisualCorticalWorldEncoder` converts the visual scene into `NeuralVisualVector` fields for luminance, contrast, edge map, motion flow, near/mid/far depth, object saliency, navigation corridor, hazard map, avatar body visibility, text priority, phosphene density/stability, flicker risk, and visual-fatigue risk.
- `VisualCorticalWorldEncoder::validateEncoded` provides the QMS visual compliance decision before visual data is treated as an accepted neural-vector candidate.
- `.ppp` grammar construction keeps visual channels readable and auditable before any spike candidate can reach the neural-chip transport boundary.
- Visual fatigue, flicker, flash-pattern, visual/vestibular conflict, active-ceiling, disabled-modality, and emergency-stop controls remain mandatory risk controls.

## Neural-chip sale / integration boundary

DNXR is middleware. It proposes, audits, and gates communication. The neural chip remains the final authorization layer and must reject invalid, unsafe, ungrammatized, over-topology, missing-ACK, or over-ceiling stimuli. Enterprise integrations must implement real transport only behind `INeuralChipHAL` and must preserve the existing HAL ACK, topology, cutoff, and feedback semantics.

## QMS evidence package map

Required review package for enterprise NDA transfer:

- Architecture: `docs/ARCHITECTURE.md`, `docs/SOFTWARE_ARCHITECTURE_DESCRIPTION.md`, `docs/FULL_DIVE_SOFTWARE_ARCHITECTURE.md`.
- Requirements and design: `docs/SOFTWARE_REQUIREMENTS_SPECIFICATION.md`, `docs/SOFTWARE_DETAILED_DESIGN.md`, `docs/NEURAL_CHIP_MIDDLEWARE_INTEGRATION.md`, `docs/VIRTUAL_WORLD_DATA_INGRESS.md`, `docs/AVATAR_NEURAL_FEEDBACK_CONTROL.md`.
- Risk management: `docs/ISO_14971_RISK_MANAGEMENT_FILE.md`, `docs/FULL_DIVE_HAZARD_ANALYSIS.md`, `docs/RISK_CONTROL_MEASURES.md`, `docs/STIMULUS_SAFETY_CASE.md`, `docs/REALITY_ANCHOR_SAFETY_CASE.md`.
- Verification and validation: `docs/SOFTWARE_VERIFICATION_PLAN.md`, `docs/SOFTWARE_VALIDATION_PLAN.md`, `docs/VALIDATION_MASTER_PLAN.md`, `docs/VALIDATION_REPORT.md`, `docs/TESTING.md`, `docs/LONG_TESTS.md`.
- Cybersecurity and privacy: `docs/CYBERSECURITY_RISK_MANAGEMENT_FILE.md`, `docs/CYBERSECURITY_THREAT_MODEL.md`, `docs/SECURITY_ARCHITECTURE.md`, `docs/PRIVACY_AND_DATA_PROTECTION_PLAN.md`, `docs/DATA_PROTECTION_IMPACT_ASSESSMENT.md`.
- Configuration/release: `VERSION`, `docs/SOFTWARE_CONFIGURATION_MANAGEMENT_PLAN.md`, `docs/SOFTWARE_RELEASE_PROCEDURE.md`, `docs/SBOM.md`, `docs/SOUP_INVENTORY_LOCKED.md`.

## NDA transfer checklist

Before providing this package to an enterprise buyer or partner, include a signed NDA covering:

1. confidential source code, middleware architecture, `.ppp` grammar, spike-thinking logic, safety controls, and test evidence;
2. prohibition on reverse engineering around safety controls, clinical gates, runtime path locks, or neural-chip authorization checks;
3. no human-use, clinical-use, or sales claim beyond the documented regulatory status without independent authorization;
4. buyer obligation to keep real neural-chip transport implementations behind `INeuralChipHAL` and equivalent safety/cybersecurity controls;
5. audit-log, replay, incident-reporting, vulnerability-disclosure, and change-control obligations;
6. return/destruction terms for source, test artifacts, signed evidence gates, and confidential hardware-interface details.

## Release validation command set

For the 0.7.1 controlled release lane, run the default CTest suite plus targeted high-risk tests:

```bash
cmake --build build -j2
ctest --test-dir build --output-on-failure
ctest --test-dir build -R '^(dnxr_visual_cortical_world_encoder_test|dnxr_virtual_world_data_ingress_test|dnxr_neural_chip_only_pipeline_test|dnxr_avatar_kinematic_rig_controller_test|dnxr_neural_chip_hal_test|dnxr_runtime_path_lock_test|dnxr_stimulus_by_spike_runtime_test)$' --output-on-failure
```

Long simulated, HIL, real hardware bench, and clinical lanes must be scheduled under their documented build flags and signed evidence gates before any regulated deployment claim.
