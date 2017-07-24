#include <iostream>
#include <QApplication>
#include <QtGui>
#include <QListWidget>
#include <QGridLayout>

using namespace std;

QListWidget *listWidget;
QListWidget *listWidget1;
struct dataStruct {
    QString alias; // англ название (numPage)
    int byte;
    QString type;
    QString name; // название датчика на русском (температура блока цилиндров)
    QString unit; // мера измерения (км в час, цельясия...)
    bool enable; // значение из сокета
    QString value; // значение из сокета

};

extern void * flex(void *arg);
extern void close_socket();
extern void connect();
extern dataStruct getTelemetry(dataStruct *telemetry_values);


void * update(void *arg) {
    dataStruct* telemetry_values = (dataStruct *)arg;
    cout << "update" << endl;
    short int x=0;
    short int y=0;
    listWidget->clear();
    listWidget1->clear();
    for (int i = 0; i < 43; i++) {
        if (telemetry_values[i].enable) {
            x=listWidget->count();
            y=listWidget1->count();
            if(x>y){
                QListWidgetItem *Item1 = new QListWidgetItem;
                Item1->setText((telemetry_values[i].name)
                                  + ": " + (telemetry_values[i].value)
                                  + " " + (telemetry_values[i].unit));

                listWidget1->insertItem(i, Item1);
                if (i == 2) {
                    Item1->setBackground(Qt::red);
                } else {
                    Item1->setBackground(Qt::green);
                }
            }
            else {
                QListWidgetItem *Item = new QListWidgetItem;
                Item->setText((telemetry_values[i].name)
                              + ": " + (telemetry_values[i].value)
                              + " " + (telemetry_values[i].unit));

                listWidget->insertItem(i, Item);
                if (i == 2) {
                    Item->setBackground(Qt::red);
                } else {
                    Item->setBackground(Qt::green);
                }
            }
            cout << telemetry_values[i].value.toStdString() << endl;

        }
    }
    cout << "end update" << endl;
}
int gui_init(int argc, char *argv[]) {
    QApplication app(argc, argv); //(постоянная) приложение

    dataStruct telemetry_values[85];

    connect();
    getTelemetry((dataStruct*) telemetry_values);

    QWidget *window = new QWidget;
    listWidget = new QListWidget;
    listWidget->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item = new QListWidgetItem;
    Item->setText(QString::fromStdString("Ожидание данных"));
    listWidget->insertItem(0, Item);

    listWidget1 = new QListWidget;
    listWidget1->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item1 = new QListWidgetItem;
    Item1->setText(QString::fromStdString("Ожидание данных"));
    listWidget1->insertItem(0, Item1);



    QGridLayout *MainLayout = new QGridLayout();
    MainLayout->addWidget(listWidget, 0, 0);
    MainLayout->addWidget(listWidget1, 0, 1);

    window->setLayout(MainLayout);
    window->showFullScreen();


   /** dataStruct telemetry_values[85];
    telemetry_values[0] = {"numPage", 4, "U32", "Номер", "", false}; // 1 id записи в черном ящике
    telemetry_values[1] = {"Code", 2, "U16", "Код события", "", false}; // 2 код события
    telemetry_values[2] = {"Time", 4, "U32", "Время", " сек.", false}; // 3 время события
    telemetry_values[3] = {"State", 1, "U8", "Состояние", "", false}; // 4 статус устройства
    telemetry_values[4] = {"Module1", 1, "U8", "Модуль1", "", false}; // 5 статус функциональных модулей 1
    telemetry_values[5] = {"Module2", 1, "U8", "Модуль2", "", false}; // 6 статус функциональных модулей 2
    telemetry_values[6] = {"GSM", 1, "U8", "GSM", "",}; // 7 уровень gsm
    telemetry_values[7] = {"StateGauge", 1, "U8", "StateGauge", "", false}; // 8 состояние навигационного датчика GPS/Глонасс
    telemetry_values[8] = {"LastTime", 4, "U32", "Время GPS", "сек.", false}; // 9 время последних валидных координат
    telemetry_values[9] = {"Lat", 4, "I32", "Широта", "", false}; // 10 последняя валидная широта
    telemetry_values[10] = {"Lon", 4, "I32", "Долгота", "", false}; // 11 долгота
    telemetry_values[11] = {"Alt", 4, "I32", "Высота", "", false}; // 12 высота
    telemetry_values[12] = {"Speed", 4, "Float", "Скорость", "км/ч", false}; // 13 скорость
    telemetry_values[13] = {"Course", 2, "U16", "Курс", "градусы", false};  // 14 курс
    telemetry_values[14] = {"Mileage", 4, "Float", "Пробег", "км", false}; // 15 текущий пробег
    telemetry_values[15] = {"Way", 4, "Float", "Направление", "", false};  // 16 последний отрезок пути
    telemetry_values[16] = {"AllSeconds", 2, "U16", "AllSeconds", "", false}; // 17 общее кол-во сек на последнем отрезке
    telemetry_values[17] = {"SecondLast", 2, "U16", "SecondLast", "", false}; // 18 Количество секунд на последнем отрезке пути, по которым вычислялся пробег
    telemetry_values[18] = {"Power", 2, "U16", "Напряжение", "", false}; // 19 Напряжение на основном источнике питания
    telemetry_values[19] = {"Reserv", 2, "U16", "Резерв. Напряжение", "", false}; // 20 Напряжение на резервном источнике питания
    telemetry_values[20] = {"StateU_Ain1", 2, "U16", "Ain1", "мв", false}; // 21 Напряжение на аналоговом входе 1 (Ain1)
    telemetry_values[21] = {"StateU_Ain2", 2, "U16", "Ain2", "мв", false}; // 22 на входе 2 (Ain2)
    telemetry_values[22] = {"StateU_Ain3", 2, "U16", "Ain3", "мв", false}; // 23 на входе 3 (Ain3)
    telemetry_values[23] = {"StateU_Ain4", 2, "U16", "Ain4", "мв", false}; // 24 на входе 4 (Ain4)
    telemetry_values[24] = {"StateU_Ain5", 2, "U16", "Ain5", "мв", false}; // 25 на входе 5 (Ain5)
    telemetry_values[25] = {"StateU_Ain6", 2, "U16", "Ain6", "мв", false}; // 26 на входе 6 (Ain6)
    telemetry_values[26] = {"StateU_Ain7", 2, "U16", "Ain7", "мв", false}; // 27 на входе 7 (Ain7)
    telemetry_values[27] = {"StateU_Ain8", 2, "U16", "Ain8", "мв", false}; // 28 на входе 8 (Ain8)
    telemetry_values[28] = {"StateIn1", 1, "U8", "In1", "мв", false}; // 29 Текущие показания дискретных датчиков 1
    telemetry_values[29] = {"StateIn2", 1, "U8", "In2", "мв", false}; // 30  датчиков 2
    telemetry_values[30] = {"stateOut1", 1, "U8", "Out1", "мв", false}; // 31 Текущее состояние выходов 1
    telemetry_values[31] = {"stateOut2", 1, "U8", "Out2", "мв", false}; // 32  выходов 2
    telemetry_values[32] = {"StateInImp1", 4, "U32", "Imp1", " имп.", false}; // 33 Показания счетчика импульсов 1
    telemetry_values[33] = {"StateInImp2", 4, "U32", "Imp2", "имп.", false}; // 34 2
    telemetry_values[34] = {"Frequency1", 2, "U16", "Frequency1", "чст.", false}; // 35 Частота на аналогово-частотном датчике уровня топлива 1
    telemetry_values[35] = {"Frequency2", 2, "U16", "Frequency2", "чст.", false}; // 36 датчике  2
    telemetry_values[36] = {"Motochas", 4, "U32", "Моточас", "сек.", false}; // 37 Моточасы, подсчитанные во время срабатывания датчика работы генератора
    telemetry_values[37] = {"LevelRS485_1", 2, "U16", "название", "км/ч", false}; // 38 Уровень топлива, измеренный датчиком уровня топлива 1 RS - 485
    telemetry_values[38] = {"LevelRS485_2", 2, "U16", "название", "км/ч", false}; // 39 Уровень топлива 2
    telemetry_values[39] = {"LevelRS485_3", 2, "U16", "название", "км/ч", false}; // 40 Уровень топлива 3
    telemetry_values[40] = {"LevelRS485_4", 2, "U16", "название", "км/ч", false}; // 41 Уровень топлива 4
    telemetry_values[41] = {"LevelRS485_5", 2, "U16", "название", "км/ч", false}; // 42 Уровень топлива 5
    telemetry_values[42] = {"LevelRS485_6", 2, "U16", "название", "км/ч", false}; // 43 Уровень топлива 6
    telemetry_values[43] = {"LevelRS232", 2, "U16", "название", "км/ч", false}; // 44 Уровень топлива, измеренный датчиком уровня топлива RS - 232
    telemetry_values[44] = {"Temp1", 1, "I8", "Temp1", "C", false}; // 45 датчика 1 (в градусах Цельсия)
    telemetry_values[45] = {"Temp2", 1, "I8", "Temp2", "C", false}; // 46 датчика 2 (в градусах Цельсия)
    telemetry_values[46] = {"Temp3", 1, "I8", "Temp3", "C", false}; // 47 датчика 3 (в градусах Цельсия)
    telemetry_values[47] = {"Temp4", 1, "I8", "Temp4", "C", false}; // 48 датчика 4 (в градусах Цельсия)
    telemetry_values[48] = {"Temp5", 1, "I8", "Temp5", "C", false}; // 49 датчика 5 (в градусах Цельсия)
    telemetry_values[49] = {"Temp6", 1, "I8", "Temp6", "C", false}; // 50 датчика 6 (в градусах Цельсия)
    telemetry_values[50] = {"Temp7", 1, "I8", "Temp7", "C", false}; // 51 датчика 7 (в градусах Цельсия)
    telemetry_values[51] = {"Temp8", 1, "I8", "Temp8", "C", false}; // 52 датчика 8
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
    telemetry_values[84] = {"ATemp1", 2, "U16", "название", "км/ч", false};**/


    pthread_t thread;
    int result = pthread_create(&thread, NULL, flex, &telemetry_values);
    {
        if (result != 0) {
            perror("Создание первого потока!");
            return EXIT_FAILURE;
        }
        pthread_detach(thread);
    }

    qRegisterMetaType<QVector<int> >("QVector<int>");

    int code = app.exec();
    close_socket();
    return code;
}
