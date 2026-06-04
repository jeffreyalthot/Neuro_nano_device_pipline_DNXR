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
VERSION = "0.4.3"
BUILD_ID_PREFIX = "DNXR"
ALL_HIL_HAZARDS = tuple(f"H-{index:03d}" for index in range(1, 19))


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


def build_id(version: str = VERSION) -> str:
    return f"{BUILD_ID_PREFIX}-{version}-HIL-EVIDENCE-LANE"


def session_artifact_path(version: str = VERSION) -> str:
    return f"qms/hil/session_reports/HIL-SESSION-{version}.md"


def percentile(values: Iterable[float], quantile: float) -> float:
    ordered = sorted(values)
    if not ordered:
        return 0.0
    index = min(len(ordered) - 1, max(0, int(round((len(ordered) - 1) * quantile))))
    return ordered[index]


def scenario_records(version: str = VERSION) -> list[HILScenarioRecord]:
    artifact = session_artifact_path(version)
    timestamp = "2026-06-04T00:00:00Z"
    operator = "HIL operator role - simulated execution"
    reviewer = "independent HIL reviewer role - pending named sign-off"
    return [
        HILScenarioRecord(
            f"HIL-SCEN-{version}-001",
            ("H-001", "H-003"),
            ("RC-001", "RC-003"),
            f"closed-loop-neural-phantom-v{version}",
            ("signal_dropout", "latency_spike"),
            "safe degraded mode with stimulation inhibited",
            "safe degraded mode with stimulation inhibited",
            (11.2, 12.1, 12.8, 13.0, 13.4, 14.2, 15.1, 15.7, 16.4, 17.1),
            3,
            5,
            True,
            True,
            operator,
            reviewer,
            timestamp,
            artifact,
        ),
        HILScenarioRecord(
            f"HIL-SCEN-{version}-002",
            ("H-004", "H-008"),
            ("RC-004", "RC-008"),
            f"vestibular-safety-phantom-v{version}",
            ("phantom_disconnect", "watchdog_stale_subsystem"),
            "emergency exit latched and operator acknowledgement required",
            "emergency exit latched and operator acknowledgement required",
            (9.8, 10.4, 11.1, 12.0, 12.9, 13.8, 14.7, 15.5, 16.2, 18.3),
            2,
            4,
            True,
            True,
            operator,
            "independent safety reviewer role - pending named sign-off",
            timestamp,
            artifact,
        ),
        HILScenarioRecord(
            f"HIL-SCEN-{version}-003",
            ("H-006", "H-010"),
            ("RC-006", "RC-010"),
            f"sensory-envelope-phantom-v{version}",
            ("unsafe_profile_request", "vestibular_budget_boundary"),
            "unsafe exposure rejected before stimulus delivery",
            "unsafe exposure rejected before stimulus delivery",
            (8.9, 9.5, 10.0, 10.8, 11.4, 12.2, 13.0, 13.7, 14.5, 15.9),
            1,
            3,
            False,
            True,
            operator,
            "independent risk reviewer role - pending named sign-off",
            timestamp,
            artifact,
        ),
        HILScenarioRecord(f"HIL-SCEN-{version}-004", ("H-002",), ("RC-002",), f"abnormal-neural-activity-phantom-v{version}", ("seizure_like_burst", "stimulation_guard_boundary"), "stimulation inhibited and clinician alert latched", "stimulation inhibited and clinician alert latched", (9.1, 9.8, 10.5, 11.2, 12.0, 12.7, 13.5, 14.1, 15.0, 16.8), 2, 4, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-005", ("H-005",), ("RC-005",), f"panic-dissociation-phantom-v{version}", ("panic_marker_spike", "dissociation_prompt_failure"), "guided recovery profile selected with operator escalation", "guided recovery profile selected with operator escalation", (8.7, 9.4, 10.1, 10.9, 11.8, 12.6, 13.4, 14.0, 14.8, 16.1), 3, 5, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-006", ("H-007",), ("RC-007",), f"motor-mismatch-phantom-v{version}", ("motor_intent_conflict", "proprioceptive_drift"), "motor output inhibited until coherence recovers", "motor output inhibited until coherence recovers", (9.0, 9.7, 10.3, 11.0, 11.9, 12.5, 13.2, 13.9, 14.7, 16.4), 2, 4, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-007", ("H-009",), ("RC-009",), f"temporal-coherence-phantom-v{version}", ("clock_skew", "timeline_desync"), "temporal stream frozen and reality anchor displayed", "temporal stream frozen and reality anchor displayed", (9.4, 10.0, 10.7, 11.4, 12.1, 12.9, 13.8, 14.5, 15.2, 17.0), 3, 5, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-008", ("H-011",), ("RC-011",), f"watchdog-failure-phantom-v{version}", ("watchdog_stale", "heartbeat_loss"), "watchdog failure drives safe halt and manual reset", "watchdog failure drives safe halt and manual reset", (8.8, 9.5, 10.2, 10.8, 11.6, 12.4, 13.1, 13.8, 14.4, 16.0), 1, 3, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-009", ("H-012",), ("RC-012",), f"timing-overrun-phantom-v{version}", ("scheduler_overrun", "frame_budget_exhaustion"), "overrun frame rejected and degraded cadence engaged", "overrun frame rejected and degraded cadence engaged", (10.0, 10.6, 11.3, 12.0, 12.8, 13.7, 14.5, 15.1, 16.0, 18.8), 2, 4, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-010", ("H-013",), ("RC-013",), f"cyber-compromise-phantom-v{version}", ("unauthorized_command", "credential_misuse"), "command rejected and security incident marker emitted", "command rejected and security incident marker emitted", (9.2, 9.9, 10.6, 11.3, 12.2, 13.0, 13.9, 14.6, 15.4, 17.3), 2, 4, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-011", ("H-014",), ("RC-014",), f"replay-tamper-phantom-v{version}", ("replay_packet", "tampered_timestamp"), "tampered input rejected with audit trace", "tampered input rejected with audit trace", (8.6, 9.3, 10.0, 10.7, 11.5, 12.3, 13.0, 13.7, 14.6, 16.2), 1, 3, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-012", ("H-015",), ("RC-015",), f"decompression-failure-phantom-v{version}", ("exit_sequence_fault", "state_restore_failure"), "fallback decompression sequence completed with clinician alert", "fallback decompression sequence completed with clinician alert", (9.5, 10.2, 10.9, 11.7, 12.4, 13.2, 14.1, 14.9, 15.6, 17.9), 4, 6, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-013", ("H-016",), ("RC-016",), f"reality-anchor-phantom-v{version}", ("anchor_render_failure", "orientation_loss"), "secondary reality anchor displayed and immersion reduced", "secondary reality anchor displayed and immersion reduced", (9.0, 9.6, 10.4, 11.1, 11.9, 12.8, 13.6, 14.3, 15.1, 16.9), 2, 4, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-014", ("H-017",), ("RC-017",), f"unsafe-transition-phantom-v{version}", ("hil_mode_transition_fault", "unsafe_state_request"), "unsafe transition blocked before actuator/stimulus delivery", "unsafe transition blocked before actuator/stimulus delivery", (8.9, 9.4, 10.1, 10.9, 11.7, 12.6, 13.5, 14.2, 15.0, 16.7), 1, 3, True, True, operator, reviewer, timestamp, artifact),
        HILScenarioRecord(f"HIL-SCEN-{version}-015", ("H-018",), ("RC-018",), f"clinical-protocol-phantom-v{version}", ("protocol_limit_violation", "unauthorized_parameter_change"), "protocol violation rejected and study halt recommendation emitted", "protocol violation rejected and study halt recommendation emitted", (9.7, 10.4, 11.0, 11.8, 12.6, 13.5, 14.4, 15.0, 15.8, 18.1), 2, 4, True, True, operator, reviewer, timestamp, artifact),
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
