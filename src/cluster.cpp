#include "src/cluster.hpp"

Color ColorFromCode(ColorCodes code)
{
    switch (code)
    {
    case COL_RED:
        return RED;
    case COL_ORANGE:
        return ORANGE;
    case COL_YELLOW:
        return YELLOW;
    case COL_GREEN:
        return GREEN;
    case COL_CYAN:
        return SKYBLUE;
    case COL_BLUE:
        return BLUE;
    case COL_PURPLE:
        return PURPLE;
    case COL_WHITE:
        return WHITE;
    case COL_GRAY:
        return GRAY;
    case COL_LIGHTGRAY:
        return LIGHTGRAY;
    case COL_DARKGRAY:
        return DARKGRAY;
    case COL_BLACK:
        return BLACK;
    case COL_BLANK:
    default:
        return BLANK;
    }
}