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

// Simple array template.
// Copyright (c) Cass W. Everitt 1999 
// Copyright (c) NVIDIA 2000

#ifndef _GLH_ARRAY_H_
#define _GLH_ARRAY_H_

namespace glh
{
  //
  // Template for array2
  //

  template <class T> class array2
  {
  public:
	typedef T value_type;

	array2(int width=1, int height=1) 
    {
      w = width;
      h = height;
      data = new T [w * h];
      clear(T());
    }
	
	array2(const array2<T> & t)
    {
      w = h = 0;
      data=0;
      (*this) = t;
    }
	
	// intentionally non-virtual 
	~array2() { delete [] data; }
	
	const array2 & operator = (const array2<T> & t)
    {
      if(w != t.w || h != t.h) set_size(t.w, t.h);
	  int sz = w * h;
      for(int i = 0; i < sz; i++) data[i] = t.data[i];
      return *this;
    }
	

	void set_size(int width, int height)

	{

		if(w == width && h == height) return;

		delete [] data;

		w = width;

		h = height;

		data = new T [w * h];

	}


	T & operator () (int i, int j)
    { return data[i + j * w]; }
	
	const T & operator () (int i, int j) const
    { return data[i + j * w]; }


	int get_width() const { return w; }

	int get_height() const { return h; }
	
	void clear(const T & val) 
    {
      int sz = w * h;
      for(int i = 0; i < sz; i++) data[i] = val;
    }


	void copy(const array2<T> & src, int i_offset = 0, int j_offset = 0,
		      int width = 0, int height = 0)

	{

		int io = i_offset;

		int jo = j_offset;

		if(width == 0) width = src.get_width();

		if(height == 0) height = src.get_height();

		if(io + width > w) return;

		if(jo + height > h) return;

		for(int i=0; i < width; i++)

			for(int j=0; j < height; j++)

				(*this)(io+i, jo+j) = src(i,j);

	}



	T * get_pointer() { return data; }

	const T * get_pointer() const { return data; }
  private:
	
	int w, h;
	T * data;	
  };

	//
	// Template for array3
	//

	template <class T> class array3
	{
	public:
	typedef T value_type;

	array3(int width=1, int height=1, int depth=1) 
	{
		w = width;
		h = height;
		d = depth;
		data = new T [w * h * d];
		clear(T());
	}
	
	array3(const array3<T> & t)
	{
		w = h = d = 0;
		data=0;
		(*this) = t;
	}
	
	// intentionally non-virtual 
	~array3() { delete [] data; }
	
	const array3 & operator = (const array3<T> & t)
	{
		if(w != t.w || h != t.h || d != t.d)
			set_size(t.w, t.h, t.d);
		int sz = w * h * d;
		for(int i = 0; i < sz; i++)
			data[i] = t.data[i];
		return *this;
	}
	

	void set_size(int width, int height, int depth)

	{
		if(w == width && h == height && d == depth)
			return;

		delete [] data;

		w = width;
		h = height;
		d = depth;

		data = new T [w * h * d];
	}


	T & operator () (int i, int j, int k)
	{ return data[i + j * w + k * w * h]; }
	
	const T & operator () (int i, int j, int k) const
	{ return data[i + j * w + k * w * h]; }


	int get_width() const
	{ return w; }

	int get_height() const
	{ return h; }
	
	int get_depth() const
	{ return d; }
	
	void clear(const T & val) 
	{
		int sz = w * h * d;
		for(int i = 0; i < sz; i++)
			data[i] = val;
	}


	void copy(const array3<T> & src,
			int i_offset = 0, int j_offset = 0, int k_offset = 0,
			int width = 0, int height = 0, int depth = 0)
	{

		int io = i_offset;
		int jo = j_offset;
		int ko = k_offset;

		if(width == 0)
			width = src.get_width();
		if(height == 0)
			height = src.get_height();
		if(depth == 0)
			depth = src.get_depth();

		if(io + width > w || jo + height > h || ko + depth > d)
			return;

		for(int i=0; i < width; i++)
			for(int j=0; j < height; j++)
				for(int k=0; k < depth; k++)
					(*this)(io+i, jo+j, ko+k) = src(i,j,k);
	}



	T * get_pointer()
	{ return data; }

	const T * get_pointer() const
	{ return data; }

	private:
	int w, h, d;
	T * data;	
	}; // template <class T> class array3
} // namespace glh
#endif
