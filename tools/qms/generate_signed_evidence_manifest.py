#!/usr/bin/env python3
from __future__ import annotations
import datetime as dt
import hashlib
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
ARTIFACTS = {
    "docs/RND_TECHNICAL_DOSSIER_0_6_5.md": "technical_dossier",
    "docs/RELEASE_NOTES_0_6_5.md": "release_notes",
    "qms/release_records/REL-0.6.5.md": "release_record",
    "docs/OPEN_RISK_REGISTER_0_6_5.md": "open_risk_register",
    "qms/generated/FullBuildValidation_0_6_5.md": "full_build_validation",
    "qms/generated/CTestSummary_0_6_5.md": "ctest_summary",
    "qms/generated/StaticChecks_0_6_5.md": "static_checks",
    "qms/generated/HILBenchMetrics_0_6_5.json": "hil_bench_metrics",
    "qms/generated/HILBenchSessionReport_0_6_5.md": "hil_bench_session_report",
    "qms/generated/HILFaultInjectionReport_0_6_5.md": "hil_fault_injection_report",
    "qms/generated/HILEmergencyStopReport_0_6_5.md": "hil_emergency_stop_report",
    "qms/generated/HILPhantomDisconnectReport_0_6_5.md": "hil_phantom_disconnect_report",
    "qms/generated/HILReplayConsistencyReport_0_6_5.md": "hil_replay_consistency_report",
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
    manifest = {"release": "0.6.5", "files": files, "generatedAt": dt.datetime.now(dt.timezone.utc).isoformat(), "generator": "dnxr-qms"}
    out = ROOT / "qms/generated/SignedEvidenceManifest_0_6_5.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    return 0
if __name__ == "__main__":
    raise SystemExit(main())
