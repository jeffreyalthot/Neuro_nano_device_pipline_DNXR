#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct ReplayFrame {
    std::vector<float> neuralVector;
    double timestamp{0.0};
};

class DeterministicSessionReplay {
public:
    void record(const ReplayFrame& frame);
    void resetCursor();
    bool hasNext() const;
    ReplayFrame next();
    std::size_t size() const;

private:
    std::vector<ReplayFrame> frames_;
    std::size_t cursor_{0};
};

} // namespace dnxr
