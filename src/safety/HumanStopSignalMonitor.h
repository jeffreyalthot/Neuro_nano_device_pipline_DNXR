#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct HumanStopSignals { bool patientButton=false; bool operatorButton=false; bool voiceStop=false; bool motorStop=false; bool communicationLost=false; bool periodicConfirmationMissed=false; };
struct HumanStopDecision { bool shutdownLatched=false; std::vector<std::string> reasons; };
class HumanStopSignalMonitor { public: HumanStopDecision evaluate(const HumanStopSignals& signals) const; };
}
