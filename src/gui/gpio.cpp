#include <iostream>
#include <pigpio.h>

//pin

using namespace std;

int gpio(void){
    cout << "gpio start"<< endl;
    unsigned int tone[8] = {956, 851, 758, 716, 638, 568, 506, 478}; // Создаём массив с рассчитанными периодами для каждой ноты

    if (!gpioInitialise()) {    // Инициализация GPIO
        cout << "bcm2835 no found" << endl;
        return 1;
    }           //Завершение программы, если инициализация не удалась

    gpioRead(16);     // Устанавливаем порт на вывод

    int i=8, j;             // Объявляем переменные. i- количество нот, j- продолжительность звучания ноты.

    while (i)  // Играем ноты пока i не равно 0
    {
        gpioDelay(200);      // Просто пауза перед проигрыванием каждой ноты
        for (j=600; j!=0; j--)   // 600 раз повторяем колебания для каждой ноты (примерно 1 секунда звучания)
        {
            gpioWrite(16, 0x0);
            gpioDelay(tone[i-1]);
            gpioWrite(16, 0x1);
            gpioDelay(tone[i-1]);
        }
        i--;
    }
    return (!gpioSerialReadClose); // Выход из программы
}


