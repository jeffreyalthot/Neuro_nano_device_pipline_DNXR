#include "../src/qms/ReviewRecord.h"
#include <cassert>
int main(){ dnxr::qms::ReviewRecord r; assert(!r.isComplete()); r.id="REV-1"; r.artifactId="EV-1"; r.reviewer="A Reviewer"; r.role="independent safety reviewer"; r.date="2026-06-04"; r.accepted=true; assert(r.isComplete()); return 0; }
