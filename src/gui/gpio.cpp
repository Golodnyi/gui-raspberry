#include <iostream>
#include <bcm2835.h>

#define PIN RPI_GPIO_J8_36


using namespace std;

int gpio(void){
    unsigned int tone[8] = {956, 851, 758, 716, 638, 568, 506, 478}; // Создаём массив с рассчитанными периодами для каждой ноты

    if (!bcm2835_init()) {    // Инициализация GPIO
        cout << "bcm2835 no found" << endl;
        return 1;
    }           //Завершение программы, если инициализация не удалась

    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);     //Устанавливаем порт Р1_03 на вывод

    int i=8, j;             // Объявляем переменные. i- количество нот, j- продолжительность звучания ноты.

    while (i)  // Играем ноты пока i не равно 0
    {
        bcm2835_delay(200);      // Просто пауза перед проигрыванием каждой ноты
        for (j=600; j!=0; j--)   // 600 раз повторяем колебания для каждой ноты (примерно 1 секунда звучания)
        {
            bcm2835_gpio_write(PIN, LOW);
            delayMicroseconds(tone[i-1]);
            bcm2835_gpio_write(PIN, HIGH);
            delayMicroseconds(tone[i-1]);
        }
        i--;
    }
    return (!bcm2835_close ()); // Выход из программы
}


