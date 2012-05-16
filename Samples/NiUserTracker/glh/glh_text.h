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

#ifndef GLH_TEXT_H
#define GLH_TEXT_H

#include <string>

#ifdef _WIN32
# include <windows.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <glh/glh_obs.h>

namespace glh
{

	struct font
	{
		// build display lists and the like
		virtual void initialize() = 0;
		// get font metrics
		virtual float get_ascent() = 0;
		virtual float get_descent() = 0;
		virtual float get_width(int i) = 0;

		// draw
		virtual void  render(int i) = 0;
	};

	inline float string_width(font * f, string text)
	{
		float w = 0;
		for(unsigned int i=0; i < text.size(); i++)
		{
			w += f->get_width(text[i]);
		}
		return w;
	}
	// skip a line
	inline void next_line(font * f)
	{
		glTranslatef(0, - (f->get_ascent() + f->get_descent()), 0);
	}

	// renders text horizontally
	inline void render_single_line(font * f, string text)
	{
		glPushMatrix();
		for(unsigned int i=0; i < text.size(); i++)
		{
			f->render(text[i]);
		}
		glPopMatrix();
	}


    // render text on multiple lines (based on newlines)
	struct simple_multi_line_text
	{
	public:
		simple_multi_line_text() : f(0), line_spacing(1.1f) {}
		

		void render()
		{
			if(!f) return;
			if(dirty)
			{
				dirty = false;
				dl.new_list(GL_COMPILE);
				make_render_calls();
				dl.end_list();
			}
			dl.call_list();
		}
		void get_dimensions(float & width, float & height)
		{
			if(!f) return;
			if(dirty)
			{
				dirty = false;
				dl.new_list(GL_COMPILE);
				make_render_calls();
				dl.end_list();
			}
			width = w;
			height = h;
		}
		void set_font(font * new_font)
		{
			dirty = true;
			f = new_font;
		}
		void set_text(const string & new_text)
		{
			dirty = true;
			text = new_text;
		}
		font * get_font() { return f; }
		const string & get_text() { return text; }
	private:
		// skip a line
		void next_line()
		{
			glTranslatef(0, - (f->get_ascent() + f->get_descent()) * line_spacing, 0);
		}

		void make_render_calls()
		{
			w = h = 0;
			float curr_width = 0;
			float font_height = f->get_ascent() + f->get_descent();
			int lines = 0;

			if(text.size() != 0) lines++;

			glPushMatrix(); // vertical translation
			glPushMatrix(); // horizontal translation
			for(unsigned int i=0; i < text.size(); i++)
			{
				if(text[i] == '\n')
				{
					lines++;
					if(curr_width > w) w = curr_width;
					curr_width = 0;

					glPopMatrix(); // back to beginning of line
					next_line();  // translate futher down
					glPushMatrix();
				}
				f->render(text[i]);
				curr_width += f->get_width(text[i]);
			}
			glPopMatrix();
			glPopMatrix();
			if(curr_width > w) w = curr_width;
			h = lines * font_height * line_spacing;
		}

		font * f;
		string text;
		display_list dl;
		float w, h;
		bool dirty;
		float line_spacing;
	};


}

#endif
