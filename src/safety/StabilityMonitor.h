// StabilityMonitor.h

#pragma once

#include <vector>

namespace dnxr {

class StabilityMonitor {
public:
    StabilityMonitor();

    /// Validation cardio-respiratoire avec garde-fous sécurité.
    bool checkCardioRespiratoryWindow(const std::vector<float>& telemetry) const;

    /// Ajuste les bornes de sécurité.
    void setBounds(float bpmMin, float bpmMax,
                   float rpmMin, float rpmMax);

    float getLastBpm() const;
    float getLastRpm() const;

    void setStrictMode(bool enabled);
    bool isStrictMode() const;

    void setAnomalyTolerance(float t);
    float getAnomalyTolerance() const;

private:
    bool strictMode_;
    float anomalyTolerance_;

    mutable float lastBpm_;
    mutable float lastRpm_;
};

} // namespace dnxr