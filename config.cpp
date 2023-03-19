#include "config.h"
#include <nlohmann/json.hpp>
#include <cstdio>
#include <memory>

using namespace std;
using json = nlohmann::json;

namespace Config
{
	int width = 800;
	int height = 600;
	bool windowed = true;
	float nearPlane = 0.1f;
	float farPlane = 1000;

	bool load()
	{
		FILE* infile;
		if (fopen_s(&infile, "config.json", "rt"))
			return false;

		if (fseek(infile, 0, SEEK_END))
			return false;

		const long numBytes = ftell(infile);
		if (fseek(infile, 0, SEEK_SET))
			return false;

		const unique_ptr<char> buffer(new char[numBytes + 1]);
		char* const pBuffer = buffer.get();
		const size_t bytesRead = fread(pBuffer, sizeof(char), numBytes, infile);
		if (!bytesRead)
			return false;

		if (fclose(infile))
			return false;

		pBuffer[bytesRead] = 0;
		json data = json::parse(pBuffer);

		width = data["width"].get<int>();
		height = data["height"].get<int>();
		windowed = data["windowed"].get<bool>();
		nearPlane = data["nearPlane"].get<float>();
		farPlane = data["farPlane"].get<float>();

		return true;
	}
}
