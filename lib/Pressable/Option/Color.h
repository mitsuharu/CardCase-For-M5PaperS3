#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

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
