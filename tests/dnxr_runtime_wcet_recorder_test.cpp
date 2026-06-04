#include "../src/runtime/RuntimeWCETRecorder.h"
#include <cassert>
int main(){ dnxr::RuntimeWCETRecorder r; r.observe(3); r.observe(7); assert(r.worstCaseMs()==7); assert(r.within(8)); assert(!r.within(6)); return 0; }
