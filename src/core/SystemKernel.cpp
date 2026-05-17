#include "SystemKernel.h"
#include "RuntimeController.h"
#include "../utils/Logger.h"

#include <thread>
#include <iomanip>
#include <sstream>

namespace dnxr {

SystemKernel::SystemKernel()
    : runtimeRunning_(false),
      gracefulShutdown_(true),
      cycleDurationMs_(11),
      maxCyclesPerSecond_(90),
      cycleCounter_(0) {}

SystemKernel::~SystemKernel() {
    stop();
    controller_.reset();
}

void SystemKernel::boot() {
    Logger::info("⚙️  [Kernel] Boot sequence initiated...");

    if (controller_) {
        Logger::warn("RuntimeController already active. Reinitializing...");
        controller_.reset();
    }

    controller_ = std::make_unique<RuntimeController>();
    runtimeRunning_ = true;
    cycleCounter_ = 0;

    sendStatus("kernel_boot_complete");
    Logger::info("✅ [Kernel] Boot successful.");
}

void SystemKernel::run() {
    if (!runtimeRunning_) {
        Logger::warn("[Kernel] Cannot run: system not booted.");
        return;
    }

    Logger::info("🚀 [Kernel] Runtime loop started.");
    using clock = std::chrono::high_resolution_clock;

    while (runtimeRunning_) {
        auto start = clock::now();

        runCycle();
        ++cycleCounter_;

        throttle(start);

        // Monitoring cycle performance
        if (cycleCounter_ % (maxCyclesPerSecond_ * 2) == 0) {
            std::ostringstream oss;
            oss << "[Kernel] Cycle " << cycleCounter_ << " running at "
                << maxCyclesPerSecond_ << " Hz target.";
            sendStatus(oss.str());
        }

        if (gracefulShutdown_ && shouldAutoStop()) {
            Logger::warn("[Kernel] Auto-stop condition reached.");
            stop();
        }
    }

    sendStatus("runtime_terminated");
    Logger::info("🛑 [Kernel] Runtime loop ended.");
}

void SystemKernel::runCycle() {
    if (!controller_) {
        Logger::error("[Kernel] RuntimeController missing.");
        return;
    }

    try {
        controller_->runSingleCycle(static_cast<double>(cycleCounter_));
    } catch (const std::exception& e) {
        Logger::error(std::string("[Kernel] Exception during cycle: ") + e.what());
        stop();
    } catch (...) {
        Logger::error("[Kernel] Unknown fatal error.");
        stop();
    }
}

void SystemKernel::stop() noexcept {
    if (!runtimeRunning_) return;

    runtimeRunning_ = false;
    sendStatus("system_stopped");

    Logger::info("[Kernel] Shutdown requested, waiting for loop termination.");
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    Logger::info("[Kernel] Shutdown complete.");
}

void SystemKernel::enableGracefulShutdown(bool enabled) {
    gracefulShutdown_ = enabled;
}

void SystemKernel::setCycleDuration(int ms) {
    cycleDurationMs_ = std::max(1, ms);
}

int SystemKernel::getCycleDuration() const {
    return cycleDurationMs_;
}

void SystemKernel::setMaxCyclesPerSecond(int cps) {
    maxCyclesPerSecond_ = std::max(1, cps);
}

int SystemKernel::getMaxCyclesPerSecond() const {
    return maxCyclesPerSecond_;
}

void SystemKernel::throttle(const std::chrono::high_resolution_clock::time_point& start) {
    using namespace std::chrono;

    auto elapsed = duration_cast<milliseconds>(
        high_resolution_clock::now() - start);

    auto target = milliseconds(cycleDurationMs_);

    if (elapsed < target) {
        std::this_thread::sleep_for(target - elapsed);
    }
}

bool SystemKernel::shouldAutoStop() const {
    // Permet d’éviter les dépassements sur les sessions longues
    return cycleCounter_ > static_cast<uint64_t>(maxCyclesPerSecond_) * 600; // ~10min
}

void SystemKernel::setStatusCallback(
    const std::function<void(const std::string&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    statusCallback_ = cb;
}

void SystemKernel::sendStatus(const std::string& msg) const {
    std::lock_guard<std::mutex> lock(mtx_);

    if (statusCallback_) {
        statusCallback_(msg);
    }

    Logger::info("[Status] " + msg);
}

} // namespace dnxr
