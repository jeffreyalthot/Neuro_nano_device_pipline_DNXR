#include "HumanStopSignalMonitor.h"
namespace dnxr {
HumanStopDecision HumanStopSignalMonitor::evaluate(const HumanStopSignals& s) const { HumanStopDecision d; if(s.patientButton) d.reasons.push_back("patient_button"); if(s.operatorButton) d.reasons.push_back("operator_button"); if(s.voiceStop) d.reasons.push_back("voice_stop"); if(s.motorStop) d.reasons.push_back("motor_stop"); if(s.communicationLost) d.reasons.push_back("communication_lost"); if(s.periodicConfirmationMissed) d.reasons.push_back("periodic_confirmation_missed"); d.shutdownLatched=!d.reasons.empty(); return d; }
}
