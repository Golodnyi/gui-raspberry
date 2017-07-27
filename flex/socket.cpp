#include <iostream>
#include <sys/socket.h>

using namespace std;

uint recv_head(int client_socket) {
    char head[16];
    recv(client_socket, head, 16, 0);        //связали сокет с head

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

    return (IDr, IDs, size, CSd, CSp);
}