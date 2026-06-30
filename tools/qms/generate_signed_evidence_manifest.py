#!/usr/bin/env python3
from __future__ import annotations
import datetime as dt
import hashlib
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
RELEASE = "0.6.6"
ARTIFACTS = {
    "docs/RND_TECHNICAL_DOSSIER_0_6_5.md": "historical_technical_dossier",
    "docs/RELEASE_NOTES_0_6_6.md": "release_notes",
    "docs/OPEN_RISK_REGISTER_0_6_6.md": "open_risk_register",
    "docs/HARDWARE_BENCH_MODE_0_6_6.md": "hardware_bench_mode",
    "qms/release_records/REL-0.6.6.md": "release_record",
    "qms/generated/HardwareBenchReadiness_0_6_6.md": "hardware_bench_readiness",
}

def sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def main() -> int:
    files = []
    for rel, role in ARTIFACTS.items():
        p = ROOT / rel
        if p.exists():
            files.append({"path": rel, "sha256": sha256(p), "role": role})
    generated = dt.datetime.now(dt.timezone.utc).isoformat()
    payload = {"release": RELEASE, "files": files, "generatedAt": generated, "generator": "dnxr-qms"}
    canonical = json.dumps(payload, sort_keys=True, separators=(",", ":"))
    internal_signature = hashlib.sha256(canonical.encode("utf-8")).hexdigest()
    manifest = {**payload, "signatureStatus": "internal_qms_sha256_signed_index", "signatureAlgorithm": "sha256(canonical_release_payload)", "signatureScope": "internal software evidence integrity only; not a medical/regulatory approval signature", "internalSignature": internal_signature}
    out = ROOT / "qms/generated/SignedEvidenceManifest_0_6_6.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    index = {"release": RELEASE, "generatedAt": generated, "generated_evidence": files, "signedEvidenceManifest": out.relative_to(ROOT).as_posix()}
    (ROOT / "qms/generated/ReleaseEvidenceIndex_0_6_6.json").write_text(json.dumps(index, indent=2) + "\n", encoding="utf-8")
    return 0
if __name__ == "__main__":
    raise SystemExit(main())
