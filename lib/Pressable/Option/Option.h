#pragma once
#include <Arduino.h>
#include "Color.h"

struct Option
{
    int textSize = 6;
    Color fontColor = Color(ColorValue::black);
    Color backgroundColor = Color(ColorValue::white);
};
