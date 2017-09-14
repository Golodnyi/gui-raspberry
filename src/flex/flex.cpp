#include <QString>
#include <QtGui>
#include <bitset>
#include <errno.h> /* Объявления кодов ошибок */
#include <fcntl.h> /* Объявления управления файлами */
#include <iostream>
#include <src/struct.h>
#include <stdio.h>  /* Стандартные объявления ввода/вывода */
#include <string.h> /* Объявления строковых функций */
#include <sys/socket.h>
#include <unistd.h> /* Объявления стандартных функций UNIX */

using namespace std;

int a;
int fd;
int listen_socket;
int client_socket;

extern int open_socket(int listen_socket);
extern int open_port(int fd);
extern TResult read_head(int fd, int client_socket); //Считывание head
extern int my_in(int fd, char *buff, int size, int client_socket);
extern char answer_collect(TResult returnValue, char *answer, char *answer_body,
                           int body_size);
extern int my_out(int fd, char *buff, int size, int client_socket);
extern unsigned char crc8_calc(unsigned char *lp_block, unsigned int len);
extern void *TelemetryConvert(dataStruct *telemetry_values,
                              bitset<85> bitfield);
extern void close(int *a);
extern string ftos(float f, int nd);

void *flex(void *arg)
{
  dataStruct *telemetry_values = (dataStruct *)arg;
  vector<char> temp_vector;

  fd = open_port(fd);

  if (fd == -1)
  {
    listen_socket = open_socket(listen_socket);
  }
  while (fd != -1 | (client_socket = accept(listen_socket, NULL, NULL)))
  {
    if (fd != -1)
    {
      a = fd;
      cout << "Start COM port " << endl;
    }
    else
    {
      a = client_socket;
      cout << "Start socket " << endl;
    }

    TResult returnValue = read_head(fd, client_socket);

    char s[3];

    copy(returnValue.buff, returnValue.buff + 3, s);
    for (int i = 0; i <= 2; i++)
    {
      cout << s[i]; // первые 3 байта-идентификатор объекта
    }

    char znak = returnValue.buff[3];
    cout << znak; // разделительный знак :

    char IMEI[15]; // идентификатор устройства GSM модема
    copy(returnValue.buff + 4, returnValue.buff + 19, IMEI);
    for (int i = 0; i <= 14; i++)
    {
      cout << IMEI[i];
    }
    cout << endl;

    cout << "Send request" << endl;
    char answer[19]; // массив ответа
    char answer_body[3] = {'*', '<', 'S'};
    uint16_t body_size = 3;
    answer_collect(returnValue, (char *)answer, (char *)answer_body, body_size);
    copy(answer_body, answer_body + 3, answer + 16);
    int bytes = my_out(fd, (char *)answer, 19, client_socket);
    if (bytes == -1)
    {
      char *errmsg = strerror(errno);
      printf("%s\n", errmsg);
    }
    cout << "Send " << bytes << " bytes"
         << endl; // выводим количество отправленных байт

    returnValue = read_head(fd, client_socket);

    char FLEX[6];
    copy(returnValue.buff, returnValue.buff + 6, FLEX);
    for (int i = 0; i <= 5; i++)
    {
      cout << FLEX[i];
    }
    cout << endl;

    uint8_t protocol =
        (uint8_t)returnValue.buff[6]; // Условное обозначение протокола, в
    // котором собирается работать устройство
    cout << protocol << endl;
    uint8_t protocol_version = (uint8_t)returnValue.buff[7]; // Версия протокола
    cout << protocol_version << endl;
    uint8_t struct_version =
        (uint8_t)returnValue.buff[8]; // Версия структуры данных
    cout << struct_version << endl;
    uint8_t data_size =
        (uint8_t)
            returnValue.buff[9]; // Размер последующего конфигурационного поля
    cout << data_size << endl;
    uint8_t bitfield_size = data_size / 8 + 1;
    cout << "Bitfield size " << bitfield_size << " bytes" << endl;

    bitset<85> bitfield;
    int g = 0;
    for (int i = 0; i < bitfield_size; i++)
    {
      for (int j = 7; j >= 0; j--)
      {
        uint8_t tbyte = returnValue.buff[i + 10];
        bool state = (bool)(tbyte & (1 << j));
        bitfield[g] = state;
        cout << bitfield[g];
        if (g >= 84)
        {
          break;
        }
        g++;
      }
    }
    cout << endl;

    cout << "Send request 2" << endl;
    char answer_2[25]; // массив ответа 2
    char body[6] = {'*', '<', 'F', 'L', 'E', 'X'};
    char answer_body_2[9];
    copy(body, body + 6, answer_body_2);
    answer_body_2[6] = protocol;
    answer_body_2[7] = protocol_version;
    answer_body_2[8] = struct_version;
    uint16_t body_size_2 = 9;
    answer_collect(returnValue, (char *)answer_2, (char *)answer_body_2,
                   body_size_2);
    copy(answer_body_2, answer_body_2 + 9, answer_2 + 16);

    int bytes_2 = my_out(fd, (char *)answer_2, 25, client_socket);
    if (bytes_2 == -1)
    {
      char *errmsg = strerror(errno);
      printf("%s\n", errmsg);
    }
    cout << "Send " << bytes_2 << " bytes" << endl;

    char buff_3_1[2];                              // 3 пакет с текущим состоянием
    my_in(fd, (char *)buff_3_1, 2, client_socket); //связали сокет с буфером 3
    char index[2];
    copy(buff_3_1, buff_3_1 + 2,
         index); // перенесли первые 2 байта из сообщения
    temp_vector.resize(2);
    for (int i = 0; i < 2; i++)
    {
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

    for (int i = 0; i < bitfield.size(); i++)
    {
      if ((bool)bitfield[i] == 1)
      {
        buff_3_2.resize(telemetry_values[i].byte);
        my_in(fd, &buff_3_2[0], telemetry_values[i].byte, client_socket);
        if (telemetry_values[i].byte == 4)
        {
          if (telemetry_values[i].type == "I32")
          {
            int32_t a = ((int8_t)buff_3_2[3] << 24) +
                        ((int8_t)buff_3_2[2] << 16) +
                        ((int8_t)buff_3_2[1] << 8) + (int8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(a));
            cout << telemetry_values[i].alias.toStdString() << " = " << a
                 << " I32";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else if (telemetry_values[i].type == "U32")
          {
            uint32_t b = ((uint8_t)buff_3_2[3] << 24) +
                         ((uint8_t)buff_3_2[2] << 16) +
                         ((uint8_t)buff_3_2[1] << 8) + (uint8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(b));
            cout << telemetry_values[i].alias.toStdString() << " = " << b
                 << " U32";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else if (telemetry_values[i].type == "Float")
          {
            for (int j = 0; j < 4; j++)
            {
              speed.speedData[j] = buff_3_2[j];
            }
            string c = ftos(speed.speed, 2);
            telemetry_values[i].value = QString::fromStdString(c);
            cout << telemetry_values[i].alias.toStdString() << " = " << c
                 << " Float";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " округленное значение с сокета" << endl;
          }
          else
          {
            cout << "mistake, 4 bytes" << endl;
          }
        }
        else if (telemetry_values[i].byte == 2)
        {
          if (telemetry_values[i].type == "I16")
          {
            int16_t d = ((int8_t)buff_3_2[1] << 8) + (int8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(d));
            cout << telemetry_values[i].alias.toStdString() << " = " << d
                 << " I16";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else if (telemetry_values[i].type == "U16")
          {
            uint16_t e = ((uint8_t)buff_3_2[1] << 8) + (uint8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(e));
            cout << telemetry_values[i].alias.toStdString() << " = " << e
                 << " U16";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else
          {
            cout << "mistake, 2 bytes" << endl;
          }
        }
        else if (telemetry_values[i].byte == 1)
        {
          if (telemetry_values[i].type == "I8")
          {
            int8_t f = (int8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(f));
            cout << telemetry_values[i].alias.toStdString() << " = " << f
                 << " I8";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else if (telemetry_values[i].type == "U8")
          {
            uint8_t g = (uint8_t)buff_3_2[0];
            telemetry_values[i].value = QString::fromStdString(to_string(g));
            cout << telemetry_values[i].alias.toStdString() << " = " << g
                 << " U8";
            cout << " = " << telemetry_values[i].value.toStdString()
                 << " значение с сокета" << endl;
          }
          else
          {
            cout << "mistake, 1 bytes" << endl;
          }
        }
        else
        {
          cout << "mistake" << endl;
        }
        temp_vector.insert(temp_vector.end(), buff_3_2.begin(), buff_3_2.end());
        buff_3_2.clear();
      }
    }

    char buff_3_3[1];
    my_in(fd, (char *)buff_3_3, 1, client_socket);
    uint8_t crc8 = (uint8_t)buff_3_3[0];
    cout << crc8 << endl;

    unsigned char buff_val_3_2 =
        crc8_calc((unsigned char *)temp_vector.data(), temp_vector.size());
    if (buff_val_3_2 == crc8)
    {
      cout << "crc8 success" << endl;
    }
    else
    {
      cout << "crc8 fail: " << buff_val_3_2 << crc8 << endl;
      continue;
    }

    cout << "Send request 3" << endl;

    char answer_3[3];
    copy(buff_3_1, buff_3_1 + 2, answer_3);
    answer_3[2] = crc8_calc((unsigned char *)answer_3, 2);

    int bytes_3 = my_out(fd, (char *)answer_3, 3, client_socket);
    if (bytes_3 == -1)
    {
      char *errmsg = strerror(errno);
      printf("%s\n", errmsg);
    }
    cout << "Send " << bytes_3 << " bytes" << endl;

    TelemetryConvert(telemetry_values, bitfield);
  }

  close(a);
  exit(0);
}
