#include "../src/network/ReplayProtectionWindow.h"
#include <cassert>
int main(){ dnxr::ReplayProtectionWindow w; assert(w.accept(7)); assert(!w.accept(7)); return 0; }
