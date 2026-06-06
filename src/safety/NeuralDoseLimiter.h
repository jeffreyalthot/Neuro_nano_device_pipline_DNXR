#pragma once

#include <array>
#include <string>
#include <vector>

namespace dnxr {

enum class NeuralDoseModality { Visual=0, Audio=1, Haptic=2, Vestibular=3, Cognitive=4, Emotional=5, Proprioceptive=6, DirectNeural=7 };

struct NeuralDoseLimits {
    std::array<double, 8> perSecond{{0.20,0.20,0.15,0.08,0.10,0.05,0.10,0.0}};
    std::array<double, 8> perMinute{{2.0,2.0,1.5,0.5,1.0,0.5,1.0,0.0}};
    std::array<double, 8> perSession{{10.0,10.0,8.0,2.0,5.0,2.0,5.0,0.0}};
};

struct NeuralDoseSample { NeuralDoseModality modality = NeuralDoseModality::Visual; double intensity = 0.0; double durationSeconds = 0.0; };
struct NeuralDoseDecision { bool allowed = false; std::vector<std::string> violations; };

class NeuralDoseLimiter {
public:
    explicit NeuralDoseLimiter(NeuralDoseLimits limits = {});
    NeuralDoseDecision evaluateAndAccumulate(const std::vector<NeuralDoseSample>& samples);
    const std::array<double, 8>& sessionDose() const;
private:
    NeuralDoseLimits limits_;
    std::array<double, 8> sessionDose_{{0,0,0,0,0,0,0,0}};
};

} // namespace dnxr
