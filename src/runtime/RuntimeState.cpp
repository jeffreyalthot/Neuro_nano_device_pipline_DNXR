#include "RuntimeState.h"

#include <stdexcept>

namespace dnxr {

RuntimeState::RuntimeState(std::size_t historyLimit)
    : historyLimit_(historyLimit == 0U ? 1U : historyLimit) {}

void RuntimeState::commitFrame(const RuntimeFrame& frame) {
    history_.push_back(frame);
    if (history_.size() > historyLimit_) {
        history_.erase(history_.begin());
    }
}

bool RuntimeState::hasFrame() const {
    return !history_.empty();
}

const RuntimeFrame& RuntimeState::currentFrame() const {
    if (history_.empty()) {
        throw std::runtime_error("RuntimeState has no committed frame");
    }
    return history_.back();
}

const std::vector<RuntimeFrame>& RuntimeState::history() const {
    return history_;
}

std::size_t RuntimeState::historyLimit() const {
    return historyLimit_;
}

void RuntimeState::clear() {
    history_.clear();
}

} // namespace dnxr
