#include <iostream>
#include <wiringPi.h>
#include <QtSql/QtSql>

int PIN;

using namespace std;

int gpio(){
    cout << "gpio start"<< endl;
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

            digitalWrite(PIN, LOW);
            delay(638);
            digitalWrite(PIN, HIGH);
            delay(638);

    return (0); // Выход из программы
}


