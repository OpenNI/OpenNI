#ifdef _WIN32 // Microsoft Visual Studio
	#include "jconfig.vc"
#elif SN_TARGET_PS3
	#include "jconfig.ps3"
#elif ((__linux__) || (__APPLE__))
	#include "jconfig.lnx86"
#elif (_ARC)
	#include "jconfig.lnx86"
#else
	#error "LibJPEG: Unsupported Platform!"
#endif

#include <stdio.h>
