# Performance

The runtime now includes low-overhead components for measurable performance:

- `PerformanceMetrics`: min/avg/max/last latency aggregation.
- `LatencyTracker`: monotonic steady-clock measurements.
- `RuntimeProfiler`: cycle profiler wrapper.
- `FrameBudgetMonitor`: budget violation accounting.
- `PipelineBenchmark`: deterministic synthetic benchmark runner.
- `HotPathDiagnostics`: hot-path readiness summary.

Benchmark command:

```bash
./build/release/distributed_neural_xr --benchmark --headless --cycles=1
```

Expected metric fields include `samples`, `last_ms`, `avg_ms`, `min_ms`, and `max_ms`.
