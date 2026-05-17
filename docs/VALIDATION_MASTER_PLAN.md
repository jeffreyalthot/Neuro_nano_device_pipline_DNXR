# Validation Master Plan (VMP) - Draft

## Objective
Define verification and validation strategy for transition from simulation scaffold to regulated development process.

## Verification levels
1. Unit verification (module-level correctness)
2. Integration verification (pipeline behavior)
3. System verification (timing/safety/performance envelopes)
4. Security verification (threat controls)
5. Human factors/usability verification

## Minimum acceptance criteria (draft)
- 100% critical safety requirements mapped to tests and evidence.
- No open high-risk hazards without approved mitigation.
- Deterministic runtime evidence under defined load profiles.
- Full bidirectional traceability complete.

## Evidence control
- Test protocol ID
- Test execution date and environment
- Raw logs/artifacts hash
- Reviewer signature

## Out of scope in this repo
- Clinical trial execution
- Regulatory submission and authority communication
- Implant/hardware human-use activation
