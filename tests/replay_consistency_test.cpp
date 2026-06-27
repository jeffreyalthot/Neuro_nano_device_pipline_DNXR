#include "replay/ReplayConsistencyReport.h"
#include <cassert>
int main(){ dnxr::ReplayConsistencyReport r; r.consistent=false; r.expected="a"; r.actual="b"; assert(r.explain().find("divergent")!=std::string::npos); return 0; }
