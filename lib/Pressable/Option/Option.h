#pragma once
#include <Arduino.h>
#include "Color.h"

struct Option
{
    int textSize = 8;
    Color fontColor = Color(ColorValue::black);
    Color backgroundColor = Color(ColorValue::white);
};
