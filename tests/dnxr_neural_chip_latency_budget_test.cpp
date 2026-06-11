#include "full_dive/sync/NeuralChipLatencyBudget.h"
#include <cassert>
int main(){ dnxr::NeuralChipLatencyBudget b; b.audioVisualSkewMs=40; b.visualVestibularSkewMs=30; auto d=dnxr::NeuralChipLatencyBudgetEvaluator{}.evaluate(b); assert(d.muteStartle); assert(d.neutralizeVestibular); assert(d.syncRisk01>0.0f); b.visualLatencyMs=200; d=dnxr::NeuralChipLatencyBudgetEvaluator{}.evaluate(b); assert(!d.allowFrame); assert(d.holdLastSafeFrame); }
