#include "Pressable.h"
#include <M5Unified.h>

void Pressable::show(String text, CallbackFunction onPress, Option option)
{
    rect = Rect();
    rect.x = 0;
    rect.y = M5.Display.getCursorY();

    M5.Display.setTextColor(option.fontColor.m5Color(), option.backgroundColor.m5Color());

    M5.Display.setTextSize(option.textSize);
    M5.Display.println(text.c_str());

    this->onPress = onPress;

    rect.width = M5.Display.width();
    rect.height = M5.Display.getCursorY() - rect.y;
}

void Pressable::loop()
{
    auto t = M5.Touch.getDetail();
    if (t.wasPressed() && rect.contains(t.x, t.y))
    {
        M5.Display.setTextSize(8);
        M5.Display.printf("onPress !");

        if (onPress != NULL)
        {
            onPress(*this);
        }
    }
}

void Pressable::release()
{
    onPress = NULL;
}
