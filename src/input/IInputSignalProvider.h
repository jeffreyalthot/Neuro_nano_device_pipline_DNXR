#pragma once

#include <string>
#include <vector>

namespace dnxr {

class IInputSignalProvider {
public:
    virtual ~IInputSignalProvider() = default;
    virtual std::vector<float> nextSignal(int cycle) = 0;
    virtual std::string name() const = 0;
};

} // namespace dnxr
