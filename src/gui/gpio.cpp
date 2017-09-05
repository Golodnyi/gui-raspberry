#include <iostream>
#if defined(__arm__)
#include <wiringPi.h>
#endif
#include <QtSql/QtSql>
#include <src/struct.h>

extern int PIN;

using namespace std;

int gpio(dataStruct *telemetry_values, int i, bool sound) {
  if (!sound) {
    return 0;
  }
#if defined(__arm__)
  if (wiringPiSetup() == -1) { // Инициализация GPIO
    cout << "not found " << endl;
    return 1; //Завершение программы, если инициализация не удалась
  }

  pinMode(PIN, OUTPUT); // Устанавливаем порт на вывод

  if (telemetry_values[i].color == "red") {
    digitalWrite(PIN, HIGH);
    delay(1000);
    digitalWrite(PIN, LOW);
  } else if (telemetry_values[i].color == "yellow") {
    digitalWrite(PIN, HIGH);
    delay(300);
    digitalWrite(PIN, LOW);
  }

  return (0); // Выход из программы
#else
  return (1);
#endif
}
