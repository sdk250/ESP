#pragma once

#include <EGL/egl.h>
#include <android/native_window.h>

#include <global_value>

#define ATTRIB_SIZE 19

class InitEGL
{
	private:
		// typedef void* EGLDisplay, EGLConfig, EGLSurface, EGLContext
		EGLDisplay EGL_display = EGL_NO_DISPLAY;
		EGLConfig EGL_config = nullptr;
		EGLSurface EGL_surface = EGL_NO_SURFACE;
		EGLContext EGL_context = EGL_NO_CONTEXT;

		// typedef signed int EGLint
		EGLint* version = nullptr;
		EGLint* EGL_attribs = nullptr;
		EGLint num_config = -1;
	public:
		InitEGL(void);
		~InitEGL(void);
		inline EGLint get_num_config(void) const {return num_config;};
		inline EGLDisplay get_egl_display(void) const {return EGL_display;};
		EGLSurface get_egl_surface(ANativeWindow *);
};