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

// This is a file for simple GL helper classes...

#ifndef GLH_OBS_H
#define GLH_OBS_H

#ifdef _WIN32
# include <windows.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <glh/glh_extensions.h>

#include <glh/glh_linear.h>

namespace glh
{
	class display_list
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		display_list(bool managed = false) 
			: valid(false), manageObjects(managed) {}
		
		virtual ~display_list()
		{ 
            if (manageObjects)
                del();
        }
		
		void call_list()
		{ if(valid) glCallList(dlist); }
		
		void new_list(GLenum mode)
		{ if(!valid) gen(); glNewList(dlist, mode); }
		
		void end_list()
		{ glEndList(); }
		
		void del()
		{ if(valid) glDeleteLists(dlist, 1); valid = false; }
		
		bool is_valid() const { return valid; }
		
	private:
		
		void gen() { dlist = glGenLists(1); valid=true; }
		
		bool valid;
        bool manageObjects;
		GLuint dlist;
	};
	
	class lazy_build_display_list
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		lazy_build_display_list(void (* builder)() = 0, bool managed = false) 
			: valid(false), manageObjects(managed), needs_rebuild(true), build_func(builder) {}
		
		virtual ~lazy_build_display_list()
		{ 
            if (manageObjects)
                del();
        }

		void set_build_func( void (* builder)())
		{ build_func = builder; }

		void call_list()
		{ 
			if(! valid) gen();
			if(needs_rebuild) rebuild_list();
			glCallList(dlist);
		}
		
		void del()
		{ if(valid) glDeleteLists(dlist, 1); valid = false; needs_rebuild = true;}
		
		bool is_valid() const { return valid; }

		// something changed, so rebuild list before next call_list()
		void rebuild() { needs_rebuild = true; }
		
	private:
		
		void gen() { dlist = glGenLists(1); valid=true; }
		void rebuild_list()
		{
			glNewList(dlist, GL_COMPILE);
			if(build_func) (* build_func)(); // call list building routine
			glEndList();
		}
		
		bool valid;
        bool manageObjects;
		bool needs_rebuild;
		GLuint dlist;
		void (* build_func)();
	};
	
	
	class tex_object
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		tex_object(GLenum tgt, bool managed) 
			: target(tgt), valid(false), manageObjects(managed) {}
		
		virtual ~tex_object()
        { 
            if (manageObjects)
                del();
        }
		
		void bind()
		{ if(!valid) gen(); glBindTexture(target, texture); }
		
		// convenience methods

#ifdef __MACOS_DISABLED__
		void parameter(GLenum pname, int i)
		{ glTexParameteri(target, pname, i); }
#endif

		void parameter(GLenum pname, GLint i)
		{ glTexParameteri(target, pname, i); }

		void parameter(GLenum pname, GLfloat f)
		{ glTexParameterf(target, pname, f); }

		void parameter(GLenum pname, GLint * ip)
		{ glTexParameteriv(target, pname, ip); }

		void parameter(GLenum pname, GLfloat * fp)
		{ glTexParameterfv(target, pname, fp); }

		void enable() { glEnable(target); }
		void disable() { glDisable(target); }


        void del()
        { if(valid) glDeleteTextures(1, &texture); valid = false; }
        bool is_valid() const { return valid; }
		
		void gen() { glGenTextures(1, &texture); valid=true; }
		
		GLenum target;
		bool valid;
        bool manageObjects;
		GLuint texture;
	};
	
	class tex_object_1D : public tex_object
	{ public: tex_object_1D(bool managed = false) : tex_object(GL_TEXTURE_1D, managed) {} };
	
	class tex_object_2D : public tex_object
	{ public: tex_object_2D(bool managed = false) : tex_object(GL_TEXTURE_2D, managed) {} };

    class tex_object_3D : public tex_object
	{ public: tex_object_3D(bool managed = false) : tex_object(GL_TEXTURE_3D, managed) {} };


# ifdef GL_ARB_texture_cube_map
	class tex_object_cube_map : public tex_object
	{ public: tex_object_cube_map(bool managed = false) : tex_object(GL_TEXTURE_CUBE_MAP_ARB, managed) {} };
# elif GL_EXT_texture_cube_map
	class tex_object_cube_map : public tex_object
	{ public: tex_object_cube_map(bool managed = false) : tex_object(GL_TEXTURE_CUBE_MAP_EXT, managed) {} };
# endif

#if defined(GL_EXT_texture_rectangle)
	class tex_object_rectangle : public tex_object
	{ public: tex_object_rectangle(bool managed = false) : tex_object(GL_TEXTURE_RECTANGLE_EXT, managed) {} };
#elif defined(GL_NV_texture_rectangle)
	class tex_object_rectangle : public tex_object
	{ public: tex_object_rectangle(bool managed = false) : tex_object(GL_TEXTURE_RECTANGLE_NV, managed) {} };
#endif


# ifdef GL_NV_vertex_program
	class vertex_program_base
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		vertex_program_base(GLenum tgt, bool managed) 
			: valid(false), manageObjects(managed), target(tgt) {}
		
		virtual ~vertex_program_base()
        { 
            if (manageObjects)
                del();
        }
		
		void bind()
		{ if(!valid) gen(); glBindProgramNV(target, program); }
		
		void unbind()
		{ glBindProgramNV(target, 0); }
		
		void load(GLuint size, const GLubyte * prog_text)
		{
			if(!valid) gen();
			glLoadProgramNV(target, program, size, prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(GLuint size, const char * prog_text)
		{
			if(!valid) gen();
			glLoadProgramNV(target, program, size, (const GLubyte *) prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(const char * prog_text)
		{ load((GLuint)strlen(prog_text), prog_text); }


		void del()
		{ if(valid) glDeleteProgramsNV(1, &program); valid = false; }
		
		bool is_valid() const { return valid; }
		
	private:
		
		void gen() { glGenProgramsNV(1, &program); valid=true; }
		
		bool valid;
        bool manageObjects;
		GLenum target;
		GLuint program;
	};

	class vertex_program : public vertex_program_base
	{
	public:
		vertex_program(bool managed = false) 
			: vertex_program_base(GL_VERTEX_PROGRAM_NV, managed) {}
	};		

	class vertex_state_program : public vertex_program_base
	{
	public:
		vertex_state_program(bool managed = false) 
			: vertex_program_base(GL_VERTEX_STATE_PROGRAM_NV, managed) {}
	};

	class lazy_load_vertex_program : public vertex_program_base
	{
		public:
		lazy_load_vertex_program(void (*vp_loader)() = 0, bool managed = false) 
			: vertex_program_base(GL_VERTEX_PROGRAM_NV, managed), needs_load(true), loader(vp_loader) {}

		void bind()
		{
			vertex_program_base::bind();
			if(needs_load && loader)
			{
				(* loader)();
				needs_load = false;
			}
		}

		void reload() { needs_load = true; }

		private:
			bool needs_load;
			void (* loader)();
	};		


#endif

# ifdef GL_ARB_vertex_program
	class arb_vertex_program_base
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		arb_vertex_program_base(GLenum tgt, bool managed) 
			: valid(false), manageObjects(managed), target(tgt) {}
		
		virtual ~arb_vertex_program_base()
        { 
            if (manageObjects)
                del();
        }
		
		void bind()
		{ if(!valid) gen(); glBindProgramARB(target, program); }
		
		void unbind()
		{ glBindProgramARB(target, 0); }
		
		void load(GLuint size, const GLubyte * prog_text)
		{
			if(!valid) gen();
			bind();
			glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(GLuint size, const char * prog_text)
		{
			if(!valid) gen();
			bind();
			glProgramStringARB(target, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(const char * prog_text)
		{ load((GLuint)strlen(prog_text), prog_text); }


		void del() { if(valid) glDeleteProgramsARB(1, &program); valid = false; }
		
		bool is_valid() const { return valid; }
		
	private:
		
		void gen() { glGenProgramsARB(1, &program); valid=true; }
		
		bool valid;
        bool manageObjects;
		GLenum target;
		GLuint program;
	};

	class arb_vertex_program : public arb_vertex_program_base
	{
	public:
		arb_vertex_program(bool managed = false) 
			: arb_vertex_program_base(GL_VERTEX_PROGRAM_ARB, managed) {}
	};		

	class lazy_load_arb_vertex_program : public arb_vertex_program_base
	{
		public:
		lazy_load_arb_vertex_program(void (*vp_loader)() = 0, bool managed = false) 
			: arb_vertex_program_base(GL_VERTEX_PROGRAM_ARB, managed), needs_load(true), loader(vp_loader) {}

		void bind()
		{
			arb_vertex_program_base::bind();
			if(needs_load && loader)
			{
				(* loader)();
				needs_load = false;
			}
		}

		void reload() { needs_load = true; }

		private:
			bool needs_load;
			void (* loader)();
	};		

#endif

# ifdef GL_NV_fragment_program

	class fragment_program
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		fragment_program(bool managed = false) 
			: valid(false), manageObjects(managed) {}
		
		virtual ~fragment_program()
        { 
            if (manageObjects)
                del();
        }
		
		void bind()
		{ if(!valid) gen(); glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, program); }
		
		void load(GLuint size, const GLubyte * prog_text)
		{
			if(!valid) gen();
			glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, program, size, prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(GLuint size, const char * prog_text)
		{
			if(!valid) gen();
			glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, program, size, (const GLubyte *) prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;

				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(const char * prog_text)
		{ load((GLuint)strlen(prog_text), prog_text); }

		void del()
		{ if(valid) glDeleteProgramsNV(1, &program); valid = false; }
		
		bool is_valid() const { return valid; }
		
		GLuint program;
	private:
		
		void gen() { glGenProgramsNV(1, &program); valid=true; }
		
		bool valid;
        bool manageObjects;
	};


#endif

#ifdef GL_ARB_fragment_program

	class arb_fragment_program
	{
	public:
        // set managed to true if you want the class to cleanup objects in the destructor
		arb_fragment_program(bool managed = false) 
			: valid(false), manageObjects(managed) {}
		
		virtual ~arb_fragment_program()
        { 
            if (manageObjects)
                del();
        }
		
		void bind()
		{ if(!valid) gen(); glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, program); }
		
		void load(GLuint size, const GLubyte * prog_text)
		{
			if(!valid) gen();
            glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
			//glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, program, size, prog_text);

			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;
				//bgn < 0 ? 0 : bgn;
				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(GLuint size, const char * prog_text)
		{
			if(!valid) gen();
            glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
			//glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, program, size, (const GLubyte *) prog_text);
			GLint errpos;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
			if(errpos != -1)
			{
				fprintf(stderr, "program error:\n");
				int bgn = errpos - 10;

				const char * c = (const char *)(prog_text + bgn);
				for(int i = 0; i < 30; i++)
				{
					if(bgn+i >= int(size-1))
						break;
					fprintf(stderr, "%c", *c++);
				}
				fprintf(stderr, "\n");
			}
		}

		void load(const char * prog_text)
		{ load((GLuint)strlen(prog_text), prog_text); }

		void del()
		{ if(valid) glDeleteProgramsARB(1, &program); valid = false; }
		
		bool is_valid() const { return valid; }
		
		GLuint program;
	private:
		
		void gen() { glGenProgramsARB(1, &program); valid=true; }
		
		bool valid;
        bool manageObjects;
	};


#endif

}
#endif
