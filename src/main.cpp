#include <SD.h>
#include <epdiy.h>
#include <M5Unified.h>

// フォントサイズの定義
#define FONT_SIZE_SPACER 2
#define FONT_SIZE_REGULAR 4
#define FONT_SIZE_FILE 10

// 対応する画像ファイル数（ファイル名の表示エリアと描画に時間がかかるので、大きな値は設定しない）
#define MAX_IMAGES 10

// タッチしたエリアを判定するための構造体
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

// TouchAreaの作成と表示を同時にする（TouchAreaのクラス関数的に書くと良いが、C++分からん）
TouchArea drawTouchArea(String text, String path, int x, int y, int textSize = 6, uint16_t fgColor = TFT_BLACK, uint16_t bgColor = TFT_WHITE)
{
  M5.Lcd.setTextSize(textSize);
  M5.Lcd.setTextColor(fgColor, bgColor);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextWrap(false);
  M5.Lcd.setEpdMode(epd_mode_t::epd_fastest);
  M5.Lcd.println(text);
  // Serial.println(text);
  M5.Lcd.setTextWrap(true);

  int w = M5.Lcd.width();
  int h = M5.Lcd.fontHeight();

  TouchArea area = {x, y, w, h, text, path};
  return area;
}

// 引数の座標が、同じく引数で与えたTouchAreaのどれにあたるか調べる
int findTouchedAreaIndex(int tx, int ty, TouchArea *list, int count)
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

TouchArea touchAreaList[MAX_IMAGES];
int touchAreaCount = 0; // 見つかった画像の数
int yTouchArea = 0;     // TouchArea を描画するために、高さを記録する

// SDカード内の画像ファイルを描画する関数
void displayImage(String filename)
{
  M5.Lcd.fillScreen(0xFFFFFF);
  M5.Lcd.setRotation(2);

  M5.Lcd.setEpdMode(epd_mode_t::epd_quality);
  if (filename.endsWith(".jpg") || filename.endsWith(".jpeg"))
  {
    M5.Lcd.drawJpgFile(SD, filename, 0, 0);
  }
  else if (filename.endsWith(".png"))
  {
    M5.Lcd.drawPngFile(SD, filename, 0, 0);
  }

  // 電池のためスリープに入る。再び画像選択したい場合は電源ボタンを押す。
  M5.Log(esp_log_level_t::ESP_LOG_INFO, "Deep sleep start\n");
  M5.Power.deepSleep();
}

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  // スリープから起きる。実機の電源ボタンを押してください。
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  M5.Log(esp_log_level_t::ESP_LOG_INFO, "Wakeup reason: %d\n", wakeup_reason);

  yTouchArea += drawTouchArea("CardCase For M5PaperS3", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;

  // Get SPI pins
  auto mosi = M5.getPin(m5::pin_name_t::sd_spi_mosi);
  auto miso = M5.getPin(m5::pin_name_t::sd_spi_miso);
  auto sclk = M5.getPin(m5::pin_name_t::sd_spi_sclk);
  auto cs = M5.getPin(m5::pin_name_t::sd_spi_cs);

  // SD の初期化
  SPI.begin(sclk, miso, mosi);
  if (!SD.begin(cs, SPI, 4000000))
  {
    yTouchArea += drawTouchArea("mount SD card .. NG", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
  else
  {
    yTouchArea += drawTouchArea("mount SD card .. OK", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }

  // SD のルートディレクトリを開く
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

  // SD を走査して画像ファイルをリストアップ
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

  // 読込み完了
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
