#!/usr/bin/env python3
from pathlib import Path
import re, sys
root = Path(__file__).resolve().parents[1]
cmake = (root / "CMakeLists.txt").read_text()
missing = []
for src in root.glob("tests/**/*.cpp"):
    name = src.stem
    if name not in cmake:
        missing.append(str(src.relative_to(root)))
if missing:
    print("Unregistered tests:")
    print("\n".join(sorted(missing)))
    sys.exit(1)
print("All tests registered in CMakeLists.txt")
