#include "utility.hpp"

grid::grid(vector2d position, int w, int h, double cellW, double cellH)
:position(position), width(w), height(h), cellWidth(cellW), cellHeight(cellH)
{}

void grid::render(aCamera *camera)
{
    double totalGridWidth = width * cellWidth;
    double totalGridHeight = height * cellHeight;
    double startX = position.x - totalGridWidth / 2.0;
    double endX = position.x + totalGridWidth / 2.0;
    double startY = position.y - totalGridHeight / 2.0;
    double endY = position.y + totalGridHeight / 2.0;

    SDL_Color minColor = {48, 48, 48, 255};
    double minThickness = 0.01;

    SDL_Color maxColor = {80, 80, 80, 255};
    double maxThickness = 0.5;

    std::vector<int> importanceIntervals = {16, 32, 64, 128};

    int maxImportanceLevel = static_cast<int>(importanceIntervals.size());

    SDL_Color xAxisColor = {180, 40, 40, 128};
    SDL_Color yAxisColor = {40, 180, 40, 128};
    double axisThickness = 0.3;

    vector2d a, b;

    for (int i = 0; i <= width; ++i)
    {
        double currentX = startX + i * cellWidth;
        SDL_Color chosenColor;
        double chosenThickness;

        if (i == 0 || i == width)
        {
            chosenColor = maxColor;
            chosenThickness = maxThickness;
        } else
        {
            int currentImportanceLevel = 0;
            for (int level = 0; level < importanceIntervals.size(); ++level)
            {
                if (i % importanceIntervals[level] == 0)
                {
                    currentImportanceLevel = level + 1;
                }
            }
            double factor = (maxImportanceLevel == 0) ? 0.0 :
                            static_cast<double>(currentImportanceLevel) / maxImportanceLevel;
            chosenColor = interpolateColor(minColor, maxColor, factor);
            chosenThickness = interpolateDouble(minThickness, maxThickness, factor);
        }

        a = {currentX, startY};
        b = {currentX, endY};
        camera->renderLine(a, b, chosenThickness, chosenColor, false);
    }

    for (int i = 0; i <= height; ++i)
    {
        double currentY = startY + i * cellHeight;
        SDL_Color chosenColor;
        double chosenThickness;

        if (i == 0 || i == height)
        {
            chosenColor = maxColor;
            chosenThickness = maxThickness;
        } else
        {
            int currentImportanceLevel = 0;
            for (int level = 0; level < importanceIntervals.size(); ++level)
            {
                if (i % importanceIntervals[level] == 0)
                {
                    currentImportanceLevel = level + 1;
                }
            }
            double factor = (maxImportanceLevel == 0) ? 0.0 :
                           static_cast<double>(currentImportanceLevel) / maxImportanceLevel;
            chosenColor = interpolateColor(minColor, maxColor, factor);
            chosenThickness = interpolateDouble(minThickness, maxThickness, factor);
        }
        a = {startX, currentY};
        b = {endX, currentY};
        camera->renderLine(a, b, chosenThickness, chosenColor, false);
    }

    if (0.0 >= startX && 0.0 <= endX)
    {
         camera->renderLine({0.0, startY}, {0.0, endY}, axisThickness, yAxisColor, false);
    }
	if (0.0 >= startY && 0.0 <= endY)
	{
	   camera->renderLine({startX, 0.0}, {endX, 0.0}, axisThickness, xAxisColor, false);
	}
}