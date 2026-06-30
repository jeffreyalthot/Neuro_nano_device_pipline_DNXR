#!/usr/bin/env python3
"""Generate due-diligence source integrity manifests and lightweight SBOMs.

The generator is intentionally dependency-free so reviewers can reproduce the
NDA-controlled evidence package from a clean checkout without network access.
It excludes VCS/build/cache outputs and marks its own manifest outputs as
self-referential, because a file cannot contain a stable hash of itself.
"""
from __future__ import annotations

import csv
import hashlib
import json
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ACQ = ROOT / "docs" / "acquisition"
SEC = ROOT / "qms" / "security"

SELF_REFERENTIAL = {
    "docs/acquisition/SOURCE_CODE_DISCLOSURE_MANIFEST.csv",
    "docs/acquisition/SHA256_MASTER_MANIFEST.txt",
    "qms/security/SBOM.spdx.json",
    "qms/security/SBOM.cyclonedx.json",
    "docs/acquisition/REPOSITORY_INVENTORY.md",
    "qms/security/LICENSE_COMPLIANCE_MATRIX.csv",
}
EXCLUDED_DIRS = {
    ".git",
    ".idea",
    ".vscode",
    "build",
    "build_asan",
    "build_tsan",
    "build_ubsan",
    "cmake-build-debug",
    "cmake-build-release",
    "__pycache__",
}
EXCLUDED_SUFFIXES = {".o", ".a", ".so", ".dll", ".dylib", ".exe", ".pyc"}

SOURCE_EXTS = {".c", ".cc", ".cpp", ".h", ".hh", ".hpp"}
CONFIG_EXTS = {".cmake", ".json", ".yaml", ".yml", ".toml", ".txt", ".csv"}


@dataclass(frozen=True)
class FileRecord:
    path: str
    sha256: str
    size_bytes: int
    extension: str
    module: str
    provenance_class: str
    confidentiality_level: str
    status: str
    notes: str


def should_include(path: Path) -> bool:
    rel_parts = path.relative_to(ROOT).parts
    if any(part in EXCLUDED_DIRS for part in rel_parts):
        return False
    if path.suffix.lower() in EXCLUDED_SUFFIXES:
        return False
    return path.is_file()


def classify(path: str, suffix: str) -> str:
    if suffix in SOURCE_EXTS:
        return "ORIGINAL_SOURCE"
    if path.startswith("qms/generated/") or path.startswith("qms/due_diligence/"):
        return "GENERATED_QMS_ARTIFACT"
    if path.startswith("qms/security/"):
        return "GENERATED_QMS_ARTIFACT"
    if suffix == ".md":
        return "ORIGINAL_DOCUMENTATION"
    if path.startswith("assets/"):
        return "THIRD_PARTY_ASSET" if "placeholder" not in path else "PLACEHOLDER_ASSET"
    if suffix in CONFIG_EXTS:
        return "CONFIG_SAMPLE"
    return "UNKNOWN_REQUIRES_REVIEW"


def confidentiality(module: str) -> str:
    if module in {"src", "tests", "qms", "clinical_evidence", "clinical_sessions"}:
        return "Level 3"
    if module in {"docs"}:
        return "Level 2"
    return "Level 2"


def collect_records() -> list[FileRecord]:
    records: list[FileRecord] = []
    for path in sorted(ROOT.rglob("*")):
        if not should_include(path):
            continue
        rel = path.relative_to(ROOT).as_posix()
        if rel in SELF_REFERENTIAL:
            continue
        data = path.read_bytes()
        suffix = path.suffix.lower() or "(none)"
        module = rel.split("/", 1)[0]
        records.append(
            FileRecord(
                path=rel,
                sha256=hashlib.sha256(data).hexdigest(),
                size_bytes=len(data),
                extension=suffix,
                module=module,
                provenance_class=classify(rel, suffix),
                confidentiality_level=confidentiality(module),
                status="present",
                notes="none",
            )
        )
    for rel in sorted(SELF_REFERENTIAL):
        records.append(
            FileRecord(
                path=rel,
                sha256="SELF_REFERENTIAL_GENERATED_MANIFEST",
                size_bytes=0,
                extension=Path(rel).suffix,
                module=rel.split("/", 1)[0],
                provenance_class="GENERATED_QMS_ARTIFACT",
                confidentiality_level="Level 3",
                status="generated",
                notes="Hash omitted because manifest is self-referential",
            )
        )
    return records


def write_source_manifest(records: list[FileRecord]) -> None:
    ACQ.mkdir(parents=True, exist_ok=True)
    fields = [
        "path",
        "sha256",
        "size_bytes",
        "extension",
        "module",
        "provenance_class",
        "confidentiality_level",
        "status",
        "notes",
    ]
    with (ACQ / "SOURCE_CODE_DISCLOSURE_MANIFEST.csv").open("w", newline="", encoding="utf-8") as fh:
        writer = csv.DictWriter(fh, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        for record in records:
            writer.writerow(record.__dict__)
    with (ACQ / "SHA256_MASTER_MANIFEST.txt").open("w", encoding="utf-8") as fh:
        for record in records:
            fh.write(f"{record.sha256}  {record.path}\n")


def write_repository_inventory(records: list[FileRecord]) -> None:
    modules: dict[str, int] = {}
    provenance: dict[str, int] = {}
    for record in records:
        modules[record.module] = modules.get(record.module, 0) + 1
        provenance[record.provenance_class] = provenance.get(record.provenance_class, 0) + 1
    module_rows = "\n".join(f"| {name} | {count} |" for name, count in sorted(modules.items()))
    provenance_rows = "\n".join(f"| {name} | {count} |" for name, count in sorted(provenance.items()))
    (ACQ / "REPOSITORY_INVENTORY.md").write_text(
        f"""# Repository Inventory

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Status: Due diligence scaffold for preclinical R&D software evidence.
> Boundary: Preclinical R&D software evidence package; simulation/HIL readiness scaffold; not authorized for human use; not clinically validated; not a certified medical device.

- Inventory generated on 2026-06-27.
- Files indexed: {len(records)}.
- Source disclosure manifest: `docs/acquisition/SOURCE_CODE_DISCLOSURE_MANIFEST.csv`.
- Master SHA-256 manifest: `docs/acquisition/SHA256_MASTER_MANIFEST.txt`.
- The disclosure, master manifest, and SBOM entries are marked self-referential because stable hashes cannot include their own final contents.

## Module counts
| Module | File count |
| --- | ---: |
{module_rows}

## Provenance counts
| Provenance class | File count |
| --- | ---: |
{provenance_rows}
""",
        encoding="utf-8",
    )


def write_sboms(records: list[FileRecord]) -> None:
    """Write compact, module-level SPDX and CycloneDX SBOMs.

    File-level hashes live in SOURCE_CODE_DISCLOSURE_MANIFEST.csv and the
    SHA-256 master manifest. The SBOM stays compact by representing repository
    modules as components and linking back to those manifests for file hashes.
    """
    SEC.mkdir(parents=True, exist_ok=True)
    created = datetime(2026, 6, 27, tzinfo=timezone.utc).isoformat().replace("+00:00", "Z")
    modules: dict[str, dict[str, object]] = {}
    for record in records:
        if record.sha256.startswith("SELF_REFERENTIAL"):
            continue
        bucket = modules.setdefault(
            record.module,
            {"count": 0, "provenance": set(), "confidentiality": set(), "hash_seed": hashlib.sha256()},
        )
        bucket["count"] = int(bucket["count"]) + 1
        bucket["provenance"].add(record.provenance_class)  # type: ignore[union-attr]
        bucket["confidentiality"].add(record.confidentiality_level)  # type: ignore[union-attr]
        bucket["hash_seed"].update(record.path.encode("utf-8"))  # type: ignore[union-attr]
        bucket["hash_seed"].update(record.sha256.encode("utf-8"))  # type: ignore[union-attr]

    packages = []
    components = []
    for idx, (module, bucket) in enumerate(sorted(modules.items()), start=1):
        digest = bucket["hash_seed"].hexdigest()  # type: ignore[union-attr]
        provenance = sorted(bucket["provenance"])  # type: ignore[arg-type]
        confidentiality = sorted(bucket["confidentiality"])  # type: ignore[arg-type]
        packages.append(
            {
                "name": module,
                "SPDXID": f"SPDXRef-Module-{idx}",
                "downloadLocation": "NOASSERTION",
                "filesAnalyzed": False,
                "licenseConcluded": "NOASSERTION",
                "licenseDeclared": "NOASSERTION",
                "copyrightText": "NOASSERTION",
                "checksums": [{"algorithm": "SHA256", "checksumValue": digest}],
                "summary": f"Repository module with {bucket['count']} files; file-level hashes are in docs/acquisition/SHA256_MASTER_MANIFEST.txt",
                "primaryPackagePurpose": "SOURCE" if module == "src" else "DOCUMENTATION",
            }
        )
        components.append(
            {
                "type": "library" if module == "src" else "file",
                "name": module,
                "version": "0.6.6",
                "hashes": [{"alg": "SHA-256", "content": digest}],
                "properties": [
                    {"name": "dnxr:file_count", "value": str(bucket["count"])},
                    {"name": "dnxr:provenance_classes", "value": ";".join(provenance)},
                    {"name": "dnxr:confidentiality_levels", "value": ";".join(confidentiality)},
                    {"name": "dnxr:file_hash_manifest", "value": "docs/acquisition/SHA256_MASTER_MANIFEST.txt"},
                ],
            }
        )
    (SEC / "SBOM.spdx.json").write_text(
        json.dumps(
            {
                "spdxVersion": "SPDX-2.3",
                "dataLicense": "CC0-1.0",
                "SPDXID": "SPDXRef-DOCUMENT",
                "name": "Neuro_nano_device_pipline due diligence module SBOM",
                "documentNamespace": "https://example.invalid/dnxr/sbom/0.6.6/due-diligence",
                "creationInfo": {"created": created, "creators": ["Tool: tools/qms/generate_due_diligence_manifests.py"]},
                "documentComment": "Compact module-level SBOM. File-level SHA-256 values are in docs/acquisition/SHA256_MASTER_MANIFEST.txt and SOURCE_CODE_DISCLOSURE_MANIFEST.csv.",
                "packages": packages,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    (SEC / "SBOM.cyclonedx.json").write_text(
        json.dumps(
            {
                "bomFormat": "CycloneDX",
                "specVersion": "1.5",
                "serialNumber": "urn:uuid:00000000-0000-0000-0000-dnxr00000606",
                "version": 1,
                "metadata": {
                    "timestamp": created,
                    "tools": [{"vendor": "DNXR QMS", "name": "generate_due_diligence_manifests.py"}],
                    "component": {"type": "application", "name": "Neuro_nano_device_pipline", "version": "0.6.6"},
                    "properties": [{"name": "dnxr:file_hash_manifest", "value": "docs/acquisition/SHA256_MASTER_MANIFEST.txt"}],
                },
                "components": components,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )

def write_license_matrix(records: list[FileRecord]) -> None:
    with (SEC / "LICENSE_COMPLIANCE_MATRIX.csv").open("w", newline="", encoding="utf-8") as fh:
        writer = csv.writer(fh, lineterminator="\n")
        writer.writerow(["component", "path", "license", "source", "review_status", "notes"])
        writer.writerow(["repository", "LICENCE", "Proprietary/owner declaration", "local", "needs_counsel_confirmation", "Owner declaration present; counsel review required before transfer"])
        for record in records:
            if record.provenance_class in {"THIRD_PARTY_SOURCE", "THIRD_PARTY_ASSET", "UNKNOWN_REQUIRES_REVIEW"}:
                writer.writerow([record.path, record.path, "NOASSERTION", "repository", "requires_review", record.provenance_class])


def main() -> None:
    records = collect_records()
    write_source_manifest(records)
    write_repository_inventory(records)
    write_sboms(records)
    write_license_matrix(records)
    print(f"Generated due-diligence manifests for {len(records)} files")


if __name__ == "__main__":
    main()
