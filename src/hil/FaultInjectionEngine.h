#pragma once
#include <string>
#include <vector>
namespace dnxr {
enum class HILFaultType { SignalLoss, ExtremeNoise, CalibrationDrift, VestibularOverload, RejectedStimulus, StaleWatchdogSubsystem, LatencyOverrun, EmergencyStop, ReplayAttack, PacketCorruption, PhantomDisconnect, TimingJitter, FullDiveDesynchronization };
const char* toString(HILFaultType fault);
struct HILFaultEvent { HILFaultType type; double magnitude = 1.0; std::string description; };
class FaultInjectionEngine { public: void inject(HILFaultEvent event); std::vector<HILFaultEvent> events() const; bool hasFault(HILFaultType type) const; void clear(); private: std::vector<HILFaultEvent> events_; };
}
