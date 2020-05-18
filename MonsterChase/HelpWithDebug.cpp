#include <crtdbg.h>

#ifdef _DEBUG

#pragma warning(disable:4074)//initializers put in compiler reserved initialization area
#pragma init_seg(compiler)//global objects in this file get constructed very early on

struct CrtBreakAllocSetter {
    CrtBreakAllocSetter() {
        // _crtBreakAlloc = 208;
    }
};

CrtBreakAllocSetter g_crtBreakAllocSetter;

#endif//_DEBUG