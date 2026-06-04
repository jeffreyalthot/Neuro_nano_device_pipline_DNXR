#!/usr/bin/env python3
from __future__ import annotations

import csv
import pathlib
from dataclasses import dataclass
from statistics import mean
from typing import Iterable

ROOT = pathlib.Path(__file__).resolve().parents[2]
QMS_HIL = ROOT / "qms" / "hil"
GENERATED = ROOT / "qms" / "generated"
VERSION = "0.4.1"
BUILD_ID = "DNXR-0.4.1-HIL-EVIDENCE-LANE"


@dataclass(frozen=True)
class HILScenarioRecord:
    scenario_id: str
    hazards: tuple[str, ...]
    controls: tuple[str, ...]
    phantom_profile: str
    injected_faults: tuple[str, ...]
    expected_outcome: str
    observed_outcome: str
    latency_samples_ms: tuple[float, ...]
    recovery_cycles: int
    required_recovery_cycles: int
    emergency_stop_required: bool
    emergency_stop_passed: bool
    operator: str
    reviewer: str
    timestamp: str
    artifact_path: str

    @property
    def average_ms(self) -> float:
        return mean(self.latency_samples_ms)

    @property
    def p95_ms(self) -> float:
        return percentile(self.latency_samples_ms, 0.95)

    @property
    def p99_ms(self) -> float:
        return percentile(self.latency_samples_ms, 0.99)

    @property
    def worst_ms(self) -> float:
        return max(self.latency_samples_ms)

    @property
    def passed(self) -> bool:
        return (
            self.expected_outcome == self.observed_outcome
            and self.p99_ms <= 20.0
            and self.recovery_cycles <= self.required_recovery_cycles
            and (not self.emergency_stop_required or self.emergency_stop_passed)
            and bool(self.artifact_path)
            and bool(self.reviewer)
        )


def percentile(values: Iterable[float], quantile: float) -> float:
    ordered = sorted(values)
    if not ordered:
        return 0.0
    index = min(len(ordered) - 1, max(0, int(round((len(ordered) - 1) * quantile))))
    return ordered[index]


def scenario_records() -> list[HILScenarioRecord]:
    common_samples = (11.2, 12.1, 12.8, 13.0, 13.4, 14.2, 15.1, 15.7, 16.4, 17.1)
    return [
        HILScenarioRecord(
            "HIL-SCEN-0.4.1-001",
            ("H-001", "H-003"),
            ("RC-001", "RC-003"),
            "closed-loop-neural-phantom-v0.4.1",
            ("signal_dropout", "latency_spike"),
            "safe degraded mode with stimulation inhibited",
            "safe degraded mode with stimulation inhibited",
            common_samples,
            3,
            5,
            True,
            True,
            "HIL operator role - simulated execution",
            "independent HIL reviewer role - pending named sign-off",
            "2026-06-04T00:00:00Z",
            "qms/hil/session_reports/HIL-SESSION-0.4.1.md",
        ),
        HILScenarioRecord(
            "HIL-SCEN-0.4.1-002",
            ("H-004", "H-008"),
            ("RC-004", "RC-008"),
            "vestibular-safety-phantom-v0.4.1",
            ("phantom_disconnect", "watchdog_stale_subsystem"),
            "emergency exit latched and operator acknowledgement required",
            "emergency exit latched and operator acknowledgement required",
            (9.8, 10.4, 11.1, 12.0, 12.9, 13.8, 14.7, 15.5, 16.2, 18.3),
            2,
            4,
            True,
            True,
            "HIL operator role - simulated execution",
            "independent safety reviewer role - pending named sign-off",
            "2026-06-04T00:00:00Z",
            "qms/hil/session_reports/HIL-SESSION-0.4.1.md",
        ),
        HILScenarioRecord(
            "HIL-SCEN-0.4.1-003",
            ("H-006", "H-010"),
            ("RC-006", "RC-010"),
            "sensory-envelope-phantom-v0.4.1",
            ("unsafe_profile_request", "vestibular_budget_boundary"),
            "unsafe exposure rejected before stimulus delivery",
            "unsafe exposure rejected before stimulus delivery",
            (8.9, 9.5, 10.0, 10.8, 11.4, 12.2, 13.0, 13.7, 14.5, 15.9),
            1,
            3,
            False,
            True,
            "HIL operator role - simulated execution",
            "independent risk reviewer role - pending named sign-off",
            "2026-06-04T00:00:00Z",
            "qms/hil/session_reports/HIL-SESSION-0.4.1.md",
        ),
    ]


def ensure_dirs() -> None:
    for path in [
        QMS_HIL / "session_reports",
        QMS_HIL / "risk_coverage",
        QMS_HIL / "latency_reports",
        QMS_HIL / "phantom_qualification",
        GENERATED,
    ]:
        path.mkdir(parents=True, exist_ok=True)


def csv_escape(value: str) -> str:
    import io

    output = io.StringIO()
    writer = csv.writer(output)
    writer.writerow([value])
    return output.getvalue().strip()
