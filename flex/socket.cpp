#include <iostream>
#include <sys/socket.h>

using namespace std;
struct TResult{
    char preamble[4];
    uint32_t IDr;
    uint32_t IDs;
    uint16_t size;
    uint8_t CSd;
    uint8_t CSp;
    char buff[19];     //массив для пакета данных
};


extern unsigned char xor_sum(unsigned char *buffer, unsigned int length);

TResult recv_head(const char *head,int client_socket) {

    TResult returnValue;

    copy(head, head + 4, returnValue.preamble);            // перенесли первые 4 байта в преамбулу
    for (int i = 0; i <= 3; i++) {
        cout << returnValue.preamble[i];
    }
    cout << endl;

// побитно сдвигаем, выводим каждый элемент
    returnValue.IDr = ((uint8_t) head[7] << 24) + ((uint8_t) head[6] << 16) + ((uint8_t) head[5] << 8) +
                   (uint8_t) head[4];    // идентификатор получателя
    cout << returnValue.IDr << endl;
    returnValue.IDs = ((uint8_t) head[11] << 24) + ((uint8_t) head[10] << 16) + ((uint8_t) head[9] << 8) +
                   (uint8_t) head[8]; //идентификатор отправителя
    cout << returnValue.IDs << endl;
    returnValue.size = ((uint8_t) head[13] << 8) + (uint8_t) head[12];        //размер
    cout << returnValue.size << endl;
    returnValue.CSd = (uint8_t) head[14];        // контрольная сумма документов
    cout << returnValue.CSd << endl;
    returnValue.CSp = (uint8_t) head[15];        // контрольная сумма заголовка
    cout << returnValue.CSp << endl;


    recv(client_socket, returnValue.buff, returnValue.size, 0);        // создаем массив, для хранения пакета и связываем с сокетом

    unsigned char buff_val = xor_sum((unsigned char *) returnValue.buff, 19);
    if (buff_val == returnValue.CSd) {
        cout << "CSd success" << endl;
    } else {
        cout << "CSd fail"
             << endl;
    }

    unsigned char CSp_val = xor_sum((unsigned char *) head, 15);

    if (CSp_val == returnValue.CSp) {
        cout << "CSp success" << endl;
    } else {
        cout << "CSp fail"
             << endl;        // проверяем заголовок (CSp-контр.сумма заголовка, head-16 байтовый заголовок пакета)
    }

    return (returnValue);
}

char answer_send(TResult returnValue, char* answer, char *answer_body, int *body_size) {
    copy(returnValue.preamble, returnValue.preamble + 4, answer);    // записываем преамбулу в ответ
    short int k = 0;
    for (int i = 4; i < 8; i++) {            // записываем IDs ответ
        answer[i] = (returnValue.IDs >> k) & 0xFF;
        k += 8;
    }
    k = 0;
    for (int i = 8; i < 12; i++) {            // записываем IDr в ответ
        answer[i] = (returnValue.IDr >> k) & 0xFF;
        k += 8;
    }
    k = 0;
    for (int i = 12; i < 14; i++) {                // записываем размер в ответ
        answer[i] = (*body_size << k) & 0xFF;
        k += 8;
    }

    unsigned char body_sign = xor_sum((unsigned char *) answer_body, 3);
    answer[14] = body_sign;

    unsigned char head_sign = xor_sum((unsigned char *) answer, 15);
    answer[15] = head_sign;

    return (*answer);
}
