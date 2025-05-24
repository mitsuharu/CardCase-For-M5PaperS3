#include "Color.h"

#ifdef ARDUINO

#include <M5Unified.h>

uint16_t Color::m5Color()
{
    switch (value)
    {
    case black:
        return TFT_BLACK;
    case white:
        return TFT_WHITE;
    default:
        return TFT_WHITE;
    }
}

#endif
