/**
 * @file IndoorBikeMeter.ino
 * @brief エアロバイク用メーター
 * @author Watanabe Toshinori
 * @date 2020/05/24
 */
 
#include <M5Stack.h>
#include <Ticker.h>

// オーディオジャックと接続したPIN
const int PIN = 5;

// ペダル1回転あたりの距離
const float perDistance = 0.006;
// ペダル1回転あたりのカロリー
const float perCalorie = 0.12;

// 最大値
const long maxDuration = 5999000; // 99:59
const float maxDistance = 99.99;
const float maxCalorie = 999.9;

// アイドル状態と判定するまでのミリ秒
const long idleDuration = 3000; // 3 sec

// 現在の計測値
long currentDuration = 0;
float currentDistance = 0;
float currentSpeed = 0;
float currentCalorie = 0;
long currentIdleDuration = idleDuration;

// 現在の表示値
char durationValue[6];
char distanceValue[6];
char calorieValue[6];
char speedValue[6];

// 経過時間計測用
Ticker ticker;

/**
 * M5Stackのセットアップ
 */
void setup() {
  M5.begin();

  // PINをプルアップに設定
  pinMode(PIN, INPUT_PULLUP);

  // 画面表示
  M5.Lcd.setTextColor(WHITE);

  M5.Lcd.fillRoundRect(8, 8, 148, 108, 10, BLUE);
  M5.Lcd.drawString("Duration", 17, 16, 2);
  M5.Lcd.drawString("00:00", 14, 40, 7);

  M5.Lcd.fillRoundRect(164, 8, 148, 108, 10, RED);
  M5.Lcd.drawString("Distance", 173, 16, 2);
  M5.Lcd.drawString("00.00", 170, 40, 7);
  M5.Lcd.drawString("km", 292, 92, 2);

  M5.Lcd.fillRoundRect(8, 124, 148, 108, 10, PINK);
  M5.Lcd.drawString("Speed", 17, 132, 2);
  M5.Lcd.drawString("000.0", 14, 156, 7);
  M5.Lcd.drawString("km/h", 122, 208, 2);

  M5.Lcd.fillRoundRect(164, 124, 148, 108, 10, ORANGE);
  M5.Lcd.drawString("Calorie", 173, 132, 2);
  M5.Lcd.drawString("000.0", 170, 156, 7);
  M5.Lcd.drawString("cal", 287, 208, 2);

  // 経過時間の計測を開始
  ticker.attach(1, updateSecond);

}

/**
 * M5Stackのループ
 */
void loop() {
  if (M5.BtnA.wasPressed()) {
    // Aボタンでリセット
    M5.Power.reset();
  }

  int value = digitalRead(PIN);
  if (value == LOW) {
    // エアロバイクを回転すると通電する

    // アイドル状態をリセット
    currentIdleDuration = 0;

    // 経過時間とカロリーを加算
    currentDistance = min(currentDistance + perDistance, maxDistance);
    currentCalorie = min(currentCalorie + perCalorie, maxCalorie);

    updateDistance();
    updateCalorie();
    
    delay(500);
  }

  // 画面を更新
  updateDuration();
  updateSpeed();

  M5.update();
}

/**
 * 経過時間を加算します
 */
void updateSecond() {
  // アイドル状態でなければ経過時間を加算  
  if (currentIdleDuration < idleDuration) {
    currentDuration += 1000;  
  }

  currentIdleDuration += 1000;
}

/**
 * 経過時間の表示を更新します
 */
void updateDuration() {
  // ミリ秒を分、秒に変換
  int minutes = currentDuration / 1000 / 60;
  int seconds = (currentDuration - (60000 * minutes)) / 1000;

  char buffer[6];
  sprintf(buffer, "%02d:%02d", minutes, seconds);

  if (strcmp(durationValue, buffer) != 0) {
    strncpy(durationValue, buffer, 6);

    M5.Lcd.fillRoundRect(8, 8, 148, 108, 10, BLUE);
    M5.Lcd.drawString("Duration", 17, 16, 2);
    M5.Lcd.drawString(durationValue, 14, 40, 7);
  }  
}

/**
 * 距離の表示を更新します
 */
void updateDistance() {
  char buffer[6];
  sprintf(buffer, "%05.2f", currentDistance);

  if (strcmp(distanceValue, buffer) != 0) {
    strncpy(distanceValue, buffer, 6);

    M5.Lcd.fillRoundRect(164, 8, 148, 108, 10, RED);
    M5.Lcd.drawString("Distance", 173, 16, 2);
    M5.Lcd.drawString(distanceValue, 170, 40, 7);
    M5.Lcd.drawString("km", 292, 92, 2);
  }
}

/**
 * カロリーの表示を更新します
 */
void updateCalorie() {
  char buffer[6];
  sprintf(buffer, "%05.1f", currentCalorie);

  if (strcmp(calorieValue, buffer) != 0) {
    strncpy(calorieValue, buffer, 6);

    M5.Lcd.fillRoundRect(164, 124, 148, 108, 10, ORANGE);
    M5.Lcd.drawString("Calorie", 173, 132, 2);
    M5.Lcd.drawString(calorieValue, 170, 156, 7);
    M5.Lcd.drawString("cal", 287, 208, 2);
  }
}

/**
 * 速度の表示を更新します
 */
void updateSpeed() {
  char buffer[6];

  if (currentIdleDuration < idleDuration) {
    // 時速を算出
    int seconds = currentDuration/ 1000;
    currentSpeed = (currentDistance / seconds) * 3600;
    sprintf(buffer, "%05.1f", currentSpeed);
  } else {
    // アイドル状態なら000.0固定
    sprintf(buffer, "000.0");
  }

  if (strcmp(speedValue, buffer) != 0) {
    strncpy(speedValue, buffer, 6);

    M5.Lcd.fillRoundRect(8, 124, 148, 108, 10, PINK);
    M5.Lcd.drawString("Speed", 17, 132, 2);
    M5.Lcd.drawString(speedValue, 14, 156, 7);
    M5.Lcd.drawString("km/h", 122, 208, 2);
  }  
}
