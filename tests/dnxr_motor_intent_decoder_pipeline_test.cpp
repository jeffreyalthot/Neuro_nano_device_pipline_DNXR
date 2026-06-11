#include "full_dive/motor_chip/MotorCortexReadFrameAdapter.h"
#include "full_dive/motor_chip/MotorIntentDecoder.h"
#include <cassert>
int main(){ dnxr::ChipReadFrame read; read.motorIntent.moveForward01=0.6f; read.motorIntent.stopIntent01=0.0f; read.signalQuality01=0.8f; auto cortex=dnxr::MotorCortexReadFrameAdapter::fromChipReadFrame(read); auto motor=dnxr::MotorIntentDecoder{}.decode(cortex); assert(motor.moveForward01==0.6f); assert(motor.confidence01==0.8f); }
