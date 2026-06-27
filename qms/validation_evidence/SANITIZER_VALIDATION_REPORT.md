# Sanitizer Validation Report

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence only; sanitizer execution does not establish clinical safety or human-use authorization.

## Completed lanes
| Lane | Configure/build/test evidence | Result | Notes |
| --- | --- | --- | --- |
| ASAN | `qms/generated/SANITIZER_ASAN_REPORT.md` | PASS | Full CTest lane passed after correcting test allocator delete coverage. |
| UBSAN | `qms/generated/SANITIZER_UBSAN_REPORT.md` | PASS | Full CTest lane passed with undefined-behavior sanitizer enabled. |
| TSAN | `qms/generated/SANITIZER_TSAN_REPORT.md` | PASS | Full CTest lane passed with thread sanitizer enabled. |

## Human-use boundary
Sanitizer success is software robustness evidence only. It does not unlock clinical or human-use modes without signed QMS, regulatory, ethics, medical, and device-specific evidence.
