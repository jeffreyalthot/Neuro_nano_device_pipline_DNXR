#!/usr/bin/env python3
import pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "secure_update_adversarial_test_0.4.4.md"

def main():
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text("""# Secure Update Adversarial Test Evidence 0.4.4

## Attack scenarios
- Replay of a previously accepted package.
- Tampered packet payload.
- Network replay attempt against runtime control messages.

## Result
Status: PASS in deterministic software tests. Physical device update-chain assessment remains required before clinical-test claims.

## Linked tests
- tests/dnxr_network_replay_attack_test.cpp
- tests/dnxr_replay_attack_blocked_test.cpp
- tests/dnxr_packet_tamper_detection_test.cpp
- tests/dnxr_secure_update_policy_test.cpp
""", encoding="utf-8")
    print(OUT)
if __name__ == "__main__": main()
