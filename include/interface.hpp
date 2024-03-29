#pragma once

#include "aCamera.hpp"
#include "math.hpp"

#include <vector>
#include <string>
#include <map>

struct state
{
	state();
	state(bool nHovered, bool nPressed, bool nSelected, bool nButton);

	bool hovered;
	bool pressed;
	bool selected;
	bool button;
};

struct tabElement
{
	virtual void render(aCamera* camera, vector2d position);
	virtual void update() {}
	int height;
	vector2d adjustment;
	std::string info;
};

struct tabText : public tabElement
{
	tabText(std::string nValue, int nSize, std::string info);
	void render(aCamera* camera, vector2d position);

	std::string value;
	int size;
};

struct tabBreak : public tabElement
{
	tabBreak();
	void render(aCamera* camera, vector2d position);
};

struct tabSliderD : public tabElement
{
	tabSliderD(double* nValue, double nMinValue, double nMaxValue, std::string info);
	void render(aCamera* camera, vector2d position);
	void update();

	double* value;
	double minValue;
	double maxValue;
};

struct tabDisplayI : public tabElement
{
	tabDisplayI(int* nValue, int nSize, std::string nText, std::string info);
	void render(aCamera* camera, vector2d position);

	int* value;
	int size;
	std::string text;
};

struct tabDisplayD : public tabElement
{
	tabDisplayD(double* nValue, int nSize, std::string nText, std::string info);
	void render(aCamera* camera, vector2d position);

	double* value;
	int size;
	std::string text;
};

struct tab
{
	tab(vector2d nPosition, std::string nName);

	void insertElement(tabElement* element);
	
	void render(aCamera* camera, vector2d globalPosition);

	std::vector<tabElement*> elements;
	vector2d position;
	int margin;
	std::string name;
	state tabState;
};

class menu
{
public:
	menu(vector2d nPosition);
	void insertTab(tab* nTab);
	void updateTabs();
	void render(aCamera* camera);

private:
	vector2d position;
	std::map<std::string, tab*> tabs;

	int margin;
};