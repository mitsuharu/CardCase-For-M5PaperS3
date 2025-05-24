#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
using String = std::string;
#endif

#include "Option/Option.h"
#include "Rect/Rect.h"

class Pressable
{
    typedef std::function<void(Pressable &pressable)> CallbackFunction;

public:
    int tag = 0;
    String userInfo = "";

    void show(String text, CallbackFunction onPress = NULL, Option option = Option());
    void loop();
    void release();

private:
    Rect rect;
    CallbackFunction onPress = NULL;
};
