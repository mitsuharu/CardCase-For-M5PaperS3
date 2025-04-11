#pragma once

#include <Arduino.h>

struct MyHelper
{
    enum class Rotation
    {
        Up = 0,
        Right = 1,
        Down = 2,
        Left = 3
    };

    enum class Color
    {
        white,
        black
    };

    struct Size
    {
        int width;
        int height;
    };

    // SDカード内の画像ファイルを描画する関数
    static void drawImageFromSD(String path, Rotation rotation = Rotation::Down, bool shouldSleepAfterDraw = false);

    static Size drawText(String text, int x, int y, int textSize = 6, Color fontColor = Color::black, Color bgColor = Color::white);
};
