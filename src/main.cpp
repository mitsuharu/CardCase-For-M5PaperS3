#include <SD.h>
#include <epdiy.h>
#include <M5Unified.h>

#define FONT_SIZE_SPACER 2
#define FONT_SIZE_REGULAR 4
#define FONT_SIZE_FILE 10

#define MAX_IMAGES 10 // 画像ファイルの最大数

struct TouchArea
{
  int x, y, w, h;
  String text;
  String path;

  bool contains(int tx, int ty)
  {
    return (tx >= x && tx <= x + w && ty >= y && ty <= y + h);
  }
};

TouchArea drawTouchArea(String text, String path, int x, int y, int textSize = 6, uint16_t fgColor = TFT_BLACK, uint16_t bgColor = TFT_WHITE)
{
  M5.Lcd.setTextSize(textSize);
  M5.Lcd.setTextColor(fgColor, bgColor);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextWrap(false);
  M5.Lcd.println(text);
  Serial.println(text);
  M5.Lcd.setTextWrap(true);

  int w = M5.Lcd.width();
  int h = M5.Lcd.fontHeight();

  TouchArea area = {x, y, w, h, text, path};
  return area;
}

int findTouchedAreaIndex(int tx, int ty, TouchArea *list, int count)
{
  for (int i = 0; i < count; i++)
  {
    if (list[i].contains(tx, ty))
    {
      return i; // 一致したインデックスを返す
    }
  }
  return -1; // 該当なし
};

TouchArea touchAreaList[MAX_IMAGES];
int touchAreaCount = 0; // 見つかった画像の数
int yTouchArea = 0;     // TODO: 計算する

// SDカード内の画像ファイルを描画する関数
void displayImage(String filename)
{
  M5.Lcd.fillScreen(0xFFFFFF);
  M5.Lcd.setRotation(2);

  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg"))
  {
    M5.Lcd.drawJpgFile(SD, filename, 0, 0);
  }
  else if (filename.endsWith(".png"))
  {
    M5.Lcd.drawPngFile(SD, filename, 0, 0);
  }

  // // Enter into deep sleep
  // M5.Log(esp_log_level_t::ESP_LOG_INFO, "Deep sleep start\n");
  // M5.Power.deepSleep();
}

void setup()
{
  // Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg);

  // // Get wakeup reason
  // esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  // M5.Log(esp_log_level_t::ESP_LOG_INFO, "Wakeup reason: %d\n", wakeup_reason);

  yTouchArea += drawTouchArea("CardCase For M5PaperS3", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;

  // Get SPI pins
  auto mosi = M5.getPin(m5::pin_name_t::sd_spi_mosi);
  auto miso = M5.getPin(m5::pin_name_t::sd_spi_miso);
  auto sclk = M5.getPin(m5::pin_name_t::sd_spi_sclk);
  auto cs = M5.getPin(m5::pin_name_t::sd_spi_cs);

  // SD の初期化
  SPI.begin(sclk, miso, mosi);
  M5.Lcd.setTextSize(FONT_SIZE_REGULAR);
  if (!SD.begin(cs, SPI, 4000000))
  {
    yTouchArea += drawTouchArea("SD card init .. NG", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
  else
  {
    yTouchArea += drawTouchArea("SD card init .. OK", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }

  // SD のルートディレクトリを走査して画像ファイルをリストアップ
  File root = SD.open("/");
  if (!root)
  {
    yTouchArea += drawTouchArea("failed to open SD card", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
  else
  {
    yTouchArea += drawTouchArea("", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }

  File file = root.openNextFile();
  while (file && touchAreaCount < MAX_IMAGES)
  {
    if (!file.isDirectory())
    {
      String filename = file.name();
      if (!filename.startsWith(".") && (filename.endsWith(".jpg") || filename.endsWith(".jpeg") || filename.endsWith(".png")))
      {
        String path = String("/") + filename;
        String text = String(filename);

        TouchArea area = drawTouchArea(text, path, 0, yTouchArea, FONT_SIZE_FILE);
        yTouchArea += area.h;

        touchAreaList[touchAreaCount] = area;
        touchAreaCount++;

        yTouchArea += drawTouchArea("", "", 0, yTouchArea, FONT_SIZE_SPACER).h;
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  yTouchArea += drawTouchArea("", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  if (touchAreaCount > 0)
  {
    yTouchArea += drawTouchArea("Touch file name!", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }
  else
  {
    yTouchArea += drawTouchArea("It does not found images.", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
}

void loop()
{
  M5.update();

  // 画面タップで画像を選択
  auto t = M5.Touch.getDetail();
  if (t.wasPressed())
  {
    int index = findTouchedAreaIndex(t.x, t.y, touchAreaList, touchAreaCount);
    if (0 <= index && index < touchAreaCount)
    {
      TouchArea area = touchAreaList[index];
      displayImage(area.path);
    }
  }
}
