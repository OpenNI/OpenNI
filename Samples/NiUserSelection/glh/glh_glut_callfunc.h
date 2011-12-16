/*
glh - is a platform-indepenedent C++ OpenGL helper library 

  
    Copyright (c) 2000 Cass Everitt
	Copyright (c) 2000 NVIDIA Corporation
    All rights reserved.
	
    Redistribution and use in source and binary forms, with or
    without modification, are permitted provided that the following
    conditions are met:
	  
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the following
	  disclaimer.
		
    * Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the following
	  disclaimer in the documentation and/or other materials
	  provided with the distribution.
		  
	* The names of contributors to this software may not be used
	  to endorse or promote products derived from this software
	  without specific prior written permission. 
			
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
	POSSIBILITY OF SUCH DAMAGE. 
			  
				
	Cass Everitt - cass@r3.nu
*/

#ifndef GLH_GLUT_CALLFUNC_H
#define GLH_GLUT_CALLFUNC_H

#include <string>

#ifdef _WIN32
#define GLH_FUNC extern "C" __declspec(dllexport) 
#else
#define GLH_FUNC
#endif


namespace glh
{

	struct library_handle
	{
#ifdef _WIN32
		bool init(const char * name)
		{
			lib = GetModuleHandle(name);
			if(lib) return true;
			std::string n(name);
			n += ".exe";
			lib = GetModuleHandle(n.c_str());

			return lib != 0;
		}
		void call_func(const char k, int x, int y)
		{
			std::string entry_name("key__");
			entry_name += k;
			void (*entry)(int, int) = (void (*)(int, int))GetProcAddress(lib, entry_name.c_str());
			if(entry)
				(*entry)(x, y);
		}
		HMODULE lib;
#endif
	};
	
}

#endif
