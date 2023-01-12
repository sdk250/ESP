#include <thread>
#include <dlfcn.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>

#include <global_value>
#include <init_egl.hpp>
#include <android_touch.hpp>
#include <ui/PixelFormat.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_android.h>
#include <ImGUI/imgui_impl_opengl3.h>

std::string exec(const std::string&);

static uint32_t width = 300;
static uint32_t height = 400;

int main(void)
{
	InitEGL* ie = nullptr;
	void* handle = nullptr;
	EGLDisplay EGL_display = EGL_NO_DISPLAY;
	EGLSurface EGL_surface = EGL_NO_SURFACE;
	ImFontConfig font_cfg {};
	ImGuiStyle style {};
	bool show_window {true};
	ImVec4 clean_color = ImVec4(0, 0, 0, 0);
	ANativeWindow* (*winfn)
		(const char*, uint32_t, uint32_t, android::PixelFormat, uint32_t) = nullptr;
	ANativeWindow* addr = nullptr;

	std::string window_size = exec("wm size");
	sscanf(window_size.c_str(), "Physical size: %ux%u", &width, &height);
	handle = dlopen("../lib/libnwindow.so.7", RTLD_LAZY);
	winfn = (ANativeWindow* (*)
		(const char*, uint32_t, uint32_t, android::PixelFormat, uint32_t))
		dlsym(handle, "createNativeWindow");
	addr = winfn(
		"sdk250",
		width,
		height,
		android::PIXEL_FORMAT_RGBA_8888,
		(uint32_t) 0
	);
	printf("Surface address: %p\n", addr);
	ie = new InitEGL;
	EGL_display = ie->get_egl_display();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui::StyleColorsDark();
	EGL_surface = ie->get_egl_surface(addr);
	if (!ImGui_ImplAndroid_Init(addr))
	{
		delete ie;
		perror("Error: cannot initialized ImGUI for the Android\nDetail");
		exit(EXIT_FAILURE);
	}
	ImGui_ImplOpenGL3_Init("#version 300 es");
	font_cfg.SizePixels = 30.0f;
	io.Fonts->AddFontDefault(&font_cfg);
	style = ImGui::GetStyle();
	style.ScaleAllSizes(10.0f);

	std::thread Touch {Android_Touch, std::ref(io)};
	Touch.detach();
	for (;;)
	{
		if (shutdown)
			break;

		// Draw
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplAndroid_NewFrame();
		ImGui::NewFrame();
		if (show_window)
			ImGui::ShowDemoWindow(&show_window);

		{
			static float x_f = 0.0f;
			static float y_f = 0.0f;
			static unsigned int counter = 0;

			ImGui::Begin("Hello, World!");

			ImGui::Text("This is some useful text.");
			ImGui::Checkbox("Demo Window", &show_window);

			ImGui::SliderFloat("<-- Window size for x", &x_f, 0.0f, 10.0f);
			ImGui::SliderFloat("<-- Window size for y", &y_f, 0.0f, 10.0f);
			ImGui::SetWindowSize(ImVec2(x_f * 100, y_f * 130));

			ImGui::Button("Add") ? counter++ : 0x0;
			ImGui::SameLine();
			ImGui::Text("Counter: %u", counter);

			ImGui::SameLine();
			ImGui::Button("Shutdown") ? shutdown = true : 0x0;

			ImGui::Text(
				"Application average %.3f ms/frame, %.4f FPS.",
				1000 / io.Framerate,
				io.Framerate
			);

			ImGui::End();
		}
		ImGui::GetForegroundDrawList()->AddLine(
			ImVec2(0, 0), // Begin
			ImVec2(width, height), // End
			IM_COL32(0, 255, 0, 255),
			16
		);

		ImGui::Render();
		glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);
		glClearColor(
			clean_color.x * clean_color.w,
			clean_color.y * clean_color.w,
			clean_color.z * clean_color.w,
			clean_color.w
		);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		eglSwapBuffers(EGL_display, EGL_surface);
	}

	delete ie;
	dlclose(handle);
	ImGui_ImplAndroid_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	puts("\x1b[92;mDraw End.\x1b[0m");
	return 0;
}

std::string exec(const std::string& command)
{
	char buffer[128];
	std::string result {};
	// Open pipe to file
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		return "popen failed!";
	}
	// read till end of process:
	while (!feof(pipe)) {
		// use buffer to read and add to result
		if (fgets(buffer, 128, pipe) != nullptr)
			result += buffer;
	}
	pclose(pipe);
	return result;
}