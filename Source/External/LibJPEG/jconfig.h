#ifdef _WIN32 // Microsoft Visual Studio
	#include "jconfig.vc"
	#include <stdio.h>
#elif SN_TARGET_PS3
	#include "jconfig.ps3"
	#include <stdio.h>
#elif (linux)
	#include "jconfig.lnx86"
	#include <stdio.h>
#else
	#error "LibJPEG: Unsupported Platform!"
#endif
