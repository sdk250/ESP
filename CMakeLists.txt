cmake_minimum_required(VERSION 3.25)

project(draw)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_compile_options(
	--target=aarch64-linux-android
	-m64
	-fPIC
	-fPIE
	-ferror-limit=5
	-Wall
	-Werror
	-Wno-bitwise-op-parentheses
	-O3
)

include_directories(
	include/frameworks/native/opengl/include
	include/frameworks/native/libs/gui/include
	include/frameworks/native/libs/ui/include
	include/frameworks/native/libs/nativewindow/include
	include/frameworks/native/libs/nativebase/include
	include/frameworks/native/libs/binder/include
	include/system/core/libutils/include
	include/system/core/libcutils/include
	include/system/core/libsystem/include
	include/hardware/libhardware/include
	include/ImGUI
	include
)

aux_source_directory(
	src
	SRC
)
aux_source_directory(
	src/ImGUI
	SRC_ImGUI
)

add_executable(
	draw
	${SRC}
	${SRC_ImGUI}
)

target_link_libraries(
	draw
	EGL
	GLESv3
	nativewindow
	android
	gui
)

