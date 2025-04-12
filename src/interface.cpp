#include "interface.hpp"

menu::menu()
:id("n/a"),
bound({0, 0, 0, 0})
{}

menuCon::menuCon()
:menu(),
size({50, 50}),
color({0, 0, 0, 255}),
alignment("c"),
sizeScaling("pixel"),
marginScaling("pixel"),
paddingScaling("pixel"),
margin({0, 0, 0, 0}),
padding({0, 0, 0, 0})
{}

menuText::menuText()
:menu(),
size({24, 24}),
color({0, 0, 0, 255}),
alignment("c"),
text(new std::string("new_text")),
textOwned(true)
{}

// ███    ███ ███████ ███    ██ ██    ██ 
// ████  ████ ██      ████   ██ ██    ██ 
// ██ ████ ██ █████   ██ ██  ██ ██    ██ 
// ██  ██  ██ ██      ██  ██ ██ ██    ██ 
// ██      ██ ███████ ██   ████  ██████  

void menu::update(aCamera* camera, SDL_FRect area, menu* root)
{
	for(int i = 0; i < int(elements.size()); ++i)
	{
		elements[i] -> update(camera, area, root);
	}
}

void menu::render(aCamera* camera, SDL_FRect area, menu* root)
{
	for(int i = 0; i < int(elements.size()); ++i)
	{	
		elements[i] -> render(camera, area, root);	
	}
}

menu* menu::getById(std::string nId)
{
   if (id == nId)
        return this;
    
    for (int i = 0; i < int(elements.size()); ++i)
    {   
        menu* found = elements[i]->getById(nId);
        if (found != nullptr)
            return found;
    }
    return nullptr;
}

void menu::setToken(std::string token, std::string value)
{
    if(token == "id")
        id = value;
    else
        log::error("menu::setToken - Mismatched token \"{}\" value \"{}\" for element \"menu\"", token, value);
}

void menu::cleanUp()
{
    for (int i = 0; i < int(elements.size()); ++i)
    {   
        elements[i] -> cleanUp();
        delete elements[i];
    }
}

// ███    ███ ███████ ███    ██ ██    ██  ██████  ██████  ███    ██ 
// ████  ████ ██      ████   ██ ██    ██ ██      ██    ██ ████   ██ 
// ██ ████ ██ █████   ██ ██  ██ ██    ██ ██      ██    ██ ██ ██  ██ 
// ██  ██  ██ ██      ██  ██ ██ ██    ██ ██      ██    ██ ██  ██ ██ 
// ██      ██ ███████ ██   ████  ██████   ██████  ██████  ██   ████ 


void menuCon::update(aCamera* camera, SDL_FRect area, menu* root)
{
    for(int i = 0; i < int(elements.size()); ++i)
    {
        elements[i] -> update(camera, area, root);
    }
}

void menuCon::render(aCamera* camera, SDL_FRect area, menu* root)
{
    SDL_FRect rect = area;
    SDL_FRect nMargin = {0, 0, 0, 0};
    SDL_FRect nPadding = {0, 0, 0, 0};

    if(sizeScaling != "pixel" && sizeScaling != "percent")
    {
        log::error("menuCon::render - sizeScaling \"{}\" not valid", sizeScaling);
    }

    if(marginScaling != "pixel" && marginScaling != "percent")
    {
        log::error("menuCon::render - marginScaling \"{}\" not valid", marginScaling);
    }

    if(paddingScaling != "pixel" && paddingScaling != "percent")
    {
        log::error("menuCon::render - paddingScaling \"{}\" not valid", paddingScaling);
    }


    rect.w = float((sizeScaling=="pixel") ? size.x : ((sizeScaling=="percent") ? (area.w*(size.x/100)) : 0));
    rect.h = float((sizeScaling=="pixel") ? size.y : ((sizeScaling=="percent") ? (area.h*(size.y/100)) : 0));

    nMargin.x = float((marginScaling=="pixel") ? margin.x : ((marginScaling=="percent") ? (margin.x*(rect.w/100)) : 0));
    nMargin.y = float((marginScaling=="pixel") ? margin.y : ((marginScaling=="percent") ? (margin.y*(rect.h/100)) : 0));
    nMargin.w = float((marginScaling=="pixel") ? margin.w : ((marginScaling=="percent") ? (margin.w*(rect.w/100)) : 0));
    nMargin.h = float((marginScaling=="pixel") ? margin.h : ((marginScaling=="percent") ? (margin.h*(rect.h/100)) : 0));

    nPadding.x = float((paddingScaling=="pixel") ? padding.x : ((paddingScaling=="percent") ? (padding.x*(rect.w/100)) : 0));
    nPadding.y = float((paddingScaling=="pixel") ? padding.y : ((paddingScaling=="percent") ? (padding.y*(rect.h/100)) : 0));
    nPadding.w = float((paddingScaling=="pixel") ? padding.w : ((paddingScaling=="percent") ? (padding.w*(rect.w/100)) : 0));
    nPadding.h = float((paddingScaling=="pixel") ? padding.h : ((paddingScaling=="percent") ? (padding.h*(rect.h/100)) : 0)); 

    if(alignment=="c")
    {
        rect.x += (area.w / 2 - rect.w / 2);
        rect.y += (area.h / 2 - rect.h / 2);
    }
    else if (alignment=="n")
    {
        rect.x += (area.w / 2 - rect.w / 2);
    }
    else if (alignment=="ne")
    {
        rect.x += area.w - rect.w;
    }
    else if (alignment=="e")
    {
        rect.x += area.w - rect.w;
        rect.y += (area.h / 2 - rect.h / 2);
    }
    else if (alignment=="se")
    {
        rect.x += area.w - rect.w;
        rect.y += area.h - rect.h;
    }
    else if (alignment=="s")
    {
        rect.x += (area.w / 2 - rect.w / 2);
        rect.y += area.h - rect.h;
    }
    else if (alignment=="sw")
    {
        rect.y += area.h - rect.h;
    }
    else if (alignment=="w")
    {
        rect.y += (area.h / 2 - rect.h / 2);
    }
    else if (alignment=="nw")  
    {
        rect.x = rect.x;
        rect.y = rect.y;
    }
    else
    {
        log::error("menuCon::render - alignment \"{}\" not valid", alignment);
    }

    rect.x += nMargin.h;
    rect.y += nMargin.x;

    camera -> renderRect(rect, color, true);
    bound = rect;
    area = rect;
    area.x += nPadding.h;
    area.y += nPadding.x;

    area.w -= nPadding.h;
    area.h -= nPadding.x;

    area.w -= nPadding.y;
    area.h -= nPadding.w;

    for(int i = 0; i < int(elements.size()); ++i)
    {   
        elements[i] -> render(camera, area, root);    
    }
}

void menuCon::setToken(std::string token, std::string value)
{
    if(token == "id")
        id = value;
    else if(token == "size")
        size = parser::parseVector(value);
    else if(token == "color")
        color = parser::parseColor(value);
    else if(token == "alignment")
        alignment = value;
    else if(token == "sizeScaling")
        sizeScaling = value;
    else if(token == "marginScaling")
        marginScaling = value;
    else if(token == "paddingScaling")
        paddingScaling = value;
    else if(token == "margin")
        margin = parser::parseRect(value);
    else if(token == "padding")
        padding = parser::parseRect(value);
    else
        log::error("menuCon::setToken - Mismatched token \"{}\" value \"{}\" for element \"con\"", token, value);
}


// ███    ███ ███████ ███    ██ ██    ██ ████████ ███████ ██   ██ ████████ 
// ████  ████ ██      ████   ██ ██    ██    ██    ██       ██ ██     ██    
// ██ ████ ██ █████   ██ ██  ██ ██    ██    ██    █████     ███      ██    
// ██  ██  ██ ██      ██  ██ ██ ██    ██    ██    ██       ██ ██     ██    
// ██      ██ ███████ ██   ████  ██████     ██    ███████ ██   ██    ██    


void menuText::update(aCamera* camera, SDL_FRect area, menu* root)
{
    for(int i = 0; i < int(elements.size()); ++i)
    {
        elements[i] -> update(camera, area, root);
    }
}

void menuText::render(aCamera* camera, SDL_FRect area, menu* root)
{
    SDL_FRect rect = area;

    if(alignment=="c")
    {
        rect.x += (area.w / 2);
        rect.y += (area.h / 2);
    }
    else if (alignment=="n")
    {
        rect.x += (area.w / 2);
    }
    else if (alignment=="ne")
    {
        rect.x += area.w;
    }
    else if (alignment=="e")
    {
        rect.x += area.w;
        rect.y += (area.h / 2);
    }
    else if (alignment=="se")
    {
        rect.x += area.w;
        rect.y += area.h;
    }
    else if (alignment=="s")
    {
        rect.x += (area.w / 2);
        rect.y += area.h;
    }
    else if (alignment=="sw")
    {
        rect.y += area.h;
    }
    else if (alignment=="w")
    {
        rect.y += (area.h / 2);
    }
    else if (alignment=="nw")  
    {
        rect.x = rect.x;
        rect.y = rect.y;
    }
    else
    {
        log::error("menuText::render - alignment \"{}\" not valid", alignment);
    }
    camera -> renderText({double(rect.x), double(rect.y)}, size.y, alignment, *text, color, true);

    for(int i = 0; i < int(elements.size()); ++i)
    {   
        elements[i] -> render(camera, area, root);    
    }
}

void menuText::setToken(std::string token, std::string value)
{
    if(token == "id")
        id = value;
    else if(token == "size")
        size = parser::parseVector(value);
    else if(token == "color")
        color = parser::parseColor(value);
    else if(token == "alignment")
        alignment = value;
    else if(token == "text")
    {
        delete text;
        std::string str = value;
        std::replace(str.begin(), str.end(), '_', ' ');
        text = new std::string(str);
        textOwned = true;
    }
    else
        log::error("menuText::setToken - Mismatched token \"{}\" value \"{}\" for element \"text\"", token, value);
}

void menuText::cleanUp()
{
    if(textOwned)
    {
        delete text;
    }
    
    for (int i = 0; i < int(elements.size()); ++i)
    {   
        elements[i] -> cleanUp();
        delete elements[i];
    }
}