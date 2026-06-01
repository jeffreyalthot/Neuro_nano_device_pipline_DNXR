#include "ReplayProtectionWindow.h"

namespace dnxr {

ReplayProtectionWindow::ReplayProtectionWindow(std::size_t windowSize)
    : windowSize_(windowSize == 0U ? 1U : windowSize) {}

bool ReplayProtectionWindow::accept(std::uint64_t counter) {
    if (counter == 0U) {
        return false;
    }

    if (hasSeen(counter)) {
        return false;
    }

    if (highestAcceptedCounter_ > windowSize_ && counter + windowSize_ <= highestAcceptedCounter_) {
        return false;
    }

    seenCounters_.insert(counter);
    if (counter > highestAcceptedCounter_) {
        highestAcceptedCounter_ = counter;
    }
    prune();
    return true;
}

bool ReplayProtectionWindow::hasSeen(std::uint64_t counter) const {
    return seenCounters_.find(counter) != seenCounters_.end();
}

void ReplayProtectionWindow::reset() {
    highestAcceptedCounter_ = 0U;
    seenCounters_.clear();
}

std::uint64_t ReplayProtectionWindow::highestAcceptedCounter() const {
    return highestAcceptedCounter_;
}

void ReplayProtectionWindow::prune() {
    if (highestAcceptedCounter_ <= windowSize_) {
        return;
    }

    const std::uint64_t minimumAllowed = highestAcceptedCounter_ - static_cast<std::uint64_t>(windowSize_) + 1U;
    for (auto it = seenCounters_.begin(); it != seenCounters_.end();) {
        if (*it < minimumAllowed) {
            it = seenCounters_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace dnxr
