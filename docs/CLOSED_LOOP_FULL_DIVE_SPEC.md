# Closed-Loop Full Dive Specification

A stimulus is never considered complete at dispatch time. The runtime records the expected perception, measured response, perception confidence, correction proposal, safety intervention, and audit trail. Corrections are bounded by patient-specific learning rate, maximum correction per second, and anti-runaway clamps. Divergence triggers decompression or emergency cutoff.
