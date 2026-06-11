#include "full_dive/chip_pipeline/SensorEncoderOutputContract.h"
#include <cassert>
int main(){ dnxr::SensorEncoderOutputContract c; dnxr::NeuralVisualVector v; v.depthFar01=2.0f; assert(!c.validate(v).accepted); dnxr::NeuralAuditoryVector a; a.envelopeSustain01=-0.1f; assert(!c.validate(a).accepted); dnxr::NeuralOlfactoryVector o; o.trigeminalContaminationRisk01=0.1f; assert(!c.validate(o,false).accepted); dnxr::NeuralGustatoryVector g; g.mouthfeel01=0.1f; assert(!c.validate(g,false).accepted); }
