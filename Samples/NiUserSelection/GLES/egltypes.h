/*****************************************************************************
 Name :		egltypes.h
 Date :		11/04/05
 Platform : Linux NULL WS

 Description :
 Native egltypes.h as required by Khronos OpenGL ES egl.h

 This file is part of the PowerVR OpenGL ES SDK.

 Copyright 2000-2005 by Imagination Technologies Ltd. All rights reserved.
 Information and source code samples contained herein are
 provided "as-is", without representations or warranties, and
 are subject to change without notice. The author cannot support
 modifications or derivative works created from the sample source
 code provided. You may use, reproduce, and modify portions or
 entire sections of the sample source code for the purposes of
 creating applications. Distribution is limited to executable
 or binary portions of the sample source code unless you gain
 written permission from the author.
*****************************************************************************/
#ifndef _egltypes_h_
#define _egltypes_h_

#define APIENTRY

/* Types */
typedef int EGLBoolean;
#define EGL_FALSE 0
#define EGL_TRUE  1

/* An integer of at least 32 bits */
typedef int EGLint;

/* Resources */
typedef EGLint EGLDisplay;
typedef EGLint EGLConfig;
typedef struct _KEGL_SURFACE_ *EGLSurface;
typedef struct _KEGL_CONTEXT_ *EGLContext;

/* Windowing system: Linux NULL */
typedef int		 NativeDisplayType;
typedef void	*NativeWindowType;
typedef void	*NativePixmapType;

/* May need to export on some platforms */
#define GLAPI_EXT

/* EGL and native handle values */
#define EGL_DEFAULT_DISPLAY ((NativeDisplayType)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

#endif
