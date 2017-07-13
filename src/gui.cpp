#include <iostream>
#include <vector>
#include <QApplication>
#include <QtGui>
#include <QTableWidget>
#include <pthread.h>
#include <unistd.h>

#define ROWS 10 // количество строк
#define COLS 10 // количество столбцов таблицы
using namespace std;
struct dataStruct {
    string alias; // англ название (numPage)
    short int byte;
    string type;
    string name; // название датчика на русском (температура блока цилиндров)
    string unit; // мера измерения (км в час, цельясия...)
    bool enable; // значение из сокета
    string value; // значение из сокета

};
extern void * flex(void *arg);
void * draw(void *arg) {
    dataStruct* telemetry_values = (dataStruct *)arg;
    while (true) {
        for (int i = 0; i < 85; i++) {
            if (telemetry_values[i].enable) {
                cout << telemetry_values[i].value << endl;
            }
        }
        usleep(1000000);
    }
}
int gui_init(int argc, char *argv[]) {
    dataStruct telemetry_values[85];
    telemetry_values[0] = {"numPage", 4, "U32", "название", "км/ч", false}; // 1 id записи в черном ящике
    telemetry_values[1] = {"Code", 2, "U16", "название", "км/ч", false}; // 2 код события
    telemetry_values[2] = {"Time", 4, "U32", "название", "км/ч", false}; // 3 время события
    telemetry_values[3] = {"State", 1, "U8", "название", "км/ч", false}; // 4 статус устройства
    telemetry_values[4] = {"Module1", 1, "U8", "название", "км/ч", false}; // 5 статус функциональных модулей 1
    telemetry_values[5] = {"Module2", 1, "U8", "название", "км/ч", false}; // 6 статус функциональных модулей 2
    telemetry_values[6] = {"GSM", 1, "U8", "название", "км/ч",}; // 7 уровень gsm
    telemetry_values[7] = {"StateGauge", 1, "U8", "название", "км/ч", false}; // 8 состояние навигационного датчика GPS/Глонасс
    telemetry_values[8] = {"LastTime", 4, "U32", "название", "км/ч", false}; // 9 время последних валидных координат
    telemetry_values[9] = {"Lat", 4, "I32", "название", "км/ч", false}; // 10 последняя валидная широта
    telemetry_values[10] = {"Lon", 4, "I32", "название", "км/ч", false}; // 11 долгота
    telemetry_values[11] = {"Alt", 4, "I32", "название", "км/ч", false}; // 12 высота
    telemetry_values[12] = {"Speed", 4, "Float", "название", "км/ч", false}; // 13 скорость
    telemetry_values[13] = {"Course", 2, "U16", "название", "км/ч", false};  // 14 курс
    telemetry_values[14] = {"Mileage", 4, "Float", "название", "км/ч", false}; // 15 текущий пробег
    telemetry_values[15] = {"Way", 4, "Float", "название", "км/ч", false};  // 16 последний отрезок пути
    telemetry_values[16] = {"AllSeconds", 2, "U16", "название", "км/ч", false}; // 17 общее кол-во сек на последнем отрезке
    telemetry_values[17] = {"SecondLast", 2, "U16", "название", "км/ч", false}; // 18 Количество секунд на последнем отрезке пути, по которым вычислялся пробег
    telemetry_values[18] = {"Power", 2, "U16", "название", "км/ч", false}; // 19 Напряжение на основном источнике питания
    telemetry_values[19] = {"Reserv", 2, "U16", "название", "км/ч", false}; // 20 Напряжение на резервном источнике питания
    telemetry_values[20] = {"StateU_Ain1", 2, "U16", "название", "км/ч", false}; // 21 Напряжение на аналоговом входе 1 (Ain1)
    telemetry_values[21] = {"StateU_Ain2", 2, "U16", "название", "км/ч", false}; // 22 на входе 2 (Ain2)
    telemetry_values[22] = {"StateU_Ain3", 2, "U16", "название", "км/ч", false}; // 23 на входе 3 (Ain3)
    telemetry_values[23] = {"StateU_Ain4", 2, "U16", "название", "км/ч", false}; // 24 на входе 4 (Ain4)
    telemetry_values[24] = {"StateU_Ain5", 2, "U16", "название", "км/ч", false}; // 25 на входе 5 (Ain5)
    telemetry_values[25] = {"StateU_Ain6", 2, "U16", "название", "км/ч", false}; // 26 на входе 6 (Ain6)
    telemetry_values[26] = {"StateU_Ain7", 2, "U16", "название", "км/ч", false}; // 27 на входе 7 (Ain7)
    telemetry_values[27] = {"StateU_Ain8", 2, "U16", "название", "км/ч", false}; // 28 на входе 8 (Ain8)
    telemetry_values[28] = {"StateIn1", 1, "U8", "название", "км/ч", false}; // 29 Текущие показания дискретных датчиков 1
    telemetry_values[29] = {"StateIn2", 1, "U8", "название", "км/ч", false}; // 30  датчиков 2
    telemetry_values[30] = {"stateOut1", 1, "U8", "название", "км/ч", false}; // 31 Текущее состояние выходов 1
    telemetry_values[31] = {"stateOut2", 1, "U8", "название", "км/ч", false}; // 32  выходов 2
    telemetry_values[32] = {"StateInImp1", 4, "U32", "название", "км/ч", false}; // 33 Показания счетчика импульсов 1
    telemetry_values[33] = {"StateInImp2", 4, "U32", "название", "км/ч", false}; // 34 2
    telemetry_values[34] = {"Frequency1", 2, "U16", "название", "км/ч", false}; // 35 Частота на аналогово-частотном датчике уровня топлива 1
    telemetry_values[35] = {"Frequency2", 2, "U16", "название", "км/ч", false}; // 36 датчике  2
    telemetry_values[36] = {"Motochas", 4, "U32", "название", "км/ч", false}; // 37 Моточасы, подсчитанные во время срабатывания датчика работы генератора
    telemetry_values[37] = {"LevelRS485_1", 2, "U16", "название", "км/ч", false}; // 38 Уровень топлива, измеренный датчиком уровня топлива 1 RS - 485
    telemetry_values[38] = {"LevelRS485_2", 2, "U16", "название", "км/ч", false}; // 39 Уровень топлива 2
    telemetry_values[39] = {"LevelRS485_3", 2, "U16", "название", "км/ч", false}; // 40 Уровень топлива 3
    telemetry_values[40] = {"LevelRS485_4", 2, "U16", "название", "км/ч", false}; // 41 Уровень топлива 4
    telemetry_values[41] = {"LevelRS485_5", 2, "U16", "название", "км/ч", false}; // 42 Уровень топлива 5
    telemetry_values[42] = {"LevelRS485_6", 2, "U16", "название", "км/ч", false}; // 43 Уровень топлива 6
    telemetry_values[43] = {"LevelRS232", 2, "U16", "название", "км/ч", false}; // 44 Уровень топлива, измеренный датчиком уровня топлива RS - 232
    telemetry_values[44] = {"Temp1", 1, "I8", "название", "км/ч", false}; // 45 датчика 1 (в градусах Цельсия)
    telemetry_values[45] = {"Temp2", 1, "I8", "название", "км/ч", false}; // 46 датчика 2 (в градусах Цельсия)
    telemetry_values[46] = {"Temp3", 1, "I8", "название", "км/ч", false}; // 47 датчика 3 (в градусах Цельсия)
    telemetry_values[47] = {"Temp4", 1, "I8", "название", "км/ч", false}; // 48 датчика 4 (в градусах Цельсия)
    telemetry_values[48] = {"Temp5", 1, "I8", "название", "км/ч", false}; // 49 датчика 5 (в градусах Цельсия)
    telemetry_values[49] = {"Temp6", 1, "I8", "название", "км/ч", false}; // 50 датчика 6 (в градусах Цельсия)
    telemetry_values[50] = {"Temp7", 1, "I8", "название", "км/ч", false}; // 51 датчика 7 (в градусах Цельсия)
    telemetry_values[51] = {"Temp8", 1, "I8", "название", "км/ч", false}; // 52 датчика 8
    telemetry_values[52] = {"CAN_FuelLevel", 2, "U16", "название", "км/ч", false}; // 53 Уровень топлива в баке
    telemetry_values[53] = {"CAN_FuelConsumption", 4, "Float", "название", "км/ч", false}; // 54 Полный расход топлива(флоат)
    telemetry_values[54] = {"CAN_EngineTurns", 2, "U16", "название", "км/ч", false}; // 55 Обороты двигателя "CAN_Temp"
    telemetry_values[55] = {"CAN_Temp", 1, "I8", "название", "км/ч", false}; // 56 Температура охлаждающей жидкости (двигателя)
    telemetry_values[56] = {"CAN_FullRun", 4, "Float", "название", "км/ч", false}; // 57 Полный пробег ТС
    telemetry_values[57] = {"CAN_AxleLoad_1", 2, "U16", "название", "км/ч", false}; // 58 Нагрузка на ось 1
    telemetry_values[58] = {"CAN_AxleLoad_2", 2, "U16", "название", "км/ч", false}; // 59 Нагрузка на ось 2
    telemetry_values[59] = {"CAN_AxleLoad_3", 2, "U16", "название", "км/ч", false}; // 60 Нагрузка на ось 3
    telemetry_values[60] = {"CAN_AxleLoad_4", 2, "U16", "название", "км/ч", false}; // 61 Нагрузка на ось 4
    telemetry_values[61] = {"CAN_AxleLoad_5", 2, "U16", "название", "км/ч", false}; // 62 Нагрузка на ось 5
    telemetry_values[62] = {"CAN_PedalAccel", 1, "U8", "название", "км/ч", false}; // 63 Положение педали газа
    telemetry_values[63] = {"CAN_PedalStop", 1, "U8", "название", "км/ч", false}; // 64  Положение педали тормоза
    telemetry_values[64] = {"CAN_EngineLoad", 1, "U8", "название", "км/ч", false}; // 65  Нагрузка на двигатель
    telemetry_values[65] = {"CAN_LevelFiltr", 2, "U16", "название", "км/ч", false}; // 66  Уровень жидкости в дизельном фильтре выхлопных газов
    telemetry_values[66] = {"CAN_EngineTime", 4, "U32", "название", "км/ч", false}; // 67 Полное время работы двигателя
    telemetry_values[67] = {"CAN_TimeTO", 2, "I16", "название", "км/ч", false};  // 68 Расстояние до ТО
    telemetry_values[68] = {"CAN_Speed", 1, "U8", "название", "км/ч", false}; // 69 Скорость ТС
    telemetry_values[69] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[70] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[71] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[72] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[73] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[74] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[75] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[76] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[77] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[78] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[79] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[80] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[81] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[82] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[83] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    telemetry_values[84] = {"ATemp1", 2, "U16", "название", "км/ч", false};
    QApplication app(argc, argv); //(постоянная) приложение
    QTableWidget table(ROWS, COLS);
    table.setGridStyle(Qt::SolidLine);
    table.setFont(QFont("Times", 14, QFont::Normal));

    pthread_t thread;
    int result = pthread_create(&thread, NULL, flex, &telemetry_values);
    if (result != 0) {
        perror("Создание первого потока!");
        return EXIT_FAILURE;
    }

    pthread_t thread2;
    int result2 = pthread_create(&thread2, NULL, draw, &telemetry_values);
    if (result2 != 0) {
        perror("Создание второго потока!");
        return EXIT_FAILURE;
    }


    QTableWidgetItem *item = new QTableWidgetItem;

    for (int i = 0; i < COLS; i++) {
        item->setText(QString(QChar('A' + i)));
        table.setHorizontalHeaderItem(i, item);
        for (int j = 0; j < ROWS; j++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString("(%1, %2)")
                                  .arg(QChar('A' + i)).arg(j + 1));
            table.setItem(j, i, item);
        }
    }
    table.showMaximized();
    return app.exec();
}