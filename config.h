#pragma once
#include <nlohmann/json.hpp>
#include <cstdio>
#include <memory>

struct Config
{
	int width;
	int height;
};

Config readConfig()
{
	using namespace std;
	using json = nlohmann::json;

	FILE * infile;
	auto err = fopen_s(&infile, "config.json", "rt");

	err = fseek(infile, 0, SEEK_END);
	const long numBytes = ftell(infile);

	err = fseek(infile, 0, SEEK_SET);
	const unique_ptr<char> buffer(new char[numBytes + 1]);

	fread(buffer.get(), sizeof(char), numBytes, infile);
	err = fclose(infile);

	buffer.get()[numBytes] = 0;

	json data = json::parse(buffer.get());

	const Config config
	{
		.width = data["width"].get<int>(),
		.height = data["height"].get<int>(),
	};

	return config;
}
