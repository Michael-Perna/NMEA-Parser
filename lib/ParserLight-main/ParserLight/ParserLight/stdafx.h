// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef __APPLE__
    #include <string>
#elif __linux__
    #include <string>
#else
	#include "targetver.h"
	#include <tchar.h>
#endif

// TODO: reference additional headers your program requires here
