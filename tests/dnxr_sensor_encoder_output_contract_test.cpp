#include "full_dive/chip_pipeline/SensorEncoderOutputContract.h"
#include <cassert>
#include <cmath>
int main(){ dnxr::SensorEncoderOutputContract c; dnxr::NeuralAuditoryVector a; a.loudness01=NAN; assert(!c.validate(a).accepted); dnxr::NeuralOlfactoryVector o; o.odorPresence01=0.1f; assert(!c.validate(o,false).accepted); o.odorPresence01=0.0f; assert(c.validate(o,false).accepted); }
