#include <stdio.h>
#include "opengles.h"

bool TestEGLError(char* pszLocation)
{
	/*
		eglGetError returns the last error that has happened using egl,
		not the status of the last called function. The user has to
		check after every single egl call or at least once every frame.
	*/
	EGLint iErr = eglGetError();
	if (iErr != EGL_SUCCESS)
	{
		printf("%s failed (%d).\n", pszLocation, iErr);
		return false;
	}

	return true;
}

static bool setup_egl(NativeDisplayType nativeDisplay, NativeWindowType nativeWindow, EGLDisplay* peglDisplay, EGLSurface* peglSurface, EGLContext* peglContext)
{
	EGLDisplay eglDisplay = EGL_NO_DISPLAY;
	EGLSurface eglSurface = EGL_NO_SURFACE;
	EGLContext eglContext = EGL_NO_CONTEXT;
	EGLConfig  eglConfig  = 0;

	/*
	Step 1 - Get the default display.
		EGL uses the concept of a "display" which in most environments
		corresponds to a single physical screen. Since we usually want
		to draw to the main screen or only have a single screen to begin
		with, we let EGL pick the default display.
		Querying other displays is platform specific.
	*/
	eglDisplay = eglGetDisplay(nativeDisplay);

	/*
		Step 2 - Initialize EGL.
		EGL has to be initialized with the display obtained in the
		previous step. We cannot use other EGL functions except
		eglGetDisplay and eglGetError before eglInitialize has been
		called.
		If we're not interested in the EGL version number we can just
		pass NULL for the second and third parameters.
	*/
	EGLint iMajorVersion = 0,
	       iMinorVersion = 0;

	if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
	{
		printf("Error: eglInitialize() failed.\n");
		return false;
	}

	/*
		Step 3 - Specify the required configuration attributes.
		An EGL "configuration" describes the pixel format and type of
		surfaces that can be used for drawing.
		For now we just want to use a 16 bit RGB surface that is a
		Window surface, i.e. it will be visible on screen. The list
		has to contain key/value pairs, terminated with EGL_NONE.
	 */
	EGLint configAttrs[] =
	{
		EGL_BUFFER_SIZE,       32,
	        EGL_DEPTH_SIZE,        24,
        	EGL_NONE
	};
	
	/*
		Step 4 - Find a config that matches all requirements.
		eglChooseConfig provides a list of all available configurations
		that meet or exceed the requirements given as the second
		argument. In most cases we just want the first config that meets
		all criteria, so we can limit the number of configs returned to 1.
	*/
	int iConfigs;
	if (!eglChooseConfig(eglDisplay, configAttrs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		int errcode = eglGetError();
		printf("Error: eglChooseConfig() failed. with ErrorCode : 0x%08X\n", errcode);
		eglTerminate(eglDisplay);
		return false;
	}

	/*
		Step 5 - Create a surface to draw to.
		Use the config picked in the previous step and the native window
		handle when available to create a window surface. A window surface
		is one that will be visible on screen inside the native display (or
		fullscreen if there is no windowing system).
		Pixmaps and pbuffers are surfaces which only exist in off-screen
		memory.
	*/
	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nativeWindow, NULL);
	if (!TestEGLError("eglCreateWindowSurface"))
	{
		eglTerminate(eglDisplay);
		return false;
	}

	/*
		Step 6 - Create a context.
		EGL has to create a context for OpenGL ES. Our OpenGL ES resources
		like textures will only be valid inside this context
		(or shared contexts)
	*/
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
	if (!TestEGLError("eglCreateContext"))
	{
		eglTerminate(eglDisplay);
		return false;
	}

	/*
		Step 7 - Bind the context to the current thread and use our
		window surface for drawing and reading.
		Contexts are bound to a thread. This means you don't have to
		worry about other threads and processes interfering with your
		OpenGL ES application.
		We need to specify a surface that will be the target of all
		subsequent drawing operations, and one that will be the source
		of read operations. They can be the same surface.
	*/
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!TestEGLError("eglMakeCurrent"))
	{
		eglTerminate(eglDisplay);
		return false;
	}

	(*peglDisplay) = eglDisplay;
	(*peglSurface) = eglSurface;
	(*peglContext) = eglContext;

	return true;
}

bool opengles_init(int width, int height, EGLDisplay* peglDisplay, EGLSurface* peglSurface, EGLContext* peglContext)
{
	NativeDisplayType display = (NativeDisplayType)EGL_DEFAULT_DISPLAY;
	NativeWindowType window;

	setup_egl(display, window, peglDisplay, peglSurface, peglContext);

	return true;
}

void opengles_shutdown(EGLDisplay eglDisplay, EGLSurface eglSurface, EGLContext eglContext)
{
    eglDestroyContext(eglDisplay, eglContext);
    eglDestroySurface(eglDisplay, eglSurface);
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(eglDisplay);

}
