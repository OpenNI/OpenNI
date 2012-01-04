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

// some helper classes for making
// and using cube maps

#ifndef GLH_CUBE_MAP_H
#define GLH_CUBE_MAP_H

#ifdef MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <glh/glh_obs.h>
#include <glh/glh_convenience.h>
#include <glh/glh_linear.h>

namespace glh
{

# ifdef GL_ARB_texture_cube_map
#   define GLH_CUBE_MAP_POSITIVE_X GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
#   define GLH_CUBE_MAP_POSITIVE_Y GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
#   define GLH_CUBE_MAP_POSITIVE_Z GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
#   define GLH_CUBE_MAP_NEGATIVE_X GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
#   define GLH_CUBE_MAP_NEGATIVE_Y GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
#   define GLH_CUBE_MAP_NEGATIVE_Z GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
# elif GL_EXT_texture_cube_map
#   define GLH_CUBE_MAP_POSITIVE_X GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT
#   define GLH_CUBE_MAP_POSITIVE_Y GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT
#   define GLH_CUBE_MAP_POSITIVE_Z GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT
#   define GLH_CUBE_MAP_NEGATIVE_X GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT
#   define GLH_CUBE_MAP_NEGATIVE_Y GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT
#   define GLH_CUBE_MAP_NEGATIVE_Z GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
# endif

# if GL_EXT_texture_cube_map || GL_ARB_texture_cube_map
	// make a cube map from a functor
	template <class FunctionOfDirection>
		void make_cube_map(FunctionOfDirection & f, GLenum internal_format,
		int size, int level = 0)
	{
		typedef typename FunctionOfDirection::Type Type;
		int components = f.components;
		GLenum type = f.type;
		GLenum format = f.format;
		Type * image  = new Type[size*size*components];
		Type * ip;

		float offset = .5;
		float delta = 1;
		float halfsize = size/2.f;
		vec3f v;

		// positive x image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[2] = -(i*delta + offset - halfsize);
					v[1] = -(j*delta + offset - halfsize);
					v[0] = halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_POSITIVE_X,
						 level, internal_format, size, size, 0, format, type, image);
		}
		// negative x image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[2] = (i*delta + offset - halfsize);
					v[1] = -(j*delta + offset - halfsize);
					v[0] = -halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_NEGATIVE_X,
						 level, internal_format, size, size, 0, format, type, image);
		}

		// positive y image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[0] = (i*delta + offset - halfsize);
					v[2] = (j*delta + offset - halfsize);
					v[1] = halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_POSITIVE_Y,
						 level, internal_format, size, size, 0, format, type, image);
		}
		// negative y image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[0] = (i*delta + offset - halfsize);
					v[2] = -(j*delta + offset - halfsize);
					v[1] = -halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_NEGATIVE_Y,
						 level, internal_format, size, size, 0, format, type, image);
		}

		// positive z image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[0] = (i*delta + offset - halfsize);
					v[1] = -(j*delta + offset - halfsize);
					v[2] = halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_POSITIVE_Z,
						 level, internal_format, size, size, 0, format, type, image);
		}
		// negative z image	
		{
			ip = image;
			for(int j = 0; j < size; j++)
			{
				for(int i=0; i < size; i++)
				{
					v[0] = -(i*delta + offset - halfsize);
					v[1] = -(j*delta + offset - halfsize);
					v[2] = -halfsize;
					v.normalize();
					f(v, ip);
					ip += components;
				}
			}
			glTexImage2D(GLH_CUBE_MAP_NEGATIVE_Z,
						 level, internal_format, size, size, 0, format, type, image);
		}
		delete [] image;
	}
# endif
	
	struct normalize_vector
	{
		typedef GLfloat Type;
		int components;
		GLenum type;
		GLenum format;
		normalize_vector() : components(3), type(GL_FLOAT), format(GL_RGB) {}
		
		void operator() (const vec3f & v, Type * t)
		{
			vec3f v2 = v;
			v2 *= .5;
			v2 += .5;
			t[0] = v2[0];
			t[1] = v2[1];
			t[2] = v2[2];
		}
	};

	struct cube_map_unextended
	{
		cube_map_unextended() :
			POSITIVE_X(0), NEGATIVE_X(1),
			POSITIVE_Y(2), NEGATIVE_Y(3),
			POSITIVE_Z(4), NEGATIVE_Z(5),
			lightnum(GL_LIGHT0), angle(90.0)
			{}

		// angle is > 90 degrees because we need a border to cull with
		void set_angle(GLint width)
		{ angle = to_degrees((float)atan2(width/2.0f, width/2.0f-1.0f)) * 2.0f; }


		void cull_for_projection(int i)
		{
			GLfloat plane[6][4] = 
			{
				{ 1, 0, 0, 0 },
				{-1, 0, 0, 0 },
				{ 0, 1, 0, 0 },
				{ 0,-1, 0, 0 },
				{ 0, 0, 1, 0 },
				{ 0, 0,-1, 0 }
			};
			//glClipPlane(GL_CLIP_PLANE0, plane[i]);
			glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90);
			glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, plane[i]);
		}

		void cull_for_reflection(int i)
		{
			GLfloat dir[6][4] = 
			{
				{ 1, 0, 0, 0 },
				{-1, 0, 0, 0 },
				{ 0, 1, 0, 0 },
				{ 0,-1, 0, 0 },
				{ 0, 0, 1, 0 },
				{ 0, 0,-1, 0 }
			};
			glLightfv(GL_LIGHT0, GL_POSITION, dir[i]);
		}
	
		matrix4f get_matrix(int cubeface)
		{
			matrix4f m;
			m = perspective((float)angle, 1.0f, 0.5f, 1.5f);
			switch(cubeface)
			{
			case 0:
				m *= camera_lookat(vec3f(0,0,0), vec3f( 1, 0, 0), vec3f(0,-1, 0));
				return m;
			case 1:
				m *= camera_lookat(vec3f(0,0,0), vec3f(-1, 0, 0), vec3f(0,-1, 0));
				return m;
			case 2:
				m *= camera_lookat(vec3f(0,0,0), vec3f( 0, 1, 0), vec3f(0, 0, 1));
				return m;
			case 3:
				m *= camera_lookat(vec3f(0,0,0), vec3f( 0,-1, 0), vec3f(0, 0,-1));
				return m;
			case 4:
				m *= camera_lookat(vec3f(0,0,0), vec3f( 0, 0, 1), vec3f(0,-1, 0));
				return m;
			case 5:
				m *= camera_lookat(vec3f(0,0,0), vec3f( 0, 0,-1), vec3f(0,-1, 0));
				return m;
			default:
				return matrix4f();
			}
		}

		matrix4f get_matrix_inverse(int cubeface)
		{
			matrix4f m;
			switch(cubeface)
			{
			case 0:
				m = object_lookat(vec3f(0,0,0), vec3f( 1, 0, 0), vec3f(0,-1, 0));
				break;
			case 1:
				m = object_lookat(vec3f(0,0,0), vec3f(-1, 0, 0), vec3f(0,-1, 0));
				break;
			case 2:
				m = object_lookat(vec3f(0,0,0), vec3f( 0, 1, 0), vec3f(0, 0, 1));
				break;
			case 3:
				m = object_lookat(vec3f(0,0,0), vec3f( 0,-1, 0), vec3f(0, 0,-1));
				break;
			case 4:
				m = object_lookat(vec3f(0,0,0), vec3f( 0, 0, 1), vec3f(0,-1, 0));
				break;
			case 5:
				m = object_lookat(vec3f(0,0,0), vec3f( 0, 0,-1), vec3f(0,-1, 0));
				break;
			default:
				break;
			}
			return m * perspective_inverse((float)angle, 1.0f, 0.5f, 1.5f);
		}
		const int POSITIVE_X;
		const int NEGATIVE_X;
		const int POSITIVE_Y;
		const int NEGATIVE_Y;
		const int POSITIVE_Z;
		const int NEGATIVE_Z;

		GLenum lightnum;
		tex_object_2D face[6];
		double angle;

		matrix4f rotation;

	};

}  // namespace glh
#endif

