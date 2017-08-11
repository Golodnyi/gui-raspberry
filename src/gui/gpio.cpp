#include <iostream>
#include <wiringPi.h>
#include <QtSql/QtSql>
#include <src/struct.h>

int PIN;

using namespace std;

int gpio(dataStruct *telemetry_values, int i){
    QSqlQuery query;
    if (!query.exec(("SELECT port FROM raspberry WHERE funk='speaker'"))) {
        cout << "SQL Query filed: " <<  query.lastError().text().toStdString() << endl;
    }
    while (query.next()) {
        PIN=query.value(0).toInt();
    }

    if (wiringPiSetup() == -1) {    // Инициализация GPIO
        cout << "not found " << endl;
        return 1;                   //Завершение программы, если инициализация не удалась
    }

    pinMode (PIN, OUTPUT) ;     // Устанавливаем порт на вывод

    if (telemetry_values[i].color == "red")
    {
            digitalWrite(PIN, HIGH);
            delay(1000);
            digitalWrite(PIN, LOW);
            delay(1000);
    }
    else{
        digitalWrite(PIN, LOW);
    }

    return (0); // Выход из программы
}


