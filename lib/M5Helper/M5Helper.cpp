#include "./M5Helper.h"

#ifdef ARDUINO

#include <SD.h>
#include <epdiy.h>
#include <M5Unified.h>

uint16_t convertColor(M5Helper::Color color)
{
  if (color == M5Helper::Color::black)
  {
    return TFT_BLACK;
  }
  else
  {
    return TFT_WHITE;
  }
}

// SDカード内の画像ファイルを描画する関数
void M5Helper::drawImageFromSD(String path, Rotation rotation, bool shouldSleepAfterDraw)
{
  M5.Lcd.fillScreen(0xFFFFFF);
  M5.Lcd.setRotation(static_cast<uint_fast8_t>(rotation));
  M5.Lcd.setEpdMode(epd_mode_t::epd_quality);

  if (path.endsWith(".jpg") || path.endsWith(".jpeg"))
  {
    M5.Lcd.drawJpgFile(SD, path, 0, 0);
  }
  else if (path.endsWith(".png"))
  {
    M5.Lcd.drawPngFile(SD, path, 0, 0);
  }

  // 引数により画像を描画した後にスリープに入る
  if (shouldSleepAfterDraw)
  {
    // 電池のためスリープに入る。再び画像選択したい場合は電源ボタンを押す。
    M5.Log(esp_log_level_t::ESP_LOG_INFO, "Deep sleep start\n");
    M5.Power.deepSleep();
  }
}

M5Helper::Size M5Helper::drawText(String text, int x, int y, int textSize, Color fontColor, Color bgColor)
{
  M5.Lcd.setTextSize(textSize);
  M5.Lcd.setTextColor(convertColor(fontColor), convertColor(bgColor));
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextWrap(false);
  M5.Lcd.setEpdMode(epd_mode_t::epd_fastest);
  M5.Lcd.println(text);
  // Serial.println(text);
  M5.Lcd.setTextWrap(true);

  int width = M5.Lcd.width();
  int height = M5.Lcd.fontHeight();

  M5Helper::Size size = {width, height};
  return size;
}

#endif
