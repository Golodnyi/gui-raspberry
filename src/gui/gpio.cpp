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

    /**unsigned int tone = 638; // частота ноты

    if (wiringPiSetup() == -1) {    // Инициализация GPIO
        cout << "not found " << endl;
        return 1;                   //Завершение программы, если инициализация не удалась
    }

    pinMode (PIN, OUTPUT) ;     // Устанавливаем порт на вывод

        for (int j=600; j!=0; j--)   // 600 раз повторяем колебания для каждой ноты (примерно 1 секунда звучания)
        {
            digitalWrite(PIN, 0x0);
            delay(tone);
            digitalWrite(PIN, 0x1);
            delay(tone);
        }

    return (0); // Выход из программы**/
}


