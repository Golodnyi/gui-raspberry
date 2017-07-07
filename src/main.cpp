#include <iostream>
#include <bitset>
#include <vector>

#if defined(_WIN32) || defined(WIN32)
#include "stdafx.h"
#pragma comment(lib, "Ws2_32.lib")   //библиотека для сокетов
#include <conio.h>                   //для использования getch()
#include <winsock2.h>
#include <WS2tcpip.h>
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>

#endif
using namespace std;

extern unsigned char xor_sum(unsigned char *buffer, unsigned int length);

extern unsigned char crc8_calc(unsigned char *lp_block, unsigned int len);

extern int gui_init(int argc, char *argv[], vector<char> data);

int main(int argc, char *argv[]) {
    vector<char> temp_vector; // длина телеметрических данных + преамбула 2 байта
    int gui = gui_init(argc, argv, temp_vector);

    if (gui != 0) {
        return gui;
    }

    struct values {
        string value;
        short int byte;
        string type;
    };
    values telemetry_values[85];
    telemetry_values[0] = {"numPage", 4, "U32"}; // 1 id записи в черном ящике
    telemetry_values[1] = {"Code", 2, "U16"}; // 2 код события
    telemetry_values[2] = {"Time", 4, "U32"}; // 3 время события
    telemetry_values[3] = {"State", 1, "U8"}; // 4 статус устройства
    telemetry_values[4] = {"Module1", 1, "U8"}; // 5 статус функциональных модулей 1
    telemetry_values[5] = {"Module2", 1, "U8"}; // 6 статус функциональных модулей 2
    telemetry_values[6] = {"GSM", 1, "U8"}; // 7 уровень gsm
    telemetry_values[7] = {"StateGauge", 1, "U8"}; // 8 состояние навигационного датчика GPS/Глонасс
    telemetry_values[8] = {"LastTime", 4, "U32"}; // 9 время последних валидных координат
    telemetry_values[9] = {"Lat", 4, "I32"}; // 10 последняя валидная широта
    telemetry_values[10] = {"Lon", 4, "I32"}; // 11 долгота
    telemetry_values[11] = {"Alt", 4, "I32"}; // 12 высота
    telemetry_values[12] = {"Speed", 4, "Float"}; // 13 скорость
    telemetry_values[13] = {"Course", 2, "U16"};  // 14 курс
    telemetry_values[14] = {"Mileage", 4, "Float"}; // 15 текущий пробег
    telemetry_values[15] = {"Way", 4, "Float"};  // 16 последний отрезок пути
    telemetry_values[16] = {"AllSeconds", 2, "U16"}; // 17 общее кол-во сек на последнем отрезке
    telemetry_values[17] = {"SecondLast", 2,
                            "U16"}; // 18 Количество секунд на последнем отрезке пути, по которым вычислялся пробег
    telemetry_values[18] = {"Power", 2, "U16"}; // 19 Напряжение на основном источнике питания
    telemetry_values[19] = {"Reserv", 2, "U16"}; // 20 Напряжение на резервном источнике питания
    telemetry_values[20] = {"StateU_Ain1", 2, "U16"}; // 21 Напряжение на аналоговом входе 1 (Ain1)
    telemetry_values[21] = {"StateU_Ain2", 2, "U16"}; // 22 на входе 2 (Ain2)
    telemetry_values[22] = {"StateU_Ain3", 2, "U16"}; // 23 на входе 3 (Ain3)
    telemetry_values[23] = {"StateU_Ain4", 2, "U16"}; // 24 на входе 4 (Ain4)
    telemetry_values[24] = {"StateU_Ain5", 2, "U16"}; // 25 на входе 5 (Ain5)
    telemetry_values[25] = {"StateU_Ain6", 2, "U16"}; // 26 на входе 6 (Ain6)
    telemetry_values[26] = {"StateU_Ain7", 2, "U16"}; // 27 на входе 7 (Ain7)
    telemetry_values[27] = {"StateU_Ain8", 2, "U16"}; // 28 на входе 8 (Ain8)
    telemetry_values[28] = {"StateIn1", 1, "U8"}; // 29 Текущие показания дискретных датчиков 1
    telemetry_values[29] = {"StateIn2", 1, "U8"}; // 30  датчиков 2
    telemetry_values[30] = {"stateOut1", 1, "U8"}; // 31 Текущее состояние выходов 1
    telemetry_values[31] = {"stateOut2", 1, "U8"}; // 32  выходов 2
    telemetry_values[32] = {"StateInImp1", 4, "U32"}; // 33 Показания счетчика импульсов 1
    telemetry_values[33] = {"StateInImp2", 4, "U32"}; // 34 2
    telemetry_values[34] = {"Frequency1", 2, "U16"}; // 35 Частота на аналогово-частотном датчике уровня топлива 1
    telemetry_values[35] = {"Frequency2", 2, "U16"}; // 36 датчике  2
    telemetry_values[36] = {"Motochas", 4,
                            "U32"}; // 37 Моточасы, подсчитанные во время срабатывания датчика работы генератора
    telemetry_values[37] = {"LevelRS485_1", 2,
                            "U16"}; // 38 Уровень топлива, измеренный датчиком уровня топлива 1 RS - 485
    telemetry_values[38] = {"LevelRS485_2", 2, "U16"}; // 39 Уровень топлива 2
    telemetry_values[39] = {"LevelRS485_3", 2, "U16"}; // 40 Уровень топлива 3
    telemetry_values[40] = {"LevelRS485_4", 2, "U16"}; // 41 Уровень топлива 4
    telemetry_values[41] = {"LevelRS485_5", 2, "U16"}; // 42 Уровень топлива 5
    telemetry_values[42] = {"LevelRS485_6", 2, "U16"}; // 43 Уровень топлива 6
    telemetry_values[43] = {"LevelRS232", 2,
                            "U16"}; // 44 Уровень топлива, измеренный датчиком уровня топлива RS - 232
    telemetry_values[44] = {"Temp1", 1, "I8"}; // 45 датчика 1 (в градусах Цельсия)
    telemetry_values[45] = {"Temp2", 1, "I8"}; // 46 датчика 2 (в градусах Цельсия)
    telemetry_values[46] = {"Temp3", 1, "I8"}; // 47 датчика 3 (в градусах Цельсия)
    telemetry_values[47] = {"Temp4", 1, "I8"}; // 48 датчика 4 (в градусах Цельсия)
    telemetry_values[48] = {"Temp5", 1, "I8"}; // 49 датчика 5 (в градусах Цельсия)
    telemetry_values[49] = {"Temp6", 1, "I8"}; // 50 датчика 6 (в градусах Цельсия)
    telemetry_values[50] = {"Temp7", 1, "I8"}; // 51 датчика 7 (в градусах Цельсия)
    telemetry_values[51] = {"Temp8", 1, "I8"}; // 52 датчика 8
    telemetry_values[52] = {"CAN_FuelLevel", 2, "U16"}; // 53 Уровень топлива в баке
    telemetry_values[53] = {"CAN_FuelConsumption", 4, "Float"}; // 54 Полный расход топлива(флоат)
    telemetry_values[54] = {"CAN_EngineTurns", 2, "U16"}; // 55 Обороты двигателя "CAN_Temp"
    telemetry_values[55] = {"CAN_Temp", 1, "I8"}; // 56 Температура охлаждающей жидкости (двигателя)
    telemetry_values[56] = {"CAN_FullRun", 4, "Float"}; // 57 Полный пробег ТС
    telemetry_values[57] = {"CAN_AxleLoad_1", 2, "U16"}; // 58 Нагрузка на ось 1
    telemetry_values[58] = {"CAN_AxleLoad_2", 2, "U16"}; // 59 Нагрузка на ось 2
    telemetry_values[59] = {"CAN_AxleLoad_3", 2, "U16"}; // 60 Нагрузка на ось 3
    telemetry_values[60] = {"CAN_AxleLoad_4", 2, "U16"}; // 61 Нагрузка на ось 4
    telemetry_values[61] = {"CAN_AxleLoad_5", 2, "U16"}; // 62 Нагрузка на ось 5
    telemetry_values[62] = {"CAN_PedalAccel", 1, "U8"}; // 63 Положение педали газа
    telemetry_values[63] = {"CAN_PedalStop", 1, "U8"}; // 64  Положение педали тормоза
    telemetry_values[64] = {"CAN_EngineLoad", 1, "U8"}; // 65  Нагрузка на двигатель
    telemetry_values[65] = {"CAN_LevelFiltr", 2, "U16"}; // 66  Уровень жидкости в дизельном фильтре выхлопных газов
    telemetry_values[66] = {"CAN_EngineTime", 4, "U32"}; // 67 Полное время работы двигателя
    telemetry_values[67] = {"CAN_TimeTO", 2, "I16"};  // 68 Расстояние до ТО
    telemetry_values[68] = {"CAN_Speed", 1, "U8"}; // 69 Скорость ТС
    telemetry_values[69] = {"ATemp1", 2, "U16"};
    telemetry_values[70] = {"ATemp1", 2, "U16"};
    telemetry_values[71] = {"ATemp1", 2, "U16"};
    telemetry_values[72] = {"ATemp1", 2, "U16"};
    telemetry_values[73] = {"ATemp1", 2, "U16"};
    telemetry_values[74] = {"ATemp1", 2, "U16"};
    telemetry_values[75] = {"ATemp1", 2, "U16"};
    telemetry_values[76] = {"ATemp1", 2, "U16"};
    telemetry_values[77] = {"ATemp1", 2, "U16"};
    telemetry_values[78] = {"ATemp1", 2, "U16"};
    telemetry_values[79] = {"ATemp1", 2, "U16"};
    telemetry_values[80] = {"ATemp1", 2, "U16"};
    telemetry_values[81] = {"ATemp1", 2, "U16"};
    telemetry_values[82] = {"ATemp1", 2, "U16"};
    telemetry_values[83] = {"ATemp1", 2, "U16"};
    telemetry_values[84] = {"ATemp1", 2, "U16"};
    int result;
#if defined(_WIN32) || defined(WIN32)
    cout << "Windows" << endl;
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)                // проверка на ошибку загрузки
    {
        cerr << "WSAStartup failed: " << result << endl;
        return result;
    }
#else
    cout << "Linux" << endl;
#endif
    struct addrinfo *addr = NULL;        // структура, хранящая информацию об IP-адресе  слущающего сокета
    struct addrinfo hints;                // шаблон для инициализации структуры адреса
#if defined(_WIN32) || defined(WIN32)
    ZeroMemory(&hints, sizeof(hints));
#endif

    hints.ai_family = AF_INET;            // использование сети для работы с сокетами
    hints.ai_socktype = SOCK_STREAM;    // Задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP;    // Используем протокол TCP
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "9000", &hints, &addr);    // задаем фактический адрес: IP и номер порта
    if (result != 0)                    // проверка на ошибку инициализации адресса
    {
        cerr << "getaddrinfo failed: " << result << endl;
#if defined(_WIN32) || defined(WIN32)
        WSACleanup();                    // выгрузка библиотеки Ws2_32.dll
#endif
        return 1;                        // выход из программы
    }

    int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);    // принимающий сокет

    result = bind(listen_socket, addr->ai_addr,
                  (int) addr->ai_addrlen); // привязываем сокет на IP-адресс из getaddrinfo
    listen(listen_socket, SOMAXCONN);    // ожидание соединения

    int client_socket;
    for (;;) {
        char head[16];
        client_socket = accept(listen_socket, NULL, NULL);
        result = recv(client_socket, head, 16, 0);        //связали сокет с head

        char preamble[4];
        copy(head, head + 4, preamble);            // перенесли первые 4 байта в преамбулу
        for (int i = 0; i <= 3; i++) {
            cout << preamble[i];
        }
        cout << endl;

        // побитно сдвигаем, выводим каждый элемент
        uint32_t IDr = ((uint8_t) head[7] << 24) + ((uint8_t) head[6] << 16) + ((uint8_t) head[5] << 8) +
                       (uint8_t) head[4];    // идентификатор получателя
        cout << IDr << endl;
        uint32_t IDs = ((uint8_t) head[11] << 24) + ((uint8_t) head[10] << 16) + ((uint8_t) head[9] << 8) +
                       (uint8_t) head[8]; //идентификатор отправителя
        cout << IDs << endl;
        uint16_t size = ((uint8_t) head[13] << 8) + (uint8_t) head[12];        //размер
        cout << size << endl;
        uint8_t CSd = (uint8_t) head[14];        // контрольная сумма документов
        cout << CSd << endl;
        uint8_t CSp = (uint8_t) head[15];        // контрольная сумма заголовка
        cout << CSp << endl;

        char buff[19];                                        // массив для пакета1
        result = recv(client_socket, buff, size, 0);        // создаем массив, для хранения пакета и связываем с сокетом

        unsigned char buff_val = xor_sum((unsigned char *) buff, 19);
        if (buff_val == CSd) {
            cout << "CSd success" << endl;
        } else {
            cout << "CSd fail"
                 << endl;            // проверяем сумму документов (CSd-контр.сумма документов, buff-общая сумма входящего пакета)
            continue;
        }

        unsigned char CSp_val = xor_sum((unsigned char *) head, 15);

        if (CSp_val == CSp) {
            cout << "CSp success" << endl;
        } else {
            cout << "CSp fail"
                 << endl;        // проверяем заголовок (CSp-контр.сумма заголовка, head-16 байтовый заголовок пакета)
            continue;
        }

        char s[3];
        copy(buff, buff + 3, s);
        for (int i = 0; i <= 2; i++) {
            cout << s[i];                    // первые 3 байта-идентификатор объекта
        }

        char znak = buff[3];
        cout << znak;                        // разделительный знак :

        char IMEI[15];                        // идентификатор устройства GSM модема
        copy(buff + 4, buff + 19, IMEI);
        for (int i = 0; i <= 14; i++) {
            cout << IMEI[i];
        }
        cout << endl;

        cout << "Send request" << endl;
        char answer[19];                // массив ответа
        char answer_body[3] = {'*', '<', 'S'};
        uint16_t body_size = 3;

        copy(preamble, preamble + 4, answer);    // записываем преамбулу в ответ
        short int k = 0;
        for (int i = 4; i < 8; i++) {            // записываем IDs ответ
            answer[i] = (IDs >> k) & 0xFF;
            k += 8;
        }
        k = 0;
        for (int i = 8; i < 12; i++) {            // записываем IDr в ответ
            answer[i] = (IDr >> k) & 0xFF;
            k += 8;
        }
        k = 0;
        for (int i = 12; i < 14; i++) {                // записываем размер в ответ
            answer[i] = (body_size << k) & 0xFF;
            k += 8;
        }
        unsigned char body_sign = xor_sum((unsigned char *) answer_body, 3);
        answer[14] = body_sign;

        unsigned char head_sign = xor_sum((unsigned char *) answer, 15);
        answer[15] = head_sign;

        copy(answer_body, answer_body + 3, answer + 16);

        int bytes = send(client_socket, answer, 19, 0);
        cout << "Send " << bytes << " bytes" << endl;        // выводим количество отправленных байт

        char head_2[16];                                    // 2 сообщение с протоколами
        result = recv(client_socket, head_2, 16, 0);        //связали сокет с head2

        char preamble_2[4];
        copy(head_2, head_2 + 4, preamble_2);            // перенесли первые 4 байта в преамбулу
        for (int i = 0; i <= 3; i++) {
            cout << preamble_2[i];
        }
        cout << endl;

        // побитно сдвигаем, выводим каждый элемент 2 cсообщения
        uint32_t IDr_2 = ((uint8_t) head_2[7] << 24) + ((uint8_t) head_2[6] << 16) + ((uint8_t) head_2[5] << 8) +
                         (uint8_t) head_2[4];    // идентификатор получателя 2 (не изменен)
        cout << IDr_2 << endl;
        uint32_t IDs_2 = ((uint8_t) head_2[11] << 24) + ((uint8_t) head_2[10] << 16) + ((uint8_t) head_2[9] << 8) +
                         (uint8_t) head_2[8]; //идентификатор отправителя 2 (не изменен)
        cout << IDs_2 << endl;
        uint16_t size_2 = ((uint8_t) head_2[13] << 8) + (uint8_t) head_2[12];        //размер 2 сообщения
        cout << size_2 << endl;
        uint8_t CSd_2 = (uint8_t) head_2[14];        // контрольная сумма документов 2
        cout << CSd_2 << endl;
        uint8_t CSp_2 = (uint8_t) head_2[15];        // контрольная сумма заголовка 2
        cout << CSp_2 << endl;

        char buff_2[19];                                        // массив для 2 сообщения
        result = recv(client_socket, buff_2, size_2, 0);        // связываем с сокетом

        unsigned char buff_val_2 = xor_sum((unsigned char *) buff_2, 19);
        if (buff_val_2 == CSd_2) {
            cout << "CSd 2 success" << endl;
        } else {
            cout << "CSd 2 fail" << endl;
            continue;
        }

        unsigned char CSp_val_2 = xor_sum((unsigned char *) head_2, 15);

        if (CSp_val_2 == CSp_2) {
            cout << "CSp 2 success" << endl;
        } else {
            cout << "CSp 2 fail" << endl;
            continue;
        }
        char FLEX[6];
        copy(buff_2, buff_2 + 6, FLEX);
        for (int i = 0; i <= 5; i++) {
            cout << FLEX[i];
        }
        cout << endl;

        uint8_t protocol = (uint8_t) buff_2[6];            // Условное обозначение протокола, в котором собирается работать устройство
        cout << protocol << endl;
        uint8_t protocol_version = (uint8_t) buff_2[7];    // Версия протокола
        cout << protocol_version << endl;
        uint8_t struct_version = (uint8_t) buff_2[8];    // Версия структуры данных
        cout << struct_version << endl;
        uint8_t data_size = (uint8_t) buff_2[9];        // Размер последующего конфигурационного поля
        cout << data_size << endl;
        uint8_t bitfield_size = data_size / 8 + 1;
        cout << "Bitfield size " << bitfield_size << " bytes" << endl;

        bitset<69> bitfield;
        int g = 0;
        for (int i = 0; i < bitfield_size; i++) {
            for (int j = 7; j >= 0; j--) {
                uint8_t tbyte = buff_2[i + 10];
                bitfield[g] = (bool) (tbyte & (1 << j));
                cout << bitfield[g];
                if (g >= 68) {
                    break;
                }
                g++;
            }
        }
        cout << endl;

        cout << "Send request 2" << endl;
        char answer_2[25];                // массив ответа 2
        char body[6] = {'*', '<', 'F', 'L', 'E', 'X'};
        char answer_body_2[9];
        copy(body, body + 6, answer_body_2);
        answer_body_2[6] = protocol;
        answer_body_2[7] = protocol_version;
        answer_body_2[8] = struct_version;
        uint16_t body_size_2 = 9;

        copy(preamble_2, preamble_2 + 4, answer_2);        // записываем преамбулу во 2 ответ
        k = 0;
        for (int i = 4; i < 8; i++) {            // записываем IDs_2 во 2 ответ
            answer_2[i] = (IDs_2 >> k) & 0xFF;
            k += 8;
        }
        k = 0;
        for (int i = 8; i < 12; i++) {            // записываем IDr_2 во 2 ответ
            answer_2[i] = (IDr_2 >> k) & 0xFF;
            k += 8;
        }
        k = 0;
        for (int i = 12; i < 14; i++) {                // записываем размер во 2 ответ
            answer_2[i] = (body_size_2 << k) & 0xFF;
            k += 8;
        }
        unsigned char body_sign_2 = xor_sum((unsigned char *) answer_body_2, body_size_2);
        answer_2[14] = body_sign_2;

        unsigned char head_sign_2 = xor_sum((unsigned char *) answer_2, 15);
        answer_2[15] = head_sign_2;

        copy(answer_body_2, answer_body_2 + 9, answer_2 + 16);

        int bytes_2 = send(client_socket, answer_2, 25, 0);
        cout << "Send " << bytes_2 << " bytes" << endl;

        int x = 0;
        for (int i = 0; i < 69; i++) {
            if ((bool) bitfield[i] == 1) {
                x += telemetry_values[i].byte;
            }
        }
        cout << "x:= " << x << " bytes" << endl;

        char buff_3_1[2];                                    // 3 пакет с текущим состоянием
        result = recv(client_socket, buff_3_1, 2, 0);        //связали сокет с буфером 3
        char index[2];
        copy(buff_3_1, buff_3_1 + 2, index);            // перенесли первые 2 байта из сообщения
        for (int i = 0; i < 2; i++) {
            cout << index[i];
            temp_vector[i] = buff_3_1[i];
        }
        cout << endl;

        vector<char> buff_3_2;
        union su {
            float speed;
            char speedData[4];
        };
        su speed;

        for (int i = 0; i < bitfield.size(); i++) {
            if ((bool) bitfield[i] == 1) {
                buff_3_2.resize(telemetry_values[i].byte);
                result = recv(client_socket, &buff_3_2[0], telemetry_values[i].byte, 0);
                if (telemetry_values[i].byte == 4) {
                    if (telemetry_values[i].type == "I32") {
                        int32_t a = ((int8_t) buff_3_2[3] << 24) + ((int8_t) buff_3_2[2] << 16) +
                                    ((int8_t) buff_3_2[1] << 8) + (int8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << a << " I32" << endl;
                    } else if (telemetry_values[i].type == "U32") {
                        uint32_t b = ((uint8_t) buff_3_2[3] << 24) + ((uint8_t) buff_3_2[2] << 16) +
                                     ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << b << " U32" << endl;
                    } else if (telemetry_values[i].type == "Float") {
                        for (int j = 0; j < 4; j++) {
                            speed.speedData[j] = buff_3_2[j];
                        }
                        float c = speed.speed;
                        cout << telemetry_values[i].value << " = " << c << " Float" << endl;
                    } else {
                        cout << "mistake, 4 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 2) {
                    if (telemetry_values[i].type == "I16") {
                        int16_t d = ((int8_t) buff_3_2[1] << 8) + (int8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << d << " I16" << endl;
                    } else if (telemetry_values[i].type == "U16") {
                        uint16_t e = ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << e << " U16" << endl;
                    } else {
                        cout << "mistake, 2 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 1) {
                    if (telemetry_values[i].type == "I8") {
                        int8_t f = (int8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << f << " I8" << endl;
                    } else if (telemetry_values[i].type == "U8") {
                        uint8_t g = (uint8_t) buff_3_2[0];
                        cout << telemetry_values[i].value << " = " << g << " U8" << endl;
                    } else {
                        cout << "mistake, 1 bytes" << endl;
                    }
                } else {
                    cout << "mistake" << endl;
                }
                temp_vector.insert(temp_vector.end(), buff_3_2.begin(), buff_3_2.end());
                buff_3_2.clear();
            }
        }

        char buff_3_3[1];
        result = recv(client_socket, buff_3_3, 1, 0);
        uint8_t crc8 = (uint8_t) buff_3_3[0];
        cout << crc8 << endl;

        unsigned char buff_val_3_2 = crc8_calc((unsigned char *) temp_vector.data(), temp_vector.size());
        if (buff_val_3_2 == crc8) {
            cout << "crc8 success" << endl;
        } else {
            cout << "crc8 fail: " << buff_val_3_2 << crc8 << endl;
            continue;
        }

        cout << "Send request 3" << endl;

        char answer_3[3];
        copy(buff_3_1, buff_3_1 + 2, answer_3);
        answer_3[2] = crc8_calc((unsigned char *) answer_3, 2);
        //copy(buff_3_3, buff_3_3, answer_3 + 3);

        int bytes_3 = send(client_socket, answer_3, 3, 0);
        cout << "Send " << bytes_3 << " bytes" << endl;
    }
#if defined(_WIN32) || defined(WIN32)
    closesocket(client_socket);
#else
    close(client_socket);
#endif
    return 0;
}
