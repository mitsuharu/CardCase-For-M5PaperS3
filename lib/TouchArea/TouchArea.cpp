#include "./TouchArea.h"

// タッチしたエリアを判定するための構造体
bool TouchArea::contains(int tx, int ty)
{
    return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
}

// 引数の座標が、同じく引数で与えたTouchAreaのどれにあたるか調べる
int TouchArea::findIndex(int tx, int ty, TouchArea *list, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (list[i].contains(tx, ty))
        {
            return i;
        }
    }
    return -1; // 該当なし
};

#ifdef ARDUINO
// native テストに影響しないようにマクロで分ける
#include <M5Helper.h>

// TouchAreaの作成と表示を同時にする
TouchArea TouchArea::draw(String text, String path, int x, int y, int textSize)
{
    M5Helper::Size size = M5Helper::drawText(text, x, y, textSize);
    TouchArea area = {x, y, size.width, size.height, text, path};
    return area;
}
#else
// テスト向けのダミー実装
TouchArea TouchArea::draw(String text, String path, int x, int y, int textSize)
{
    TouchArea area = {x, y, 100, 40, text, path};
    return area;
}
#endif
