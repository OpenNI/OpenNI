/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef _OPENGLES_
#define _OPENGLES_

#include "GLES/gl.h"
#include "GLES/egl.h"

bool opengles_init(int width, int height, EGLDisplay* peglDisplay, EGLSurface* peglSurface, EGLContext* peglContext);
void opengles_shutdown(EGLDisplay eglDisplay, EGLSurface eglSurface, EGLContext eglContext);

#endif //_OPENGLES_

