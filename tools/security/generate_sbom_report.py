#!/usr/bin/env python3
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = ROOT / "qms" / "security" / "sbom_generated_0.4.4.md"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    source_docs = ["docs/SBOM.md", "docs/SOUP_INVENTORY.md", "CMakeLists.txt"]
    OUT.write_text(
        "# SBOM Generated Evidence 0.4.4\n\n"
        "## Scope\nGenerated SBOM evidence index for the 0.4.4 software/QMS evidence update.\n\n"
        "## Source documents\n"
        + "".join(f"- {doc}\n" for doc in source_docs)
        + "\n## Completion state\nSBOM evidence is generated for software review; named SOUP/cybersecurity sign-off remains required for clinical promotion.\n\n"
        "## Reviewer / status\nReviewer role: cybersecurity/SOUP reviewer. Status: generated evidence index, named sign-off pending.\n",
        encoding="utf-8",
    )
    print(OUT)


if __name__ == "__main__":
    main()
