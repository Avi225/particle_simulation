#pragma once

#include "aCamera.hpp"
#include "math.hpp"

#include <vector>
#include <string>

struct menuElement
{};

struct text: public menuElement
{
	text(std::string nValue, int nSize);

	std::string value;
	int size;
	float height;
};

class menu
{
public:
	menu(){};

	void render(aCamera* camera);

private:
	std::vector<menuElement> elements;
	vector2f position;

};
