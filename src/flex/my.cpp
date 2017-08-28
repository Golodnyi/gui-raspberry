#include <errno.h> /* Объявления кодов ошибок */
#include <iostream>
#include <stdio.h> /* Стандартные объявления ввода/вывода */
#include <string.h> /* Объявления строковых функций */
#include <sys/socket.h>
#include <unistd.h> /* Объявления стандартных функций UNIX */

using namespace std;
int result;

int my_in(int fd, char *buff, int size, int client_socket) { //считывание данных
  if (fd != -1) {
    result = read(fd, buff, size);
    if (result == -1) {
      char *errmsg = strerror(errno);
      printf("error: %s\n", errmsg);
    }
    cout << "read " << size << " bytes: ";
    for (int i = 0; i < size; i++) {
      cout << buff[i];
    }
    cout << endl;
    return result;
  } else {
    result = recv(client_socket, buff, size, 0);
    if (result == -1) {
      char *errmsg = strerror(errno);
      printf("%s\n", errmsg);
    }
    return result;
  }
}

int my_out(int fd, char *buff, int size, int client_socket) { //отправка данных
  if (fd != -1) {
    result = write(fd, buff, size);
    return result;
  } else {
    result = send(client_socket, buff, size, 0);
    return result;
  }
}