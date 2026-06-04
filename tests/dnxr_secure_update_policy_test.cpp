#include "../src/security/SecureUpdatePolicy.h"
#include <cassert>
int main(){ dnxr::SecureUpdatePolicy p; assert(p.acceptsUpdate("abcdefghijklmnop",2,1)); assert(!p.acceptsUpdate("short",2,1)); assert(!p.acceptsUpdate("abcdefghijklmnop",1,1)); return 0; }
