#include "../src/qms/SignOffRecord.h"
#include <cassert>
int main(){ dnxr::qms::SignOffRecord s{"SO-1","EV-1","A Signer","independent reviewer","2026-06-04","accepted",true}; assert(s.acceptsResidualRisk()); assert(s.isValid()); s.signedByIndependentReviewer=false; assert(!s.isValid()); return 0; }
