# Software Delta Summary 0.4.2

## Scope

This summary identifies the software and generated-evidence delta introduced by 0.4.2.

## Runtime delta

- Added a bounded default mode for the runtime controller 10k validation path.
- Preserved an opt-in full execution path through `DNXR_CONTROLLER_10K_FULL=1`.
- Added timing/projection fields to runtime evidence so routine validation can complete without an external timeout.

## QMS/reporting delta

- Updated generated clinical-readiness reporting to detect persistent HIL scenario artifacts.
- Kept the regulatory conclusion conservative: bounded runtime software evidence and simulated HIL evidence do not establish ClinicalTestReady.

## Known limitations

- Historical HIL artifacts remained stamped 0.4.1 in the 0.4.2 package.
- Runtime 10k evidence was still represented by a single report name, which could blur bounded projection evidence and full execution evidence.
- HIL coverage remained partial and required either additional scenarios or explicit non-HIL-applicable justifications.

## 0.4.3 software objectives

- Add explicit version arguments to HIL generators.
- Emit `Runtime10kBoundedEvidence.md` and `Runtime10kFullEvidence.md` separately.
- Report bounded evidence as routine readiness evidence only, not as a substitute for full 10k execution evidence.
- Extend generated HIL software scenarios to cover all 18 known HIL hazards.
