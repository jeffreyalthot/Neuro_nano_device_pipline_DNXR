#!/usr/bin/env python3
"""Fail if a real-runtime binary or link map contains simulation-only symbols."""
from __future__ import annotations

import pathlib
import subprocess
import sys

FORBIDDEN = (
    "SimulatedSignalProvider",
    "NeuralSignalSimulator",
    "BiometricSimulator",
    "SimulatedNeuralChipHAL",
    "SimulatedNeuralChipTransport",
    "SimulationCryptoTransport",
    "ScenarioRunner",
    "DNXR-SIM-FNV1A-HASHCHAIN",
    "simulation_session_key",
)


def read_symbols(path: pathlib.Path) -> str:
    if path.suffix in {".map", ".txt", ".log"}:
        return path.read_text(errors="ignore")
    try:
        return subprocess.check_output(["nm", "-C", str(path)], text=True, errors="ignore")
    except Exception:
        return path.read_bytes().decode("utf-8", errors="ignore")


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("usage: check_real_binary_forbidden_symbols.py <binary-or-link-map>", file=sys.stderr)
        return 2
    path = pathlib.Path(argv[1])
    haystack = read_symbols(path)
    hits = [symbol for symbol in FORBIDDEN if symbol in haystack]
    if hits:
        print("forbidden simulation symbols in real binary:")
        for symbol in hits:
            print(f"  - {symbol}")
        return 1
    print("no forbidden simulation symbols detected")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
