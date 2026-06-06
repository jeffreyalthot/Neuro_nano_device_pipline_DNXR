#include "ClosedLoopNeuralFeedbackReader.h"
#include <algorithm>
namespace dnxr { ClosedLoopFeedback ClosedLoopNeuralFeedbackReader::read(const NeuralChipFrame& r,const SafeNeuralStimulusCommand& s) const { ClosedLoopFeedback f; f.available=!r.channels.empty(); f.perceptionConfidence=std::clamp(r.signalQuality*(1.0f-s.riskScore),0.0f,1.0f); f.adjustment=std::clamp(s.aggregateSensoryLoad-f.perceptionConfidence,-0.1f,0.1f); f.safetyVerified=f.available&&f.perceptionConfidence>0.25f&&!s.boundedChannels.empty(); return f; } }
