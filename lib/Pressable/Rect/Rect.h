#pragma once

struct Rect
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    bool contains(int tx, int ty);
};
