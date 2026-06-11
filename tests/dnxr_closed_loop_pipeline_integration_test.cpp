#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>

int main(){
    dnxr::NeuralChipBus bus; dnxr::FullDiveChipFrame seed; seed.frameId=1; seed.timestampMs=1; seed.visual.motionFlow01=0.8f; seed.visual.flickerRisk01=0.7f; seed.touch.painBlocked01=seed.touch.burnBlocked01=seed.touch.crushBlocked01=seed.touch.shockBlocked01=1.0f; assert(bus.submitSensoryFrame(seed));
    dnxr::NeuralChipOnlyPipelineConfig cfg=dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults(); cfg.enableResearchVision=true; cfg.capabilities.canVisual=true; cfg.patientMap.visual.enabled=true; cfg.patientMap.visual.confidence01=1.0f;
    dnxr::NeuralChipOnlyFullDivePipeline p(bus,cfg); dnxr::NeuralChipOnlyWorldFrame w; w.patientMap=cfg.patientMap; w.frameId=2; w.timestampMs=2; w.scene.luminance01=0.5f; assert(p.step(w));
    dnxr::ChipReadFrame bad; bad.frameId=2; bad.signalQuality01=0.0f; bus.injectSimulatedMotorReadFrame(bad); w.frameId=3; w.timestampMs=3; w.scene.motionFlow01=1.0f; bool accepted=p.step(w); assert(!accepted || p.sessionState().closedLoopStability01<=1.0f);
}
