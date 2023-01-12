#include <init_egl.hpp>

InitEGL::InitEGL(void)
{
	if ((EGL_display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
	{
		perror("Init EGL display failure.\nDetail");
		fprintf(stderr, "Error code: %d\n", (signed int) eglGetError());
		exit(EXIT_FAILURE);
	}
	version = new EGLint [2];
	EGL_attribs = new EGLint [ATTRIB_SIZE];
	if (memcpy(
		(void *) EGL_attribs,
		(const void *) ((EGLint []) {
			EGL_BUFFER_SIZE, 32,
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_STENCIL_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
		}),
		sizeof(EGLint) * ATTRIB_SIZE
	) == nullptr)
	{
		perror("Set EGL attribute failed.\nDetail");
		this->~InitEGL();
		exit(EXIT_FAILURE);
	}
	if (eglInitialize(EGL_display, version, version + 1) == EGL_FALSE)
	{
		perror("Init EGL failure.\nDetail");
		this->~InitEGL();
		exit(EXIT_FAILURE);
	}
	if (eglGetConfigs(EGL_display, nullptr, (EGLint) 1, &num_config) == EGL_FALSE)
	{
		perror("Fail to get total count for config of EGL.\nDetail");
		this->~InitEGL();
		exit(EXIT_FAILURE);
	}
	if (
		eglChooseConfig(EGL_display, (const EGLint *)EGL_attribs, &EGL_config, (EGLint) 1, &num_config)
		== EGL_FALSE || EGL_config == nullptr
	)
	{
		perror("Cannot choose config of EGL.\nDetail");
		this->~InitEGL();
		exit(EXIT_FAILURE);
	}
	if ((EGL_context = eglCreateContext(EGL_display, EGL_config, EGL_NO_CONTEXT,
		(EGLint []) {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		}
	)) == EGL_NO_CONTEXT)
	{
		perror("Cannot create EGL context.\nDetail");
		this->~InitEGL();
		exit(EXIT_FAILURE);
	}
}

InitEGL::~InitEGL(void)
{
	puts("Start destroy memory of EGL....");
	if (EGL_display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(EGL_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		EGL_context != EGL_NO_CONTEXT ? eglDestroyContext(EGL_display, EGL_context) : 0x0;
		EGL_surface != EGL_NO_SURFACE ? eglDestroySurface(EGL_display, EGL_surface) : 0x0;
		eglTerminate(EGL_display);
	}
	EGL_display = EGL_NO_DISPLAY;
	EGL_surface = EGL_NO_SURFACE;
	EGL_context = EGL_NO_CONTEXT;

	delete [] version;
	delete [] EGL_attribs;

	puts("\x1b[92;mEGL End.\x1b[0m");
}

EGLSurface InitEGL::get_egl_surface(ANativeWindow* window)
{
	if (EGL_surface != EGL_NO_SURFACE)
		return EGL_surface;
	if ((EGL_surface = eglCreateWindowSurface(EGL_display, EGL_config, window, nullptr))
		== EGL_NO_SURFACE
	)
		perror("No surface\nDetail");
	if (eglMakeCurrent(EGL_display, EGL_surface, EGL_surface, EGL_context) == EGL_FALSE)
		perror("Make current surface failure.\nDetail");
	return EGL_surface;
}
