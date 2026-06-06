#include "FiveSenseNeuralState.h"
#include <algorithm>
namespace dnxr { float FiveSenseNeuralState::globalRisk() const { return std::max({visualState.riskScore,auditoryState.riskScore,somatosensoryState.riskScore,thermalState.riskScore,proprioceptiveState.riskScore,vestibularState.riskScore,olfactoryState.riskScore,gustatoryState.riskScore,interoceptiveState.riskScore}); } }
