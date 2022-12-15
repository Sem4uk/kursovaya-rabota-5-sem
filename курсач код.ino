// библиотека для работы с датчиками MQ (Troyka-модуль)
#include <TroykaMQ.h>
// имя для пина, к которому подключен датчик
#define PIN_MQ2         A0
// имя для пина, к которому подключен нагреватель датчика
#define PIN_MQ2_HEATER  13
 
// создаём объект для работы с датчиком
// и передаём ему номер пина выходного сигнала и нагревателя
MQ2 mq2(PIN_MQ2, PIN_MQ2_HEATER);
 
// номер цифрового пина зелёного светодиода
#define LED_GREEN_PIN  9
// номер цифрового пина красного светодиода
#define LED_RED_PIN    A3
// номер цифрового пина пищалки
#define BUZZER_PIN     4
 
void setup(){
  // открываем последовательный порт
  Serial.begin(9600);
  // назначаем 2 пина в режиме выхода
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  // включаем нагреватель
  mq2.heaterPwrHigh();
  Serial.println("Heated sensor");
}
 
void loop(){
  // если прошёл интервал нагрева датчика
  // и калибровка не была совершена
  if (!mq2.isCalibrated() && mq2.heatingCompleted()) {
    // выполняем калибровку датчика на чистом воздухе
    mq2.calibrate();
    // выводим сопротивление датчика в чистом воздухе (Ro) в serial-порт
    Serial.print("Ro = ");
    Serial.println(mq2.getRo());
  }
  // если прошёл интервал нагрева датчика
  // и калибровка была совершена
  if (mq2.isCalibrated() && mq2.heatingCompleted()) {
      // выводим отношения текущего сопротивление датчика
    // к сопротивлению датчика в чистом воздухе (Rs/Ro)
    Serial.print("Ratio: ");
    Serial.print(mq2.readRatio());
    // выводим значения газов в ppm
    Serial.print("LPG: ");
    Serial.print(mq2.readLPG());
    Serial.print(" ppm ");
    Serial.print(" Methane: ");
    Serial.print(mq2.readMethane());
    Serial.print(" ppm ");
    Serial.print(" Smoke: ");
    Serial.print(mq2.readSmoke());
    Serial.print(" ppm ");
    Serial.print(" Hydrogen: ");
    Serial.print(mq2.readHydrogen());
    Serial.println(" ppm ");
    delay(100);
    // считываем показания вредных газов
    float valueLPG = mq2.readLPG();
    // если показания превышают пороговое значение
    if (mq2.readLPG() > 500) {
      // включаем предупреждающий сигнал и мигаем красным светодиодом
      signaling(100);
      // если показания сильно превышают предельную норму
    } else if (mq2.readLPG() > 100) {
      // включаем сирену и быстро мигаем красным светодиодом
      signaling(300);
    } else {
      // гасим красный светодиода и зажигаем зелёный
      digitalWrite(LED_GREEN_PIN, HIGH);
      digitalWrite(LED_RED_PIN, LOW);
    }
  }
}
 
// функция сигнализации
void signaling(int frequency)
{
  // гасим зелёный светодиод
  digitalWrite(LED_GREEN_PIN, LOW);
  // в зависимости от переданной переменной
  // пищим бузером и мигаем красным светодиодом
  digitalWrite(LED_RED_PIN, HIGH);
  tone(BUZZER_PIN, 300, frequency);
  delay(frequency);
  digitalWrite(LED_RED_PIN, LOW);
  delay(frequency);
}