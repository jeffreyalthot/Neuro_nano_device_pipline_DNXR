#pragma once

namespace dnxr {

class NeuralChipEmergencyLatch {
public:
    void trigger(const char* reason);
    bool latched() const;
    const char* reason() const;
    void resetWithOperatorAuthorization();

private:
    bool latched_ = false;
    const char* reason_ = "";
};

} // namespace dnxr
