#include <iostream>
#if defined(__arm__)
#include <wiringPi.h>
#endif
#include <QtSql/QtSql>
#include <src/struct.h>

extern int PIN;

using namespace std;

void *warning(void *arg)
{
#if defined(__arm__)
  if (wiringPiSetup() == -1)
  { // Инициализация GPIO
    cout << "not found " << endl;
  }

  pinMode(PIN, OUTPUT); // Устанавливаем порт на вывод

  digitalWrite(PIN, HIGH);
  delay(300);
  digitalWrite(PIN, LOW);
#endif
}

void *danger(void *arg)
{
#if defined(__arm__)
  if (wiringPiSetup() == -1)
  { // Инициализация GPIO
    cout << "not found " << endl;
  }

  pinMode(PIN, OUTPUT); // Устанавливаем порт на вывод

  digitalWrite(PIN, HIGH);
  delay(1000);
  digitalWrite(PIN, LOW);
#endif
}
