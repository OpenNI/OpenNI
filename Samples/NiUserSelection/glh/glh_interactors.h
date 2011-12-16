/*
    glh - is a platform-indepenedent C++ OpenGL helper library 


    Copyright (c) 2000 Cass Everitt
	Copyright (c) 2001 NVIDIA Corporation
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

#ifndef GLH_INTERACTORS_H
#define GLH_INTERACTORS_H

#include <math.h>

namespace glh
{

  class translator
  {
  public:
	translator()
	{
	  scale = .01f;
	  invert_increment = false;
	  parent_rotation = 0;
	}

	void pan  (int dx, int dy) { update(dx, dy,  0); }
	void dolly(int dz)         { update( 0,  0, dz); }

	void update(int dx, int dy, int dz)
	{
	  vec3f v(dx, dy, dz);

	  // apply parent rotation
	  if(parent_rotation != 0)
		  parent_rotation->mult_vec(v);

	  if(invert_increment)
		  t -= v * scale;
	  else
		  t += v * scale;
	}


	matrix4f get_transform()
	{
		matrix4f m;
		m.set_translate(t);
		return m;
	}	

	matrix4f get_inverse_transform()
	{
		matrix4f m;
		m.set_translate(-t);
		return m;
	}	

	bool invert_increment;
	const rotationf * parent_rotation;
	vec3f t;
	float scale;
  };



  class trackball
  {
  public:
	trackball()
	{
		r = rotationf(vec3f(0, 1, 0), 0);
		centroid = vec3f(0,0,0);
		scale = -.01f;
		invert_increment = false;
		parent_rotation = 0;
		legacy_mode = false;
	}

	void rotate(int x0, int y0, int x1, int y1)
	{ update(x0, y0, x1, y1); }

	void update(int x0, int y0, int x1, int y1)
	{
		int dx = x1 - x0;
		int dy = y1 - y0;
        if(dx == 0 && dy == 0)
		{
			incr = rotationf();
			return;
		}
		
		if(legacy_mode)
		{
			vec3f v(dy, -dx, 0);
			float len = v.normalize();
			if(parent_rotation != 0)
				parent_rotation->mult_vec(v);
			//r.mult_dir(vec3f(v), v);
			if(invert_increment)
				incr.set_value(v, -len * scale);
			else
				incr.set_value(v, len * scale);
		}
		else
		{
			vec3f a(x0, y0, 0);
			vec3f b(x1, y1, 0);
			a -= offset;
			b -= offset;
			a /= radius;
			b /= radius;

			float tmpscale = 1;
			a[2] = pow(2.0f, -0.5f * a.length());
			a.normalize();
			b[2] = pow(2.0f, -0.5f * b.length());
			b.normalize();



			vec3f axis = a.cross(b);
			axis.normalize();

			float angle = acos(a.dot(b));

			if(parent_rotation != 0) parent_rotation->mult_vec(axis);

			if(invert_increment)
				incr.set_value(axis, -angle * tmpscale);
			else
				incr.set_value(axis, angle * tmpscale);
				
		}

		// fixme: shouldn't operator*() preserve 'r' in this case? 
		if(incr[3] != 0)
			r = incr * r;
	}

	void increment_rotation()
	{
		// fixme: shouldn't operator*() preserve 'r' in this case? 
		if(incr[3] != 0)
			r = incr * r;
	}

	matrix4f get_transform()
	{
		matrix4f mt, mr, minvt;
		mt.set_translate(centroid);
		r.get_value(mr);
		minvt.set_translate(-centroid);
		return mt * mr * minvt;
	}

	matrix4f get_inverse_transform()
	{
		matrix4f mt, mr, minvt;
		mt.set_translate(centroid);
		r.inverse().get_value(mr);
		minvt.set_translate(-centroid);
		return mt * mr * minvt;
	}

	bool invert_increment;
	const rotationf * parent_rotation;
	rotationf r;
	vec3f centroid;
	float scale;
	bool legacy_mode;
	rotationf incr;
	float radius;
	vec3f offset;
  }; 

}

#endif
