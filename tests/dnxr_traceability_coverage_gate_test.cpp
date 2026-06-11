#include "qms/TraceabilityCoverageGate.h"
#include <cassert>
#include <fstream>
int main(){ const char* path="traceability_gate_test.csv"; { std::ofstream o(path); o<<"requirement,hazard,risk control,source file,test,evidence artifact,verification method,status,reviewer signoff\nREQ-1,H-1,RC-1,src/a.cpp,test-a,artifact,inspection,open,reviewer\n"; } dnxr::TraceabilityCoverageGate gate(path); auto d=gate.evaluate(); assert(d.complete); assert(d.rowsChecked==1); }
