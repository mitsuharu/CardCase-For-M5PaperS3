#pragma once
#include <Arduino.h>

struct Color
{
    enum Value
    {
        white,
        black
    };

    Value value;
    Color(Value v) : value(v) {}

    uint16_t m5Color();
};

using ColorValue = Color::Value;
