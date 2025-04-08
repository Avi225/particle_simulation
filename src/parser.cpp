#include "parser.hpp"

parser::parser()
{}

menu* parser::parse(const char* filePath) {
    menu* m = nullptr;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        log::error(std::format("Error: Could not open file {}", filePath));
        return nullptr;
    }
    char c;
    tokens.clear();
    std::string token = "";

    while (file.get(c)) {
        if (std::isspace(static_cast<unsigned char>(c)))
            continue;
        if (c == '{' || c == '}') {
            if (!token.empty()) {
                tokens.push_back(token);
            }
            tokens.push_back(std::string() + c);
            token = "";
            continue;
        }
        token += c;
    }
    file.close();
    
    if (!token.empty()) {
        tokens.push_back(token);
    }

    size_t pos = 0;
    m = parseMenu(pos, tokens, filePath);
    return m;
}

menu* parser::parseMenu(size_t &pos, const std::vector<std::string>& tokens, const char* filePath)
{
    if(tokens.empty()){
        return nullptr;
    }
    menu* m = new menu();
    if (tokens[pos] == "con") {
        delete m;
        m = new menuCon();
    }else
    if (tokens[pos] == "text") {
        delete m;
        m = new menuText();
    }else
    {
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected element type but found \"{}\"", filePath, pos, tokens[pos]));
        delete m;
        return nullptr;
    }

    pos++;
    if(!(pos < tokens.size())){
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"{}\"", filePath, pos, tokens[pos-1]));
        return nullptr;
    }

    if (tokens[pos] != "{") {
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"con\", but found \"{}\"", filePath, pos, tokens[pos]));
        return nullptr;
    }
    pos++;
    if(!(pos < tokens.size())){
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" to close container", filePath, pos));
        return nullptr;
    }

    while (pos < tokens.size() && tokens[pos] != "}")
    {  
        if (tokens[pos] == "elements")
        {
            pos++;

            if(!(pos < tokens.size())){
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"{}\"", filePath, pos, tokens[pos-1]));
                delete m;
                return nullptr;
            }

            if (tokens[pos] != "{") {
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"elements\", but found \"{}\"", filePath, pos, tokens[pos]));
                delete m;
                return nullptr;
            }
            pos++;
            if(!(pos < tokens.size())){
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" to close elements block", filePath, pos));
                delete m;
                return nullptr;
            }
            while (pos < tokens.size() && tokens[pos] != "}")
            {
                menu* child = parseMenu(pos, tokens, filePath);
                if (child) {
                    m->elements.push_back(child);
                } else {
                    log::error(std::format("Error: In file \"{}\", failed to parse menu", filePath));
                    delete m;
                    return nullptr;
                }
            }
            if (tokens[pos] != "}")
            {
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" to close elements block, but found \"{}\"", filePath, pos, tokens[pos]));
                delete m;
                return nullptr;
            }
            pos++;
        }else
        {
            std::string token = tokens[pos];
            pos++;

            if(!(pos < tokens.size())){
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"{}\"", filePath, pos, tokens[pos-1]));
                delete m;
                return nullptr;
            }

            if (tokens[pos] != "{")
            {
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"{{\" after \"{}\", but found \"{}\"", filePath, pos, tokens[pos-1], tokens[pos]));
                delete m;
                return nullptr;
            }
            pos++;

            if(!(pos < tokens.size())){
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected value after \"{}\"", filePath, pos, tokens[pos-1]));
                delete m;
                return nullptr;
            }
            
            m -> setToken(token, tokens[pos]);

            pos++;

            if(!(pos < tokens.size())){
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" after \"{}\"", filePath, pos, tokens[pos-1]));
                delete m;
                return nullptr;
            }

            if (tokens[pos] != "}")
            {
                log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" after \"{}\", but found \"{}\"", filePath, pos, tokens[pos-1], tokens[pos]));
                delete m;
                return nullptr;
            }
            pos++;
        }
    }

    if(!(pos < tokens.size()))
    {
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" to close container", filePath, pos));
        delete m;
        return nullptr;
    }

    if (tokens[pos] != "}")
    {
        log::error(std::format("Error: In file \"{}\" at token index \"{}\", expected \"}}\" to close container, but found \"{}\"", filePath, pos, tokens[pos]));
        delete m;
        return nullptr;
    }
    pos++;

    return m;
}

vector2d parser::parseVector(const std::string& token) {
    vector2d v;
    if (sscanf(token.c_str(), "%lf,%lf", &v.x, &v.y) != 2) {
        log::error(std::format("Error: Failed to parse vector2d from token \"{}\", setting to default value", token));
        v = {0, 0};
    }
    return v;
}

SDL_Color parser::parseColor(const std::string& token) {
    SDL_Color color;
    if (sscanf(token.c_str(), "%hhu,%hhu,%hhu,%hhu", &color.r, &color.g, &color.b, &color.a) != 4) {
        log::error(std::format("Error: Failed to parse SDL_Color from token \"{}\", setting to default value", token));
        color = {0, 0, 0, 255};
    }
    return color;
}

SDL_FRect parser::parseRect(const std::string& token) {
    SDL_FRect rect;
    if (sscanf(token.c_str(), "%f,%f,%f,%f", &rect.x, &rect.y, &rect.w, &rect.h) != 4) {
        log::error(std::format("Error: Failed to parse SDL_FRect from token \"{}\", setting to default value", token));
        rect = {0, 0, 0, 0};
    }
    return rect;
}