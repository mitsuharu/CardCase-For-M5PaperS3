#include "Rect.h"

bool Rect::contains(int tx, int ty)
{
    return (tx >= x && tx <= x + width && ty >= y && ty <= y + height);
};