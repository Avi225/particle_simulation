#pragma once

#include <fstream>
#include <functional>
#include <format>

#include "math.hpp"
#include "utility.hpp"
#include "log.hpp"
#include "interface.hpp"

struct menu;

struct parser
{
	parser();
	menu* parse(const char* filePath);

	menu* parseMenu(size_t &pos, const std::vector<std::string>& tokens, const char* filePath);

	static vector2d parseVector(const std::string& token);
	static SDL_Color parseColor(const std::string& token);
	static SDL_FRect parseRect(const std::string& token);

	std::vector<std::string> tokens;
};
