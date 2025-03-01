#include "screen_capture.h"

#include <memory>

#if !defined(EDITOR)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/debug/debug.h>
#include <engine/application.h>

bool ScreenCapture::MakeScreenshot(const std::string& fileName)
{
	if (Graphics::usedCamera)
	{
		std::unique_ptr<uint8_t[]> frameBufferData = Graphics::usedCamera->GetRawFrameBuffer();
		if(!frameBufferData)
		{
			Debug::PrintError("Failed to get the framebuffer data");
			return false;
		}

		std::string path = fileName + ".png";

#if defined(__vita__)
		path = std::string(PSVITA_DEBUG_LOG_FOLDER) + "screenshots/" + path;
#elif defined(__PS3__)
		path = Application::GetMountingPoint() + std::string(PS3_DATA_FOLDER) + path;
#endif

		// On some platforms, the framebuffer is flipped vertically
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
		stbi_flip_vertically_on_write(true); // Windows / PsVita
#else // PSP, PS3
		stbi_flip_vertically_on_write(false); // PSP / PS3
#endif

		if (stbi_write_png(path.c_str(), Graphics::usedCamera->GetWidth(), Graphics::usedCamera->GetHeight(), 3, frameBufferData.get(), 0) == 0)
		{
			Debug::PrintError("Failed to export the framebuffer data");
			return false;
		}

		return true;
	}

	return false;
}
