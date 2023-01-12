#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <ui/PixelFormat.h>
#include <android/native_window.h>
#include <utils/StrongPointer.h>

ANativeWindow* createNaiveWindow(
	const char* name,
	uint32_t width,
	uint32_t height,
	android::PixelFormat format,
	uint32_t flags
)
{
	android::sp<android::SurfaceComposerClient> pClient {
		new android::SurfaceComposerClient>
	};
	android::sp<android::SurfaceControl> pControl {
		pClient->createSurface(
			(android::String8) name,
			width,
			height,
			format,
			flags
		)
	};
	return pControl->getSurface().get();
}