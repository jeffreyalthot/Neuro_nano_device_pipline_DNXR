#include "../src/security/ThreatEvent.h"
#include <cassert>
int main(){ dnxr::ThreatEvent e{"T-REPLAY","network",dnxr::ThreatSeverity::High,42,false,true}; assert(e.replayDetected); assert(e.counter==42); return 0; }
