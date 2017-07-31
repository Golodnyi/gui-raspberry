#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>   /* Объявления управления файлами */
#include <bitset>
#include <QtGui>
#include <QString>

using namespace std;

int open_port(int fd){
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY |O_NONBLOCK);
    if (fd == -1)
    {
        //порт не открывается
        perror("open_port: Unable to open /dev/ttyS0 - ");
    }
    else
        fcntl(fd, F_SETFL, 0);
    return (fd);
}

int open_socket(int listen_socket){
    struct addrinfo *addr = NULL;        // структура, хранящая информацию об IP-адресе  слущающего сокета
    struct addrinfo hints;                // шаблон для инициализации структуры адреса


    hints.ai_family = AF_INET;            // использование сети для работы с сокетами
    hints.ai_socktype = SOCK_STREAM;    // Задаем потоковый тип сокета
    hints.ai_protocol = IPPROTO_TCP;    // Используем протокол TCP
    hints.ai_flags = AI_PASSIVE;

    int result = getaddrinfo(NULL, "9000", &hints, &addr);    // задаем фактический адрес: IP и номер порта
    if (result != 0)                    // проверка на ошибку инициализации адресса
    {
        cerr << "getaddrinfo failed: " << result << endl;
        exit(1);                        // выход из программы
    }
    cout << "Socket open: " << result << endl;

    listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);    // принимающий сокет
    bind(listen_socket, addr->ai_addr,
         (int) addr->ai_addrlen); // привязываем сокет на IP-адресс из getaddrinfo
    listen(listen_socket, SOMAXCONN);    // ожидание соединения**/
    return listen_socket;
}
