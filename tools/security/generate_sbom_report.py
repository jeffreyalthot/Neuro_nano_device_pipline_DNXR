#!/usr/bin/env python3
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "sbom_generated_0.4.1.md"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    source_docs = ["docs/SBOM.md", "docs/SOUP_INVENTORY.md"]
    OUT.write_text(
        "# SBOM Generated Evidence 0.4.1\n\n"
        "## Scope\nGenerated SBOM evidence index for the 0.4.1 software/QMS evidence update.\n\n"
        "## Source documents\n"
        + "".join(f"- {doc}\n" for doc in source_docs)
        + "\n## Completion state\nTool-generated SBOM output remains required before clinical-test readiness promotion.\n\n"
        "## Reviewer / status\nReviewer role: cybersecurity/SOUP reviewer. Status: generated evidence index, named sign-off pending.\n",
        encoding="utf-8",
    )
    print(OUT)


if __name__ == "__main__":
    main()
