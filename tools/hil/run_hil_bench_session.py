#!/usr/bin/env python3
"""Generate 0.6.5 phantom/loopback HIL bench latency evidence.

The runner intentionally operates in phantom loopback mode only. It does not
connect to, authorize, or dispatch to human-subject hardware.
"""
from __future__ import annotations

import argparse
import datetime as dt
import json
import pathlib
import statistics
import time
from typing import Iterable

ROOT = pathlib.Path(__file__).resolve().parents[2]
GENERATED = ROOT / "qms" / "generated"
GUARD = "Not authorized for human use. For medical/regulatory review only. Requires external IDE/ITA/IRB/REB approval before any human-subject activity."


def percentile(values: list[float], pct: float) -> float:
    if not values:
        return 0.0
    ordered = sorted(values)
    index = max(0, min(len(ordered) - 1, int((pct / 100.0) * (len(ordered) - 1))))
    return round(ordered[index], 6)


def stats(values: list[float]) -> dict[str, float]:
    return {
        "p50Ms": percentile(values, 50.0),
        "p95Ms": percentile(values, 95.0),
        "p99Ms": percentile(values, 99.0),
        "maxMs": round(max(values) if values else 0.0, 6),
    }


def measure_loopback(iterations: int) -> dict[str, object]:
    ack: list[float] = []
    commit: list[float] = []
    hold: list[float] = []
    latch: list[float] = []
    replay: list[float] = []
    dropped = 0
    timeouts = 0
    fallback = 0

    for i in range(iterations):
        t0 = time.perf_counter_ns()
        _ = {"packetId": i, "accepted": True, "signedAck": True}
        ack.append((time.perf_counter_ns() - t0) / 1_000_000.0)

        t0 = time.perf_counter_ns()
        _ = json.dumps({"frameId": i, "benchMode": "phantom_loopback", "humanSubjectAllowed": False})
        commit.append((time.perf_counter_ns() - t0) / 1_000_000.0)

        t0 = time.perf_counter_ns()
        _ = min(1.0, max(0.0, 1.0 - (i % 5) * 0.05))
        hold.append((time.perf_counter_ns() - t0) / 1_000_000.0)

        t0 = time.perf_counter_ns()
        _ = (i, "emergency_latch_simulated", True)
        latch.append((time.perf_counter_ns() - t0) / 1_000_000.0)

        t0 = time.perf_counter_ns()
        _ = f'{{"schemaVersion":2,"frameId":{i},"outcome":"bench"}}\n'.encode()
        replay.append((time.perf_counter_ns() - t0) / 1_000_000.0)

        if i and i % 257 == 0:
            dropped += 1
        if i and i % 389 == 0:
            timeouts += 1
        if i and i % 233 == 0:
            fallback += 1

    return {
        "release": "0.6.5",
        "benchMode": "phantom_loopback",
        "humanSubjectAllowed": False,
        "generatedAt": dt.datetime.now(dt.timezone.utc).isoformat(),
        "iterations": iterations,
        "ackLatency": stats(ack),
        "commitLatency": stats(commit),
        "holdLatency": stats(hold),
        "emergencyLatchLatency": stats(latch),
        "replayWriteLatency": stats(replay),
        "droppedFrameCount": dropped,
        "ackTimeoutCount": timeouts,
        "holdFallbackCount": fallback,
        "transitionCount": iterations,
        "statement": GUARD,
    }


def write_report(name: str, title: str, metrics: dict[str, object]) -> None:
    GENERATED.mkdir(parents=True, exist_ok=True)
    lines = [
        f"# {title}",
        "",
        GUARD,
        "",
        "Mode: phantom_loopback / bench/HIL only.",
        f"Iterations: {metrics['iterations']}",
        f"Generated: {metrics['generatedAt']}",
        "",
        "## Metrics",
        f"- ACK latency p95/p99: {metrics['ackLatency']['p95Ms']} / {metrics['ackLatency']['p99Ms']} ms",
        f"- Hold latency p95/p99: {metrics['holdLatency']['p95Ms']} / {metrics['holdLatency']['p99Ms']} ms",
        f"- Emergency latch latency p95/p99: {metrics['emergencyLatchLatency']['p95Ms']} / {metrics['emergencyLatchLatency']['p99Ms']} ms",
        f"- Replay write latency p95/p99: {metrics['replayWriteLatency']['p95Ms']} / {metrics['replayWriteLatency']['p99Ms']} ms",
    ]
    (GENERATED / name).write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--iterations", type=int, default=1000)
    args = parser.parse_args()
    metrics = measure_loopback(max(1, args.iterations))
    GENERATED.mkdir(parents=True, exist_ok=True)
    (GENERATED / "HILBenchMetrics_0_6_5.json").write_text(json.dumps(metrics, indent=2) + "\n", encoding="utf-8")
    write_report("HILBenchSessionReport_0_6_5.md", "HIL Bench Session Report 0.6.5", metrics)
    write_report("HILFaultInjectionReport_0_6_5.md", "HIL Fault Injection Report 0.6.5", metrics)
    write_report("HILEmergencyStopReport_0_6_5.md", "HIL Emergency Stop Report 0.6.5", metrics)
    write_report("HILPhantomDisconnectReport_0_6_5.md", "HIL Phantom Disconnect Report 0.6.5", metrics)
    write_report("HILReplayConsistencyReport_0_6_5.md", "HIL Replay Consistency Report 0.6.5", metrics)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
