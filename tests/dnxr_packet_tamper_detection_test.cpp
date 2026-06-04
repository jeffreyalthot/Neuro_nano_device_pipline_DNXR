#include "../src/security/ThreatEvent.h"
#include <cassert>
int main(){ dnxr::ThreatEvent e{"T-TAMPER","packet",dnxr::ThreatSeverity::High,9,true,false}; assert(e.tamperDetected); return 0; }
