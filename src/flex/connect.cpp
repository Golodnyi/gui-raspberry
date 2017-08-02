#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>   /* Объявления управления файлами */
#include <cstring>
#include <termio.h>

using namespace std;

int
set_interface_attribs (int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
    {
        return -1;
    }
    return 0;
}

int open_port(int fd){
    fd = open("/dev/ttyACM1", O_RDWR | O_NOCTTY |O_NONBLOCK);
    if (fd == -1)
    {
        //порт не открывается
        perror("open_port: Unable to open /dev/ttyACM1 - ");
    }
    else {
        set_interface_attribs(fd, B9600, 0);
        fcntl(fd, F_SETFL, 0);
    }
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

void close(int *a) {
    close(a);
    cout << "Close " << a << endl;
}