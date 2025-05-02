#include <SD.h>
#include <M5Unified.h>

#include <M5Helper.h>
#include <TouchArea.h>

// フォントサイズの定義
#define FONT_SIZE_SPACER 2
#define FONT_SIZE_REGULAR 4
#define FONT_SIZE_FILE 8

// 対応する画像ファイル数（ファイル名の表示エリアと描画に時間がかかるので、大きな値は設定しない）
#define MAX_IMAGES 10

TouchArea touchAreaList[MAX_IMAGES];
int touchAreaCount = 0; // 見つかった画像の数
int yTouchArea = 0;     // TouchArea を描画するために、高さを記録する

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  // スリープから起きる。実機の電源ボタンを押してください。
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  M5.Log(esp_log_level_t::ESP_LOG_INFO, "Wakeup reason: %d\n", wakeup_reason);

  yTouchArea += TouchArea::draw("CardCase For M5PaperS3", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;

  // Get SPI pins
  auto mosi = M5.getPin(m5::pin_name_t::sd_spi_mosi);
  auto miso = M5.getPin(m5::pin_name_t::sd_spi_miso);
  auto sclk = M5.getPin(m5::pin_name_t::sd_spi_sclk);
  auto cs = M5.getPin(m5::pin_name_t::sd_spi_cs);

  // SD の初期化
  SPI.begin(sclk, miso, mosi);
  if (!SD.begin(cs, SPI, 4000000))
  {
    yTouchArea += TouchArea::draw("mount SD card .. NG", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
  else
  {
    yTouchArea += TouchArea::draw("mount SD card .. OK", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }

  // SD のルートディレクトリを開く
  File root = SD.open("/");
  if (!root)
  {
    yTouchArea += TouchArea::draw("failed to open SD card", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
    while (1)
      ;
  }
  else
  {
    yTouchArea += TouchArea::draw("", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
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

        TouchArea area = TouchArea::draw(text, path, 0, yTouchArea, FONT_SIZE_FILE);
        yTouchArea += area.h;

        touchAreaList[touchAreaCount] = area;
        touchAreaCount++;

        yTouchArea += TouchArea::draw("", "", 0, yTouchArea, FONT_SIZE_SPACER).h;
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  // 読込み完了
  yTouchArea += TouchArea::draw("", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  if (touchAreaCount > 0)
  {
    yTouchArea += TouchArea::draw("Touch file name!", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
  }
  else
  {
    yTouchArea += TouchArea::draw("It does not found images.", "", 0, yTouchArea, FONT_SIZE_REGULAR).h;
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
    int index = TouchArea::findIndex(t.x, t.y, touchAreaList, touchAreaCount);
    if (0 <= index && index < touchAreaCount)
    {
      TouchArea area = touchAreaList[index];
      M5Helper::drawImageFromSD(area.path, M5Helper::Rotation::Down, true);
    }
  }
}
