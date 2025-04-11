#pragma once

#include <Arduino.h>

/**
 * タッチしたエリアを判定するための構造体
 */
struct TouchArea
{
    /// @brief 座標系
    int x, y, w, h;

    /// @brief 表示されるテキスト
    String text;

    /// @brief このアプリで利用するファイルパス
    String path;

    /**
     * 引数の座標が、インスタンスが定義する座標内に収まっているか判定する
     */
    bool contains(int tx, int ty);

    /**
     * TouchArea を生成して、テキストを表示する
     */
    static TouchArea draw(String text, String path, int x, int y, int textSize);

    /**
     * 引数で指定する TouchArea 配列から一致する座標を持つ配列番号を取得する
     */
    static int findIndex(int tx, int ty, TouchArea *list, int count);
};
