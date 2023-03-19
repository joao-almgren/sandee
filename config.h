#pragma once

namespace Config
{
	extern int width;
	extern int height;
	extern bool windowed;
	extern float nearPlane;
	extern float farPlane;

	[[nodiscard]] bool load();
}
