#!/usr/bin/env python3
import csv
import pathlib
import re

ROOT = pathlib.Path(__file__).resolve().parents[2]
DOCS = ROOT / "docs"
TRACE = DOCS / "TRACEABILITY_MATRIX.csv"
GENERATED = ROOT / "qms" / "generated"

REQUIRED_DOCS = [
    "CLINICAL_TEST_READINESS_PACKAGE.md",
    "CANADA_ITA_READINESS_CHECKLIST.md",
    "US_IDE_READINESS_CHECKLIST.md",
    "REB_IRB_READINESS_CHECKLIST.md",
    "PHYSICIAN_REVIEW_PACKAGE.md",
    "PRECLINICAL_SUMMARY_REPORT.md",
    "SOFTWARE_SAFETY_CASE_SUMMARY.md",
    "FULL_DIVE_EVIDENCE_SUMMARY.md",
    "HIL_EVIDENCE_SUMMARY.md",
    "CYBERSECURITY_EVIDENCE_SUMMARY.md",
    "FULL_DIVE_HAZARD_ANALYSIS.md",
    "TRACEABILITY_MATRIX.csv",
    "SOFTWARE_DEVELOPMENT_PLAN.md",
    "CYBERSECURITY_THREAT_MODEL.md",
]

MINIMUM_DOC_SECTIONS = [
    "purpose",
    "scope",
    "current status",
    "required evidence",
    "available evidence",
    "missing evidence",
    "linked artifacts",
    "linked tests",
    "reviewer / status",
    "next actions",
]

PLACEHOLDER_PATTERNS = [
    re.compile(r"requirement\s+\d+\s+shall\s+be\s+traceable", re.IGNORECASE),
    re.compile(r"\bTBD\b", re.IGNORECASE),
    re.compile(r"Missing:\s*none\b", re.IGNORECASE),
]

CRITICAL_DISABLED_TESTS = [
    "dnxr_runtime_controller_pipeline_10k_cycles_test",
    "dnxr_vestibular_exposure_budget_long_test",
    "dnxr_full_dive_intervention_window_long_test",
    "dnxr_runtime_controller_100k_cycles_test",
]

READINESS_LANE_COMMANDS = [
    "cmake -S . -B build_readiness -DDNXR_ENABLE_CLINICAL_READINESS_VALIDATION=ON",
    "cmake --build build_readiness -j2",
    "ctest --test-dir build_readiness --output-on-failure",
]


def traceability_rows():
    with TRACE.open(newline="") as handle:
        return list(csv.DictReader(handle))


def split_paths(value):
    return [part.strip() for part in re.split(r"[;|]", value or "") if part.strip()]


def missing_required_docs():
    return [doc for doc in REQUIRED_DOCS if not (DOCS / doc).exists()]


def document_section_gaps(path):
    text = path.read_text(encoding="utf-8")
    lowered = text.lower()
    sections = [section for section in MINIMUM_DOC_SECTIONS if section not in lowered]
    too_short = len([line for line in text.splitlines() if line.strip()]) < 35
    placeholders = [pattern.pattern for pattern in PLACEHOLDER_PATTERNS if pattern.search(text)]
    return sections, too_short, placeholders


def placeholder_traceability_rows(rows=None):
    rows = rows if rows is not None else traceability_rows()
    bad = []
    for row in rows:
        text = row.get("RequirementText", "")
        if any(pattern.search(text) for pattern in PLACEHOLDER_PATTERNS):
            bad.append(row.get("RequirementID", "<unknown>"))
    return bad


def traceability_field_gaps(rows=None):
    rows = rows if rows is not None else traceability_rows()
    required = ["RequirementID", "RequirementText", "Subsystem", "HazardID", "RiskControlID", "SourceFiles", "TestFiles", "EvidenceArtifact", "VerificationMethod", "VerificationStatus", "Reviewer", "ResidualRisk"]
    gaps = []
    for row in rows:
        missing = [field for field in required if not row.get(field, "").strip()]
        if missing:
            gaps.append((row.get("RequirementID", "<unknown>"), missing))
    return gaps


def evidence_artifact_gaps(rows=None):
    rows = rows if rows is not None else traceability_rows()
    gaps = []
    for row in rows:
        for rel in split_paths(row.get("EvidenceArtifact", "")):
            if not (ROOT / rel).exists():
                gaps.append((row.get("RequirementID", "<unknown>"), rel))
    return gaps


def test_file_gaps(rows=None):
    rows = rows if rows is not None else traceability_rows()
    gaps = []
    for row in rows:
        for rel in split_paths(row.get("TestFiles", "")):
            path = ROOT / rel
            if rel.startswith("dnxr_"):
                continue
            if not path.exists():
                gaps.append((row.get("RequirementID", "<unknown>"), rel))
    return gaps


def disabled_test_impact():
    cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    impacts = []
    for test in CRITICAL_DISABLED_TESTS:
        disabled_by_default = test in cmake and re.search(rf"if\(NOT .*?\).*?{re.escape(test)}.*?DISABLED TRUE", cmake, re.DOTALL)
        impacts.append({
            "test": test,
            "disabled_by_default": bool(disabled_by_default),
            "readiness_impact": "Blocking unless executed in the clinical-readiness validation lane or justified in the clinical gap report.",
        })
    return impacts


def high_severity_open_hazards():
    hazard = DOCS / "FULL_DIVE_HAZARD_ANALYSIS.md"
    if not hazard.exists():
        return ["FULL_DIVE_HAZARD_ANALYSIS.md missing"]
    rows = []
    for line in hazard.read_text(encoding="utf-8").splitlines():
        if line.startswith("| H-") and "High" in line and "Requires independent review" in line:
            rows.append(line.split("|")[1].strip())
    return rows
