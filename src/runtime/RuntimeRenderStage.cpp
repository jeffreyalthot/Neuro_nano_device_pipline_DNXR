#include "RuntimeRenderStage.h"

namespace dnxr {

const char* RuntimeRenderStage::name() {
    return "render";
}

RuntimeStageStatus RuntimeRenderStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeRenderStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.renderStatus = validate(input.present && input.pushSensoryFrame, "render_stage_unavailable");
    if (!frame.renderStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.renderStatus.reason;
        return frame;
    }

    input.present(frame.sensoryFrame);
    const bool renderOk = input.finishRenderBudget ? input.finishRenderBudget() : true;
    if (!renderOk) {
        frame.renderStatus = RuntimeStageStatus::degraded("render_budget_exceeded");
        if (input.onRenderBudgetExceeded) {
            input.onRenderBudgetExceeded();
        }
        if (input.logWarn) {
            input.logWarn("⚠️ Render budget exceeded — jitter risk. task=render");
        }
    } else {
        frame.renderStatus = validate(true, "render_stage_unhealthy");
    }

    if (input.heartbeatRenderer) {
        input.heartbeatRenderer();
    }
    if (input.assessWatchdog) {
        const WatchdogAssessment watchdog = input.assessWatchdog(
            input.runtimeLatencyMs,
            !frame.rawNeuralSignals.empty(),
            frame.fusedSafetyDecision.riskScore,
            frame.diagnostics.decisionDrift);
        if (watchdog.requiresEmergencyStop) {
            if (input.triggerEmergency) {
                input.triggerEmergency();
            }
            if (input.latchEmergency) {
                input.latchEmergency("watchdog:" + watchdog.reason);
            }
            if (input.logError) {
                input.logError("[IndependentWatchdog] " + watchdog.reason);
            }
            frame.continueCycle = false;
            frame.emergencyActive = true;
            frame.stopReason = "watchdog:" + watchdog.reason;
            frame.renderStatus = RuntimeStageStatus::emergency(frame.stopReason);
            return frame;
        }
    }

    input.pushSensoryFrame(frame.sensoryFrame);
    return frame;
}

} // namespace dnxr
