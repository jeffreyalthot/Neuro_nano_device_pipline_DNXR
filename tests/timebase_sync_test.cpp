#include "utils/TimeSync.h"
#include <cassert>
int main(){ auto a=dnxr::TimeSync::nowMs(); auto b=dnxr::TimeSync::nowMs(); assert(b>=a); return 0; }
