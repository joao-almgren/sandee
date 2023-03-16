#pragma once

class Config
{
public:
	int width;
	int height;
	bool windowed;

	[[nodiscard]] bool load();
};
