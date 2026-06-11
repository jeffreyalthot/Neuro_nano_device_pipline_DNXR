#include "full_dive/motor_chip/MotorIntentConfidenceGate.h"
#include <cassert>
int main(){ dnxr::MotorIntentConfidenceGate g; dnxr::NeuralMotorIntentVector i; i.confidence01=0.1f; i.gaitIntent01=1.0f; dnxr::MotorIntentConfidenceDecision d; auto o=g.apply(i,1.0f,&d); assert(o.stopIntent01==1.0f && o.gaitIntent01==0.0f && d.safetyPoseRequired); i.confidence01=1.0f; i.spasmRisk01=0.5f; o=g.apply(i,1.0f,&d); assert(d.freezeAvatar && o.stopIntent01==1.0f); }
