#include "StimulusLoadSimulator.h"
namespace dnxr { bool StimulusLoadSimulator::withinLoad(double requestedLoad, double maxLoad) const { return requestedLoad >= 0.0 && requestedLoad <= maxLoad; } }
