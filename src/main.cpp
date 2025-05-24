#include <SD.h>
#include <M5Unified.h>
#include <M5Helper.h>
#include <Pressable.h>

// フォントサイズの定義
#define FONT_SIZE_SPACER 2
#define FONT_SIZE_REGULAR 4
#define FONT_SIZE_FILE 8

// 対応する画像ファイル数（ファイル名の表示エリアと描画に時間がかかるので、大きな値は設定しない）
#define MAX_IMAGES 10

// ボタン群
int buttonCount = 0;
Pressable buttonList[MAX_IMAGES];

// ボタンイベント
void onPress(Pressable &button)
{
  int index = button.tag;
  String path = button.userInfo;
  if (0 <= index && index < buttonCount)
  {
    M5Helper::drawImageFromSD(path, M5Helper::Rotation::Down, true);
  }
}

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  // スリープから起きる。実機の電源ボタンを押してください。
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  M5.Log(esp_log_level_t::ESP_LOG_INFO, "Wakeup reason: %d\n", wakeup_reason);

  M5.Lcd.setEpdMode(epd_mode_t::epd_fastest);
  M5.Display.setTextSize(FONT_SIZE_REGULAR);
  M5.Display.println("CardCase For M5PaperS3");

  // Get SPI pins
  auto mosi = M5.getPin(m5::pin_name_t::sd_spi_mosi);
  auto miso = M5.getPin(m5::pin_name_t::sd_spi_miso);
  auto sclk = M5.getPin(m5::pin_name_t::sd_spi_sclk);
  auto cs = M5.getPin(m5::pin_name_t::sd_spi_cs);

  // SD の初期化
  SPI.begin(sclk, miso, mosi);
  if (!SD.begin(cs, SPI, 4000000))
  {
    M5.Display.println("mount SD card .. NG");
    while (1)
      ;
  }
  else
  {
    M5.Display.println("mount SD card .. OK");
  }

  // SD のルートディレクトリを開く
  File root = SD.open("/");
  if (!root)
  {
    M5.Display.println("failed to open SD card");
    while (1)
      ;
  }
  else
  {
    M5.Display.setTextSize(FONT_SIZE_SPACER);
    M5.Display.println("");
  }

  // SD を走査して画像ファイルをリストアップ
  File file = root.openNextFile();
  while (file && buttonCount < MAX_IMAGES)
  {
    if (!file.isDirectory())
    {
      String filename = file.name();
      if (!filename.startsWith(".") && (filename.endsWith(".jpg") || filename.endsWith(".jpeg") || filename.endsWith(".png")))
      {
        String path = String("/") + filename;
        String text = String(filename);

        Pressable button = Pressable();
        button.tag = buttonCount;
        button.userInfo = path;
        button.show(text, onPress);

        buttonList[buttonCount] = button;
        buttonCount++;

        M5.Display.setTextSize(FONT_SIZE_SPACER);
        M5.Display.println("");
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  // 読込み完了
  M5.Display.setTextSize(FONT_SIZE_REGULAR);
  if (buttonCount > 0)
  {
    M5.Display.println("Touch file name!");
  }
  else
  {
    M5.Display.println("It does not found images.");
    while (1)
      ;
  }
}

void loop()
{
  M5.update();

  for (Pressable button : buttonList)
  {
    button.loop();
  }
}
