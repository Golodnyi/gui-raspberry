#include <iostream>
#include <wiringPi.h>

#define PIN 27

using namespace std;

int gpio(){
    cout << "gpio start"<< endl;
    unsigned int tone[8] = {956, 851, 758, 716, 638, 568, 506, 478}; // Создаём массив с рассчитанными периодами для каждой ноты

    if (wiringPiSetup() == -1) {    // Инициализация GPIO
        cout << "not found " << endl;
        return 1;
    }           //Завершение программы, если инициализация не удалась

    pinMode (PIN, OUTPUT) ;     // Устанавливаем порт на вывод

    int i=8, j;             // Объявляем переменные. i- количество нот, j- продолжительность звучания ноты.

    while (i)  // Играем ноты пока i не равно 0
    {
        delay(200);      // Просто пауза перед проигрыванием каждой ноты
        for (j=600; j!=0; j--)   // 600 раз повторяем колебания для каждой ноты (примерно 1 секунда звучания)
        {
            digitalWrite(PIN, 0x0);
            delay(tone[i-1]);
            digitalWrite(PIN, 0x1);
            delay(tone[i-1]);
        }
        i--;
    }
    return (0); // Выход из программы
}


