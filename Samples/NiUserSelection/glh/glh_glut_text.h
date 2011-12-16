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

#ifndef GLH_GLUT_TEXT_H
#define GLH_GLUT_TEXT_H

#include <glh/glh_obs.h>
#include <glh/glh_text.h>

#ifdef MACOS
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

namespace glh
{
	
	struct glut_stroke_roman : font
	{
		glut_stroke_roman() : initialized(false) {}

        virtual ~glut_stroke_roman() { } 
        
        struct glyph
		{
			float width;
			display_list dl;
		};
		
		
		// get font metrics
		virtual float get_ascent()
		{  return 119.05f; }
		virtual float get_descent()
		{  return  33.33f; }
		virtual float get_width(int i)
		{
			if(32 <= i && i <= 127)
			{
				if(! initialized) init();
				return glyphs[i].width;
			}
			return 0;
		}
		
		// draw
		virtual void  render(int i)
		{
			if(32 <= i && i <= 127)
			{
				if(! initialized) init();
				glyphs[i].dl.call_list();
			}			
		}

		virtual void initialize() { if(! initialized) init(); }
		
		bool initialized;
		glyph glyphs[128];
		
	private:
			
		void init()
		{
			initialized = true;
			GLfloat m[16];
			glColorMask(0,0,0,0);
			glDepthMask(0);
			glStencilMask(0);
			for(int i=32; i < 128; i++)
			{
				glPushMatrix();
				glLoadIdentity();

				// build display list
				glyphs[i].dl.new_list(GL_COMPILE_AND_EXECUTE);
				glutStrokeCharacter(GLUT_STROKE_ROMAN, i);
				glyphs[i].dl.end_list();
				// get *float* character width (glut only returns integer width)
				glGetFloatv(GL_MODELVIEW_MATRIX, m);
				glyphs[i].width = m[12];

				glPopMatrix();
			}
			glColorMask(1,1,1,1);
			glDepthMask(1);
			glStencilMask(1);
		}

	};
	
	
	struct glut_stroke_mono_roman : font
	{
		glut_stroke_mono_roman() : initialized(false) {}
		
        virtual ~glut_stroke_mono_roman() { }
        
		struct glyph
		{
			display_list dl;
		};
		
		
		// get font metrics
		virtual float get_ascent()
		{  return 119.05f; }
		virtual float get_descent()
		{  return  33.33f; }
		virtual float get_width(int i)
		{
			if(32 <= i && i <= 127)
			{
				if(! initialized) init();
				return 104.76;
			}
			return 0;
		}
		
		// draw
		virtual void  render(int i)
		{
			if(32 <= i && i <= 127)
			{
				if(! initialized) init();
				glyphs[i].dl.call_list();
			}			
		}

		virtual void initialize() { if(! initialized) init(); }
		
		bool initialized;
		glyph glyphs[128];
		
	private:
			
		void init()
		{
			initialized = true;
			for(int i=32; i < 128; i++)
			{
				// build display list
				glyphs[i].dl.new_list(GL_COMPILE);
				glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, i);
				glyphs[i].dl.end_list();
			}
		}

	};
	
	
	
}

#endif
