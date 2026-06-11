#include "neural_chip/ReleaseModeLock.h"
#include <cassert>
int main(){ dnxr::ReleaseModeLock lock; dnxr::ReleaseModeLockConfig requested; requested.humanUse=true; requested.allowStimulusDispatch=true; requested.activeCeiling01=2.0F; auto applied=lock.apply(requested); assert(lock.isCompliant(applied)); assert(!applied.humanUse); assert(!applied.allowStimulusDispatch); assert(applied.benchOnly); assert(applied.activeCeiling01<=0.80F); }
