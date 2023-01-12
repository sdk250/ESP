cmake_minimum_required(VERSION 3.25)

project(draw)

# set(
	# CMAKE_C_COMPILER /data/data/com.termux/files/home/android-ndk-r24/toolchains/llvm/prebuilt/linux-aarch64/bin/clang
# )
# set(
	# CMAKE_CXX_COMPILER /data/data/com.termux/files/home/android-ndk-r24/toolchains/llvm/prebuilt/linux-aarch64/bin/clang++
# )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_compile_options(
	--target=aarch64-linux-android
	-fPIC
	-fPIE
	-ferror-limit=5
	-O3
)

# link_directories(
	# /data/data/com.termux/files/home/NDK/lib/aarch64-linux-android
	# /data/data/com.termux/files/home/NDK/lib/aarch64-linux-android/31
	# # /system/lib64
# )

include_directories(
	# /data/data/com.termux/files/home/NDK/include
	# /data/data/com.termux/files/home/NDK/include/aarch64-linux-android
	# /data/data/com.termux/files/home/NDK/include/c++/v1
	include/frameworks/native/opengl/include
	include/frameworks/native/include
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

