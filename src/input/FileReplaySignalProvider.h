#pragma once

#include "IInputSignalProvider.h"

#include <cstddef>
#include <string>

namespace dnxr {

class FileReplaySignalProvider final : public IInputSignalProvider {
public:
    explicit FileReplaySignalProvider(const std::string& path);

    std::vector<float> nextSignal(int cycle) override;
    std::string name() const override;
    std::size_t frameCount() const;
    bool usingFallback() const;

private:
    void useFallbackFrame(const std::string& reason);

    std::vector<std::vector<float>> frames_;
    bool usingFallback_ = false;
};

} // namespace dnxr
