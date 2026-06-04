#!/usr/bin/env python3
import sys
from qms_lib import DOCS, REQUIRED_DOCS, document_section_gaps, missing_required_docs

missing = missing_required_docs()
section_gaps = []
for doc in REQUIRED_DOCS:
    path = DOCS / doc
    if path.suffix == ".md" and path.exists() and doc in {
        "CLINICAL_TEST_READINESS_PACKAGE.md",
        "CANADA_ITA_READINESS_CHECKLIST.md",
        "US_IDE_READINESS_CHECKLIST.md",
        "PRECLINICAL_SUMMARY_REPORT.md",
        "SOFTWARE_SAFETY_CASE_SUMMARY.md",
        "FULL_DIVE_EVIDENCE_SUMMARY.md",
        "HIL_EVIDENCE_SUMMARY.md",
        "CYBERSECURITY_EVIDENCE_SUMMARY.md",
    }:
        sections, too_short, placeholders = document_section_gaps(path)
        if sections or too_short or placeholders:
            section_gaps.append((doc, sections, too_short, placeholders))

if missing or section_gaps:
    if missing:
        print("Missing required docs:", ", ".join(missing))
    for doc, sections, too_short, placeholders in section_gaps:
        print(f"Document quality gap: {doc} missing_sections={sections} too_short={too_short} placeholders={placeholders}")
    sys.exit(1)
print(f"All {len(REQUIRED_DOCS)} required readiness docs present with minimum operational sections")
