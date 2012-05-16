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

#ifndef GLH_GLUT_REPLAY_H
#define GLH_GLUT_REPLAY_H

// a facility for recording and playing back glut events

#include <list>
#include <glh/glh_glut.h>

namespace glh
{


  struct glut_event
  {
	  enum event_type
	  {
		  DISPLAY, IDLE, KEYBOARD, MENU_STATUS, MOTION, MOUSE,
		  PASSIVE_MOTION, RESHAPE, SPECIAL, TIMER, VISIBILITY
	  };
	  glut_event(event_type t) : type(t) {}
	  virtual ~glut_event() {}
	  virtual void dispatch() = 0;
	  const event_type type;
  };

  struct glut_display_event : public glut_event
  {
	  glut_display_event() : glut_event(DISPLAY) {}
	  virtual void dispatch() { glut_display_function(); }
  };

  struct glut_idle_event : public glut_event
  {
	  glut_idle_event() : glut_event(IDLE) {}
	  virtual void dispatch() { glut_idle_function(); }
  };

  struct glut_keyboard_event : public glut_event
  {
	  glut_keyboard_event(unsigned char key, int xpos, int ypos)
		  :	glut_event(KEYBOARD), k(key), x(xpos), y(ypos)
	  {}
	  virtual void dispatch() { glut_keyboard_function(k,x,y); }
	  unsigned char k;
	  int x, y;
  };

  struct glut_menu_status_event : public glut_event
  {
	  glut_menu_status_event(int status, int xpos, int ypos)
		  :	glut_event(MENU_STATUS), s(status), x(xpos), y(ypos)
	  {}
	  virtual void dispatch() { glut_menu_status_function(s,x,y); }
	  int s, x, y;
  };
  
  struct glut_motion_event : public glut_event
  {
	  glut_motion_event(int xpos, int ypos)
		  :	glut_event(MOTION), x(xpos), y(ypos)
	  {}
	  virtual void dispatch() { glut_motion_function(x,y); }
	  int x, y;
  };

  struct glut_mouse_event : public glut_event
  {
	  glut_mouse_event(int button, int state, int xpos, int ypos)
		  :	glut_event(MOUSE), b(button), s(state), x(xpos), y(ypos)
	  {}
	  virtual void dispatch() { glut_mouse_function(b,s,x,y); }
	  int b, s, x, y;
  };

  struct glut_passive_motion_event : public glut_event
  {
	  glut_passive_motion_event(int xpos, int ypos)
		  :	glut_event(PASSIVE_MOTION), x(xpos), y(ypos)
	  {}
	  virtual void dispatch() { glut_passive_motion_function(x,y); }
	  int x, y;
  };

  struct glut_reshape_event : public glut_event
  {
	  glut_reshape_event(int width, int height)
		  :	glut_event(RESHAPE), w(width), h(height)
	  {}
	  virtual void dispatch() { glut_reshape_function(w,h); }
	  int w, h;
  };

  struct glut_special_event : public glut_event
  {
	  glut_special_event(int key, int xpos, int ypos)
		  :	glut_event(SPECIAL), k(key), x(xpos), y(ypos) 
	  {}
	  virtual void dispatch() { glut_special_function(k,x,y); }
	  int k, x, y;
  };

  struct glut_timer_event : public glut_event
  {
	  glut_timer_event(int value)
		  :	glut_event(TIMER), v(value) 
	  {}
	  virtual void dispatch() { glut_timer_function(v); }
	  int v;
  };

  struct glut_visibility_event : public glut_event
  {
	  glut_visibility_event(int visibility)
		  :	glut_event(VISIBILITY), v(visibility)
	  {}
	  virtual void dispatch() { glut_visibility_function(v); }
	  int v;
  };



  struct glut_replay : public glut_interactor
  {
	enum recorder_mode
	{
		RECORD, PLAY, STOP
	};

	glut_replay()
	{
		it = event_list.end();
		mode = STOP;
		paused = false;
	}

	virtual ~glut_replay()
	{ erase(); }

	virtual void display()
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_display_event());
	}
	virtual void idle()
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_idle_event());
	}
	virtual void keyboard(unsigned char key, int x, int y)
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_keyboard_event(key,x,y));
	}
	virtual void menu_status(int status, int x, int y) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_menu_status_event(status,x,y));
	}
	virtual void motion(int x, int y) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_motion_event(x,y));
	}
	virtual void mouse(int button, int state, int x, int y) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_mouse_event(button,state,x,y));
	}
	virtual void passive_motion(int x, int y) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_passive_motion_event(x,y));
	}
	virtual void reshape(int w, int h) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_reshape_event(w,h));
	}
	virtual void special(int  key, int x, int y) 
	{
		if(enabled)
		{
			if     (key == GLUT_KEY_F6)
			{ mode = RECORD; glut_event_processed(); erase(); }
			else if(key == GLUT_KEY_F7)
			{ mode = PLAY; glut_event_processed(); glutPostRedisplay(); }
			else if(key == GLUT_KEY_F8)
			{ mode = STOP; glut_event_processed(); }
			else if(key == GLUT_KEY_F9)
			{ paused = !paused; glut_event_processed(); glutPostRedisplay(); }
			else if(RECORD == mode)
			{ event_list.push_back(new glut_special_event(key,x,y)); }
		}
	}
	virtual void timer(int value) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_timer_event(value));
	}
	virtual void visibility(int v) 
	{
		if(enabled && RECORD == mode && ! paused)
			event_list.push_back(new glut_visibility_event(v));
	}

    
	// other methods

	bool playing() { return mode == PLAY; }

	void dispatch_accumulated_events()
	{
		if(mode == PLAY && ! paused)
		{
			while(it != event_list.end() && (*it)->type != glut_event::DISPLAY)
			{
				(*it)->dispatch();
				it++;
			}
			if(it == event_list.end())
			{
				mode = STOP;
				it = event_list.begin();
			}
			else if ((*it)->type == glut_event::DISPLAY)
			{
				it++;
			}
		}
	}

	void erase()
	{ 
		while(event_list.begin() != event_list.end())
		{
			glut_event * e = event_list.back();
			event_list.pop_back();
			delete e;
		}
	}

	std::list<glut_event *> event_list;
	std::list<glut_event *>::iterator it;
	recorder_mode mode;
	bool paused;
  };

}

#endif
