#pragma once

#include <fstream>
#include <functional>

#include "math.hpp"
#include "aCamera.hpp"
#include "utility.hpp"
#include "log.hpp"
#include "parser.hpp"

struct menu
{
	virtual ~menu() = default;
	menu();

	virtual void update(aCamera* camera, SDL_FRect area, menu* root);
	virtual void render(aCamera* camera, SDL_FRect area, menu* root);
	virtual void setToken(std::string token, std::string value);
	virtual menu* getById(std::string nId);
	virtual void cleanUp();

	std::vector<menu*> elements;

	std::string id;
	SDL_FRect bound;
};

struct menuCon : public menu
{
	menuCon();

	void update(aCamera* camera, SDL_FRect area, menu* root);
	void render(aCamera* camera, SDL_FRect area, menu* root);
	void setToken(std::string token, std::string value);

	vector2d size;
	SDL_Color color;

	std::string alignment; // c / n / ne / e / se / s / sw / w / nw
	std::string sizeScaling; // pixel / percent
	std::string marginScaling; // pixel / percent
	std::string paddingScaling; // pixel / percent

	SDL_FRect margin;
	SDL_FRect padding;
};

struct menuText : public menu
{
	menuText();

	void update(aCamera* camera, SDL_FRect area, menu* root);
	void render(aCamera* camera, SDL_FRect area, menu* root);
	void setToken(std::string token, std::string value);
	void cleanUp();


	vector2d size;
	SDL_Color color;

	std::string alignment; // c / n / ne / e / se / s / sw / w / nw

	std::string* text;
	bool textOwned;

};