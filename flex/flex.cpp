#include <iostream>
#include <bitset>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>
#include <QtGui>

using namespace std;

struct dataStruct {
    QString alias; // англ название (numPage)
    int byte; // кол-во байт
    QString type; // тип предоставления данных
    QString name; // название датчика на русском (температура блока цилиндров)
    QString unit; // мера измерения (км в час, цельясия...)
    QString filter; // перевод значений
    bool enable; // значение из сокета
    QString value; // значение датчика
    QString color;

};
struct TResult{
    char preamble[4];
    uint32_t IDr;
    uint32_t IDs;
    uint16_t size;
    uint8_t CSd;
    uint8_t CSp;
    char buff[19];     //массив для пакета данных
};
extern TResult recv_head(int client_socket);
extern char answer_send(TResult returnValue, char *answer, char *answer_body, int body_size);
extern unsigned char crc8_calc(unsigned char *lp_block, unsigned int len);
extern void *TelemetryConvert(dataStruct *telemetry_values, bitset<85> bitfield);


int client_socket;
void close_socket() {
    close(client_socket);
    cout << "Close socket" << endl;
}
void *flex(void *arg) {
    dataStruct *telemetry_values = (dataStruct *) arg;
    vector<char> temp_vector;
    int result;

    struct addrinfo *addr = NULL;        // структура, хранящая информацию об IP-адресе  слущающего сокета
    struct addrinfo hints;                // шаблон для инициализации структуры адреса


    hints.ai_family = AF_INET;            // использование сети для работы с сокетами
    hints.ai_socktype = SOCK_STREAM;    // Задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP;    // Используем протокол TCP
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "9000", &hints, &addr);    // задаем фактический адрес: IP и номер порта
    if (result != 0)                    // проверка на ошибку инициализации адресса
    {
        cerr << "getaddrinfo failed: " << result << endl;
        exit(1);                        // выход из программы
    }

    int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);    // принимающий сокет
    cout << "socket start: " << result << endl;
    bind(listen_socket, addr->ai_addr,
                  (int) addr->ai_addrlen); // привязываем сокет на IP-адресс из getaddrinfo
    listen(listen_socket, SOMAXCONN);    // ожидание соединения

    while (client_socket = accept(listen_socket, NULL, NULL)) {
        cout << "connect" << endl;
        temp_vector.clear();

        TResult returnValue = recv_head(client_socket);

        char s[3];
        copy(returnValue.buff, returnValue.buff + 3, s);
        for (int i = 0; i <= 2; i++) {
            cout << s[i];                    // первые 3 байта-идентификатор объекта
        }

        char znak = returnValue.buff[3];
        cout << znak;                        // разделительный знак :

        char IMEI[15];                        // идентификатор устройства GSM модема
        copy(returnValue.buff + 4, returnValue.buff + 19, IMEI);
        for (int i = 0; i <= 14; i++) {
            cout << IMEI[i];
        }
        cout << endl;

        cout << "Send request" << endl;
        char answer[19];                // массив ответа
        char answer_body[3] = {'*', '<', 'S'};
        uint16_t body_size = 3;
        answer_send(returnValue, (char*) answer, (char*) answer_body, body_size);
        copy(answer_body, answer_body + 3, answer + 16);
        int bytes = send(client_socket, answer, 19, 0);
        cout << "Send " << bytes << " bytes" << endl;        // выводим количество отправленных байт

        returnValue = recv_head(client_socket);

        char FLEX[6];
        copy(returnValue.buff, returnValue.buff + 6, FLEX);
        for (int i = 0; i <= 5; i++) {
            cout << FLEX[i];
        }
        cout << endl;

        uint8_t protocol = (uint8_t) returnValue.buff[6];            // Условное обозначение протокола, в котором собирается работать устройство
        cout << protocol << endl;
        uint8_t protocol_version = (uint8_t) returnValue.buff[7];    // Версия протокола
        cout << protocol_version << endl;
        uint8_t struct_version = (uint8_t) returnValue.buff[8];    // Версия структуры данных
        cout << struct_version << endl;
        uint8_t data_size = (uint8_t) returnValue.buff[9];        // Размер последующего конфигурационного поля
        cout << data_size << endl;
        uint8_t bitfield_size = data_size / 8 + 1;
        cout << "Bitfield size " << bitfield_size << " bytes" << endl;

        bitset<85> bitfield;
        int g = 0;
        for (int i = 0; i < bitfield_size; i++) {
            for (int j = 7; j >= 0; j--) {
                uint8_t tbyte = returnValue.buff[i + 10];
                bool state = (bool) (tbyte & (1 << j));
                bitfield[g] = state;
                cout << bitfield[g];
                if (g >= 84) {
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
        answer_send(returnValue, (char*) answer_2, (char*) answer_body_2, body_size_2);
        copy(answer_body_2, answer_body_2 + 9, answer_2 + 16);

        int bytes_2 = send(client_socket, answer_2, 25, 0);
        cout << "Send " << bytes_2 << " bytes" << endl;

        char buff_3_1[2];                                    // 3 пакет с текущим состоянием
        recv(client_socket, buff_3_1, 2, 0);        //связали сокет с буфером 3
        char index[2];
        copy(buff_3_1, buff_3_1 + 2, index);            // перенесли первые 2 байта из сообщения
        temp_vector.resize(2);
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
                        telemetry_values[i].value = QString::fromStdString(to_string(a));
                        cout << telemetry_values[i].alias.toStdString() << " = " << a << " I32";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U32") {
                        uint32_t b = ((uint8_t) buff_3_2[3] << 24) + ((uint8_t) buff_3_2[2] << 16) +
                                     ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = QString::fromStdString(to_string(b));
                        cout << telemetry_values[i].alias.toStdString() << " = " << b << " U32";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "Float") {
                        for (int j = 0; j < 4; j++) {
                            speed.speedData[j] = buff_3_2[j];
                        }
                        float c = speed.speed;
                        telemetry_values[i].value = QString::fromStdString(to_string(c));
                        cout << telemetry_values[i].alias.toStdString() << " = " << c << " Float";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else {
                        cout << "mistake, 4 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 2) {
                    if (telemetry_values[i].type == "I16") {
                        int16_t d = ((int8_t) buff_3_2[1] << 8) + (int8_t) buff_3_2[0];
                        telemetry_values[i].value = QString::fromStdString(to_string(d));
                        cout << telemetry_values[i].alias.toStdString() << " = " << d << " I16";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U16") {
                        uint16_t e = ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = QString::fromStdString(to_string(e));
                        cout << telemetry_values[i].alias.toStdString() << " = " << e << " U16";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else {
                        cout << "mistake, 2 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 1) {
                    if (telemetry_values[i].type == "I8") {
                        int8_t f = (int8_t) buff_3_2[0];
                        telemetry_values[i].value = QString::fromStdString(to_string(f));
                        cout << telemetry_values[i].alias.toStdString() << " = " << f << " I8";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U8") {
                        uint8_t g = (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = QString::fromStdString(to_string(g));
                        cout << telemetry_values[i].alias.toStdString() << " = " << g << " U8";
                        cout << " = " << telemetry_values[i].value.toStdString() << " значение с сокета" << endl;
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
        recv(client_socket, buff_3_3, 1, 0);
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

        int bytes_3 = send(client_socket, answer_3, 3, 0);
        cout << "Send " << bytes_3 << " bytes" << endl;

        TelemetryConvert(telemetry_values, bitfield);
    }
    close_socket();
    exit(0);
}