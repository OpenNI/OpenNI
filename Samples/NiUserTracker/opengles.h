#ifndef _OPENGLES_
#define _OPENGLES_

#include "GLES/gl.h"
#include "GLES/egl.h"

bool opengles_init(int width, int height, EGLDisplay* peglDisplay, EGLSurface* peglSurface, EGLContext* peglContext);
void opengles_shutdown(EGLDisplay eglDisplay, EGLSurface eglSurface, EGLContext eglContext);

#endif //_OPENGLES_

