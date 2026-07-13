#include "RuntimeRenderStage.h"

#include "RuntimeStageServices.h"
#include "ObserverMirrorSink.h"
#include "../safety/EmergencyShutdown.h"
#include "../safety/IndependentWatchdog.h"

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

    frame.renderStatus = validate(static_cast<bool>(input.prepareNeuralSensory), "neural_sensory_preparation_unavailable");
    if (!frame.renderStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.renderStatus.reason;
        return frame;
    }

    if (!input.prepareNeuralSensory(frame.sensoryFrame)) {
        frame.renderStatus = RuntimeStageStatus::emergency("neural_sensory_preparation_failed");
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = frame.renderStatus.reason;
        return frame;
    }
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

    if (input.heartbeatObserver) {
        input.heartbeatObserver();
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

    if (input.publishObserverMirror) {
        const std::vector<float> immutableCopy = frame.sensoryFrame;
        input.publishObserverMirror(immutableCopy);
    }
    return frame;
}

RuntimePipelineFrame RuntimeRenderStage::execute(
    RuntimePipelineFrame frame,
    RuntimeStageServices& services,
    SchedulerTickReport& tickReport) {
    if (!frame.continueCycle) {
        return frame;
    }

    services.watchdog.heartbeat("observer_mirror");

    const WatchdogAssessment watchdog = services.watchdog.assess(
        tickReport.totalMs,
        !frame.rawNeuralSignals.empty(),
        frame.fusedSafetyDecision.riskScore,
        frame.diagnostics.decisionDrift);
    if (watchdog.requiresEmergencyStop) {
        services.emergency.trigger();
        services.safetyStateMachine.latchEmergency("watchdog:" + watchdog.reason);
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "watchdog:" + watchdog.reason;
        frame.renderStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    services.observerMirror.publishImmutableCopy(frame.sensoryFrame);
    frame.renderStatus = validate(true, "neural_sensory_prepared");
    return frame;
}

} // namespace dnxr
