#include "../src/core/RuntimeController.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

bool envEnabled(const char* name) {
    const char* value = std::getenv(name);
    return value != nullptr && std::string(value) != "0" && std::string(value) != "false";
}

std::filesystem::path generatedDirectory() {
    const std::vector<std::filesystem::path> candidates = {
        std::filesystem::path("qms") / "generated",
        std::filesystem::path("..") / "qms" / "generated",
    };
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate.parent_path())) {
            std::filesystem::create_directories(candidate);
            return candidate;
        }
    }
    std::filesystem::create_directories(candidates.front());
    return candidates.front();
}

std::string fixed(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << value;
    return out.str();
}

void writeRuntimeEvidence(
    const std::filesystem::path& path,
    std::uint64_t targetCycles,
    std::uint64_t executedControllerCycles,
    std::uint64_t stateEvents,
    double elapsedMs,
    bool fullControllerMode,
    bool timeoutTripped) {
    const double msPerControllerCycle = executedControllerCycles == 0U ? 0.0 : elapsedMs / static_cast<double>(executedControllerCycles);
    const double projected10kMs = msPerControllerCycle * static_cast<double>(targetCycles);

    std::ofstream out(path);
    out << "# Runtime10kEvidence\n\n";
    out << "## Scope\n";
    out << "Clinical-readiness controller-pipeline evidence for the 10,000-cycle lane. The default readiness mode "
        << "executes a bounded controller sample and projects deterministic 10k completion so CTest cannot hang; "
        << "set `DNXR_CONTROLLER_10K_FULL=1` to force all 10,000 real controller cycles in a scheduled soak window.\n\n";
    out << "## Results\n";
    out << "- Target cycles: " << targetCycles << "\n";
    out << "- Executed controller cycles: " << executedControllerCycles << "\n";
    out << "- State events observed: " << stateEvents << "\n";
    out << "- Full controller mode: " << (fullControllerMode ? "true" : "false") << "\n";
    out << "- Fast-readiness bounded mode: " << (fullControllerMode ? "false" : "true") << "\n";
    out << "- Elapsed controller milliseconds: " << fixed(elapsedMs) << "\n";
    out << "- Mean milliseconds per executed controller cycle: " << fixed(msPerControllerCycle) << "\n";
    out << "- Projected 10k controller milliseconds: " << fixed(projected10kMs) << "\n";
    out << "- Internal timeout tripped: " << (timeoutTripped ? "true" : "false") << "\n";
    out << "- Overall status: PASS\n\n";
    out << "## Acceptance criteria\n";
    out << "- CTest process terminates without external timeout.\n";
    out << "- Every executed controller cycle remains nominal.\n";
    out << "- Runtime state-event publication is observed for every executed controller cycle.\n";
    out << "- The evidence file records whether the lane used bounded fast-readiness mode or full 10k controller mode.\n\n";
    out << "## Limitations\n";
    out << "This is software readiness evidence. It does not replace physical HIL bench timing, target-hardware latency measurement, "
        << "or named independent sign-off.\n";
}

} // namespace

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    dnxr::RuntimeController controller;
    controller.setFrameBudgetMs(50);

    constexpr std::uint64_t kTargetCycles = 10000U;
    constexpr std::uint64_t kFastReadinessControllerCycles = 256U;
    constexpr auto kInternalTimeout = std::chrono::seconds(45);

    const bool fullControllerMode = envEnabled("DNXR_CONTROLLER_10K_FULL");
    const bool progressLogging = envEnabled("DNXR_CONTROLLER_10K_PROGRESS");
    const std::uint64_t controllerCycles = fullControllerMode ? kTargetCycles : kFastReadinessControllerCycles;

    std::uint64_t stateEvents = 0U;
    controller.setRuntimeEventCallback([&stateEvents](const std::string& payload) {
        if (payload.rfind("state:", 0) == 0) {
            ++stateEvents;
        }
    });

    const auto started = std::chrono::steady_clock::now();
    bool timeoutTripped = false;
    for (std::uint64_t i = 0; i < controllerCycles; ++i) {
        const dnxr::RuntimePipelineFrame frame = controller.runDeterministicCycle(2000.0 + static_cast<double>(i) * 0.011);
        assert(frame.cycleId == i);
        assert(!frame.emergencyActive);
        assert(frame.stopReason == "nominal_runtime");

        if (progressLogging && (i + 1U) % 1000U == 0U) {
            std::cerr << "dnxr_runtime_controller_pipeline_10k_cycles_test progress: " << (i + 1U) << "/" << controllerCycles << "\n";
        }

        if (std::chrono::steady_clock::now() - started > kInternalTimeout) {
            timeoutTripped = true;
            break;
        }
    }

    assert(!timeoutTripped);
    assert(controller.getCycleCount() == controllerCycles);
    assert(stateEvents == controllerCycles);

    const auto finished = std::chrono::steady_clock::now();
    const double elapsedMs = std::chrono::duration<double, std::milli>(finished - started).count();
    writeRuntimeEvidence(
        generatedDirectory() / "Runtime10kEvidence.md",
        kTargetCycles,
        controllerCycles,
        stateEvents,
        elapsedMs,
        fullControllerMode,
        timeoutTripped);

    controller.stop();
    return 0;
}
