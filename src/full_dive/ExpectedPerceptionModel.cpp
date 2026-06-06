#include "ExpectedPerceptionModel.h"
namespace dnxr { std::vector<ExpectedPerception> ExpectedPerceptionModel::predict(const FullDiveNeuralFrame& frame) const { std::vector<ExpectedPerception> out; for(const auto& c: frame.allChannels()) out.push_back({c.encodingModelId+":"+c.targetRegion, c.intensity, 0.10f + c.latencyBudgetMs/1000.0f}); return out; } }
