/*
    glh - is a platform-indepenedent C++ OpenGL helper library 


    Copyright (c) 2000 Cass Everitt
	Copyright (c) 2000, 2001 NVIDIA Corporation
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

/*
   gluBuild2DMipmaps cannot build mipmaps for textures whose
   "format" it does not recognize.  This is primarily because
   it infers the number of components and how to average them
   from the format.  This helper eliminates that problem by
   factoring out that functionality.
   
   Cass Everitt
   1-9-01

*/

#ifndef GLH_MIPMAPS_H
#define GLH_MIPMAPS_H

#ifdef _WIN32
#  include <windows.h>
#endif

#ifdef MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

namespace glh
{
	
	template <class T> 
	class tex_indexer2
	{
		public:
			tex_indexer2(int width, int height, int tuple_size, T * data) :
			  w(width), h(height), n(tuple_size), d(data) {}

			T * operator()(int i, int j)
			{ return d + n * (w * j + i); }
		private:
			int w, h, n;
			T * d;
	};

	template <class T>
	struct generic_filter
	{
		typedef T element_type;

		generic_filter(int tuplesize, GLenum gltype) :
			gl_type(gltype), tuple_size(tuplesize) {}

		void average( T * out,
					  const T * a, const T * b, 
					  const T * c, const T * d)
		{
			for(int i=0; i < tuple_size; i++)
			{
				double in = double(a[i]) + double(b[i]) + double(c[i]) + double(d[i]);
				in /= 4;
				out[i] = T(in);
			}
		}

		const GLenum gl_type;
		const int tuple_size;
	};

	// fixme: supports non-square textures!
	template <class F>
	void build_2D_mipmaps( GLenum target, GLenum internal_format,
						   GLsizei w, GLsizei h, GLenum format, 
						   F filter, const void * vdata)
	{

		typedef typename F::element_type DataType;
		const DataType * in_data = (const DataType *)vdata;
		DataType * data = new DataType [w * h * filter.tuple_size];

		glTexImage2D(target, 0, internal_format, w, h, 0, format, filter.gl_type, (const DataType *)vdata);

		int level = 1;
		if( w >= 2 ) w /= 2;
		if( h >= 2 ) h /= 2;
		bool done = false;
		while(! done)
		{
			tex_indexer2<const DataType> bg(w*2, h*2, filter.tuple_size, in_data);
			tex_indexer2<DataType> sm(w  , h  , filter.tuple_size, data);
			for(int j=0; j < h; j++)
			{
				int J = j * 2;
				for(int i=0; i < w; i++)
				{
					int I = i*2;
					filter.average( sm(i,j),
									bg(I  , J  ), bg(I+1, J  ),
									bg(I  , J+1), bg(I+1, J+1));
				}
			}
			
			glTexImage2D(target, level, internal_format, w, h, 0, format, filter.gl_type, data);

			if(w == 1 && h == 1) done = true;
			
			if( w >= 2 ) w /= 2;
			if( h >= 2 ) h /= 2;
			level++;
			in_data = data;
		}

		delete [] data;
	}

}

#endif
