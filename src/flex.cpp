#include <iostream>
#include <bitset>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zconf.h>

using namespace std;

extern unsigned char xor_sum(unsigned char *buffer, unsigned int length);

extern unsigned char crc8_calc(unsigned char *lp_block, unsigned int len);

extern void *update(void *arg);

struct dataStruct {
    string alias; // англ название (numPage)
    short int byte;
    string type;
    string name; // название датчика на русском (температура блока цилиндров)
    string unit; // мера измерения (км в час, цельясия...)
    bool enable; // значение из сокета
    string value; // значение из сокета

};

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
    result = bind(listen_socket, addr->ai_addr,
                  (int) addr->ai_addrlen); // привязываем сокет на IP-адресс из getaddrinfo
    listen(listen_socket, SOMAXCONN);    // ожидание соединения

    int client_socket;
    while (client_socket = accept(listen_socket, NULL, NULL)) {
        cout << "connect" << endl;
        temp_vector.clear();
        char head[16];

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
                bool state = (bool) (tbyte & (1 << j));
                bitfield[g] = state;
                telemetry_values[g].enable = state;
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
                        telemetry_values[i].value = to_string(a);
                        cout << telemetry_values[i].alias << " = " << a << " I32";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U32") {
                        uint32_t b = ((uint8_t) buff_3_2[3] << 24) + ((uint8_t) buff_3_2[2] << 16) +
                                     ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = to_string(b);
                        cout << telemetry_values[i].alias << " = " << b << " U32";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "Float") {
                        for (int j = 0; j < 4; j++) {
                            speed.speedData[j] = buff_3_2[j];
                        }
                        float c = speed.speed;
                        telemetry_values[i].value = to_string(c);
                        cout << telemetry_values[i].alias << " = " << c << " Float";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else {
                        cout << "mistake, 4 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 2) {
                    if (telemetry_values[i].type == "I16") {
                        int16_t d = ((int8_t) buff_3_2[1] << 8) + (int8_t) buff_3_2[0];
                        telemetry_values[i].value = to_string(d);
                        cout << telemetry_values[i].alias << " = " << d << " I16";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U16") {
                        uint16_t e = ((uint8_t) buff_3_2[1] << 8) + (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = to_string(e);
                        cout << telemetry_values[i].alias << " = " << e << " U16";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else {
                        cout << "mistake, 2 bytes" << endl;
                    }
                } else if (telemetry_values[i].byte == 1) {
                    if (telemetry_values[i].type == "I8") {
                        int8_t f = (int8_t) buff_3_2[0];
                        telemetry_values[i].value = to_string(f);
                        cout << telemetry_values[i].alias << " = " << f << " I8";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
                    } else if (telemetry_values[i].type == "U8") {
                        uint8_t j = (uint8_t) buff_3_2[0];
                        telemetry_values[i].value = to_string(j);
                        cout << telemetry_values[i].alias << " = " << g << " U8";
                        cout << " = " << telemetry_values[i].value << " значение с сокета" << endl;
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

        update(telemetry_values);
    }
    close(client_socket);
    exit(0);
}
