// ConsoleApplication3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")   //библиотека для сокетов
#include <conio.h>                   //для использования getch()
#include <winsock2.h>
#include <WS2tcpip.h>
#include <bitset>
#include <vector>

using namespace std;

extern unsigned char xor_sum(unsigned char *buffer, unsigned int length);

int main()
{
	WSADATA wsaData;                // старт использования библиотеки сокетов
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)				// проверка на ошибку загрузки
	{
		cerr << "WSAStartup failed: " << result << endl;
		return result;
	}
	struct addrinfo* addr = NULL;		// структура, хранящая информацию об IP-адресе  слущающего сокета
	struct addrinfo hints;				// шаблон для инициализации структуры адреса
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;			// использование сети для работы с сокетами
	hints.ai_socktype = SOCK_STREAM;	// Задаем потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;	// Используем протокол TCP
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, "9000", &hints, &addr);	// задаем фактический адрес: IP и номер порта
	if (result != 0)					// проверка на ошибку инициализации адресса
	{
		cerr << "getaddrinfo failed: " << result << endl;
		WSACleanup();					// выгрузка библиотеки Ws2_32.dll
		return 1;						// выход из программы
	}

	int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);	// принимающий сокет

	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen); // привязываем сокет на IP-адресс из getaddrinfo
	listen(listen_socket, SOMAXCONN);	// ожидание соединения

	int client_socket;
	for (;;)
	{
		char head[16];
		client_socket = accept(listen_socket, NULL, NULL);
		result = recv(client_socket, head, 16, 0);		//связали сокет с head

		char preamble[4];
		copy(head, head + 4, preamble);			// перенесли первые 4 байта в преамбулу 
		for (int i = 0; i <= 3; i++) {
			cout << preamble[i];
		}
		cout << endl;

		// побитно сдвигаем, выводим каждый элемент
		uint32_t IDr = ((uint8_t)head[7] << 24) + ((uint8_t)head[6] << 16) + ((uint8_t)head[5] << 8) + (uint8_t)head[4];	// идентификатор получателя
		cout << IDr << endl;
		uint32_t IDs = ((uint8_t)head[11] << 24) + ((uint8_t)head[10] << 16) + ((uint8_t)head[9] << 8) + (uint8_t)head[8]; //идентификатор отправителя
		cout << IDs << endl;
		uint16_t size = ((uint8_t)head[13] << 8) + (uint8_t)head[12];		//размер 
		cout << size << endl;
		uint8_t CSd = (uint8_t)head[14];		// контрольная сумма документов
		cout << CSd << endl;
		uint8_t CSp = (uint8_t)head[15];		// контрольная сумма заголовка
		cout << CSp << endl;

		char buff[19];										// массив для пакета1
		result = recv(client_socket, buff, size, 0);		// создаем массив, для хранения пакета и связываем с сокетом

		unsigned char buff_val = xor_sum((unsigned char*)buff, 19);
		if (buff_val == CSd) {
			cout << "CSd success" << endl;
		}
		else {
			cout << "CSd fail" << endl;			// проверяем сумму документов (CSd-контр.сумма документов, buff-общая сумма входящего пакета)
		}

		unsigned char CSp_val = xor_sum((unsigned char*)head, 15);

		if (CSp_val == CSp) {
			cout << "CSp success" << endl;
		}
		else {
			cout << "CSp fail" << endl;		// проверяем заголовок (CSp-контр.сумма заголовка, head-16 байтовый заголовок пакета)
		}

		char s[3];
		copy(buff, buff + 3, s);
		for (int i = 0; i <= 2; i++) {
			cout << s[i];					// первые 3 байта-идентификатор объекта
		}

		char znak = buff[3];
		cout << znak;						// разделительный знак :

		char IMEI[15];						// идентификатор устройства GSM модема
		copy(buff + 4, buff + 19, IMEI);
		for (int i = 0; i <= 14; i++) {
			cout << IMEI[i];
		}
		cout << endl;

		cout << "Send request" << endl;
		char answer[19];				// массив ответа
		char answer_body[3] = { '*', '<', 'S' };
		uint16_t body_size = 3;

		copy(preamble, preamble + 4, answer);	// записываем преамбулу в ответ
		int k = 0;
		for (int i = 4; i < 8; i++) {			// записываем IDs ответ
			answer[i] = (IDs >> k) & 0xFF;
			k = k + 8;
		}
		k = 0;
		for (int i = 8; i < 12; i++) {			// записываем IDr в ответ
			answer[i] = (IDr >> k) & 0xFF;
			k = k + 8;
		}
		k = 0;
		for (int i = 12; i < 14; i++) {				// записываем размер в ответ
			answer[i] = (body_size << k) & 0xFF;
			k = k + 8;
		}
		unsigned char body_sign = xor_sum((unsigned char*)answer_body, 3);
		answer[14] = body_sign;

		unsigned char head_sign = xor_sum((unsigned char*)answer, 15);
		answer[15] = head_sign;

		copy(answer_body, answer_body + 3, answer + 16);

		int bytes = send(client_socket, answer, 19, 0);
		cout << "Send " << bytes << " bytes" << endl;		// выводим количество отправленных байт

		char head_2[16];									// 2 сообщение с протоколами
		result = recv(client_socket, head_2, 16, 0);		//связали сокет с head2

		char preamble_2[4];
		copy(head_2, head_2 + 4, preamble_2);			// перенесли первые 4 байта в преамбулу 
		for (int i = 0; i <= 3; i++) {
			cout << preamble_2[i];
		}
		cout << endl;

		// побитно сдвигаем, выводим каждый элемент 2 cсообщения
		uint32_t IDr_2 = ((uint8_t)head_2[7] << 24) + ((uint8_t)head_2[6] << 16) + ((uint8_t)head_2[5] << 8) + (uint8_t)head_2[4];	// идентификатор получателя 2 (не изменен)
		cout << IDr_2 << endl;
		uint32_t IDs_2 = ((uint8_t)head_2[11] << 24) + ((uint8_t)head_2[10] << 16) + ((uint8_t)head_2[9] << 8) + (uint8_t)head_2[8]; //идентификатор отправителя 2 (не изменен)
		cout << IDs_2 << endl;
		uint16_t size_2 = ((uint8_t)head_2[13] << 8) + (uint8_t)head_2[12];		//размер 2 сообщения
		cout << size_2 << endl;
		uint8_t CSd_2 = (uint8_t)head_2[14];		// контрольная сумма документов 2
		cout << CSd_2 << endl;
		uint8_t CSp_2 = (uint8_t)head_2[15];		// контрольная сумма заголовка 2
		cout << CSp_2 << endl;

		char buff_2[19];										// массив для 2 сообщения
		result = recv(client_socket, buff_2, size_2, 0);		// связываем с сокетом

		unsigned char buff_val_2 = xor_sum((unsigned char*)buff_2, 19);
		if (buff_val_2 == CSd_2) {
			cout << "CSd 2 success" << endl;
		}
		else {
			cout << "CSd 2 fail" << endl;
		}

		unsigned char CSp_val_2 = xor_sum((unsigned char*)head_2, 15);

		if (CSp_val_2 == CSp_2) {
			cout << "CSp 2 success" << endl;
		}
		else {
			cout << "CSp 2 fail" << endl;
		}
		char FLEX[6];
		copy(buff_2, buff_2 + 6, FLEX);
		for (int i = 0; i <= 5; i++) {
			cout << FLEX[i];
		}
		cout << endl;

		uint8_t protocol = (uint8_t)buff_2[6];			// Условное обозначение протокола, в котором собирается работать устройство 
		cout << protocol << endl;
		uint8_t protocol_version = (uint8_t)buff_2[7];	// Версия протокола
		cout << protocol_version << endl;
		uint8_t struct_version = (uint8_t)buff_2[8];	// Версия структуры данных
		cout << struct_version << endl;
		uint8_t data_size = (uint8_t)buff_2[9];		// Размер последующего конфигурационного поля 
		cout << data_size << endl;
		uint8_t bitfield_size = data_size / 8 + 1;
		cout << "Bitfield size " << bitfield_size << " bytes" << endl;

		bitset<69> bitfield;
		int g = 0;
		for (int i = 0; i < bitfield_size; i++) {
			for (int j = 7; j >= 0; j--) {
				uint8_t tbyte = buff_2[i + 10];
				bitfield[g] = (bool)(tbyte & (1 << j));
				cout << bitfield[g];
				if (g >= 68) {
					break;
				}
				g++;
			}
		}
		cout << endl;

		cout << "Send request 2" << endl;
		char answer_2[25];				// массив ответа 2
		char body[6] = { '*', '<', 'F', 'L', 'E', 'X' };
		char answer_body_2[9];
		copy(body, body + 6, answer_body_2);
		answer_body_2[6] = protocol;
		answer_body_2[7] = protocol_version;
		answer_body_2[8] = struct_version;
		uint16_t body_size_2 = 9;

		copy(preamble_2, preamble_2 + 4, answer_2);		// записываем преамбулу во 2 ответ
		int j = 0;
		for (int i = 4; i < 8; i++) {			// записываем IDs_2 во 2 ответ
			answer_2[i] = (IDs_2 >> j) & 0xFF;
			j = j + 8;
		}
		j = 0;
		for (int i = 8; i < 12; i++) {			// записываем IDr_2 во 2 ответ
			answer_2[i] = (IDr_2 >> j) & 0xFF;
			j = j + 8;
		}
		j = 0;
		for (int i = 12; i < 14; i++) {				// записываем размер во 2 ответ
			answer_2[i] = (body_size_2 << j) & 0xFF;
			j = j + 8;
		}
		unsigned char body_sign_2 = xor_sum((unsigned char*)answer_body_2, body_size_2 );
		answer_2[14] = body_sign_2;

		unsigned char head_sign_2 = xor_sum((unsigned char*)answer_2, 15);
		answer_2[15] = head_sign_2;

		copy(answer_body_2, answer_body_2 + 9, answer_2 + 16 );

		int bytes_2 = send(client_socket, answer_2, 25, 0);
		cout << "Send " << bytes_2 << " bytes" << endl;

		struct values {
			int byte;
			string value;
			string type;
		};
		values telemetry_values[69][3];
		telemetry_values[0][0].value = { "numPage" }; // 1 id записи в черном ящике
		telemetry_values[0][1].byte = 4;
		telemetry_values[0][2].type = { "U32" };
		telemetry_values[1][0].value = { "Code" }; // 2 код события
		telemetry_values[1][1].byte = 2;
		telemetry_values[1][2].type = { "U16" };
		telemetry_values[2][0].value = { "Time" }; // 3 время события
		telemetry_values[2][1].byte = 4;
		telemetry_values[2][2].type = { "U32" };
		telemetry_values[3][0].value = { "State" }; // 4 статус устройства (битфилд)
		telemetry_values[3][1].byte = 1;
		telemetry_values[3][2].type = { "U8" };
		telemetry_values[4][0].value = { "Module1" }; // 5 статус функциональных модулей 1 (битфилд)
		telemetry_values[4][1].byte = 1;
		telemetry_values[4][2].type = { "U8" };
		telemetry_values[5][0].value = { "Module2" }; // 6 статус функциональных модулей 2 (битфилд)
		telemetry_values[5][1].byte = 1;
		telemetry_values[5][2].type = { "U8" };
		telemetry_values[6][0].value = { "GSM" }; // 7 уровень gsm
		telemetry_values[6][1].byte = 1;
		telemetry_values[6][2].type = { "U8" };
		telemetry_values[7][0].value = { "StateGauge" }; // 8 состояние навигационного датчика GPS/Глонасс (битфилд)
		telemetry_values[7][1].byte = 1;
		telemetry_values[7][2].type = { "U8" };
		telemetry_values[8][0].value = { "LastTime" }; // 9 время последних валидных координат
		telemetry_values[8][1].byte = 4;
		telemetry_values[8][2].type = { "U32" };
		telemetry_values[9][0].value = { "Lat" }; // 10 последняя валидная широта
		telemetry_values[9][1].byte = 4;
		telemetry_values[9][2].type = { "I32" };
		telemetry_values[10][0].value = { "Lon" }; // 11 долгота
		telemetry_values[10][1].byte = 4;
		telemetry_values[10][2].type = { "I32" };
		telemetry_values[11][0].value = { "Alt" }; // 12 высота
		telemetry_values[11][1].byte = 4;
		telemetry_values[11][2].type = { "I32" };
		telemetry_values[12][0].value = { "Speed" }; // 13 скорость (флоут)
		telemetry_values[12][1].byte = 4;
		telemetry_values[12][2].type = { "Float" };
		telemetry_values[13][0].value = { "Course" };  // 14 курс
		telemetry_values[13][1].byte = 2;
		telemetry_values[13][2].type = { "U16" };
		telemetry_values[14][0].value = { "Mileage" }; // 15 текущий пробег (флоут)
		telemetry_values[14][1].byte = 4;
		telemetry_values[14][2].type = { "Float" };
		telemetry_values[15][0].value = { "Way" };  // 16 последний отрезок пути (флоут)
		telemetry_values[15][1].byte = 4;
		telemetry_values[15][2].type = { "Float" };
		telemetry_values[16][0].value = { "AllSeconds" }; // 17 общее кол-во сек на последнем отрезке
		telemetry_values[16][1].byte = 2;
		telemetry_values[16][2].type = { "U16" };
		telemetry_values[17][0].value = { "SecondLast" }; // 18 Количество секунд на последнем отрезке пути, по которым вычислялся пробег
		telemetry_values[17][1].byte = 2;
		telemetry_values[17][2].type = { "U16" };
		telemetry_values[18][0].value = { "Power" }; // 19 Напряжение на основном источнике питания
		telemetry_values[18][1].byte = 2;
		telemetry_values[18][2].type = { "U16" };
		telemetry_values[19][0].value = { "Reserv" }; // 20 Напряжение на резервном источнике питания
		telemetry_values[19][1].byte = 2;
		telemetry_values[19][2].type = { "U16 " };
		telemetry_values[20][0].value = { "StateU_Ain1" }; // 21 Напряжение на аналоговом входе 1 (Ain1)
		telemetry_values[20][1].byte = 2;
		telemetry_values[20][2].type = { "U16" };
		telemetry_values[21][0].value = { "StateU_Ain2" }; // 22 на входе 2 (Ain2)
		telemetry_values[21][1].byte = 2;
		telemetry_values[21][2].type = { "U16" };
		telemetry_values[22][0].value = { "StateU_Ain3" }; // 23 на входе 3 (Ain3)
		telemetry_values[22][1].byte = 2;
		telemetry_values[22][2].type = { "U16" };
		telemetry_values[23][0].value = { "StateU_Ain4" }; // 24 на входе 4 (Ain4)
		telemetry_values[23][1].byte = 2;
		telemetry_values[23][2].type = { "U16" };
		telemetry_values[24][0].value = { "StateU_Ain5" }; // 25 на входе 5 (Ain5)
		telemetry_values[24][1].byte = 2;
		telemetry_values[24][2].type = { "U16" };
		telemetry_values[25][0].value = { "StateU_Ain6" }; // 26 на входе 6 (Ain6)
		telemetry_values[25][1].byte = 2;
		telemetry_values[25][2].type = { "U16" };
		telemetry_values[26][0].value = { "StateU_Ain7" }; // 27 на входе 7 (Ain7)
		telemetry_values[26][1].byte = 2;
		telemetry_values[26][2].type = { "U16" };
		telemetry_values[27][0].value = { "StateU_Ain8" }; // 28 на входе 8 (Ain8)
		telemetry_values[27][1].byte = 2;
		telemetry_values[27][2].type = { "U16" };
		telemetry_values[28][0].value = { "StateIn1" }; // 29 Текущие показания дискретных датчиков 1
		telemetry_values[28][1].byte = 1;
		telemetry_values[28][2].type = { "U8" };
		telemetry_values[29][0].value = { "StateIn2" }; // 30  датчиков 2
		telemetry_values[29][1].byte = 1;
		telemetry_values[29][2].type = { "U8" };
		telemetry_values[30][0].value = { "stateOut1" }; // 31 Текущее состояние выходов 1
		telemetry_values[30][1].byte = 1;
		telemetry_values[30][2].type = { "U8" };
		telemetry_values[31][0].value = { "stateOut2" }; // 32  выходов 2
		telemetry_values[31][1].byte = 1;
		telemetry_values[31][2].type = { "U8" };
		telemetry_values[32][0].value = { "StateInImp1" }; // 33 Показания счетчика импульсов 1
		telemetry_values[32][1].byte = 4;
		telemetry_values[32][2].type = { "U32" };
		telemetry_values[33][0].value = { "StateInImp2" }; // 34 2
		telemetry_values[33][1].byte = 4;
		telemetry_values[33][2].type = { "U32" };
		telemetry_values[34][0].value = { "Frequency1" }; // 35 Частота на аналогово-частотном датчике уровня топлива 1
		telemetry_values[34][1].byte = 2;
		telemetry_values[34][2].type = { "U16" };
		telemetry_values[35][0].value = { "Frequency2" }; // 36 датчике  2
		telemetry_values[35][1].byte = 2;
		telemetry_values[35][2].type = { "U16" };
		telemetry_values[36][0].value = { "Motochas" }; // 37 Моточасы, подсчитанные во время срабатывания датчика работы генератора
		telemetry_values[36][1].byte = 4;
		telemetry_values[36][2].type = { "U32" };
		telemetry_values[37][0].value = { "LevelRS485_1" }; // 38 Уровень топлива, измеренный датчиком уровня топлива 1 RS - 485
		telemetry_values[37][1].byte = 2;
		telemetry_values[37][2].type = { "U16" };
		telemetry_values[38][0].value = { "LevelRS485_2" }; // 39 Уровень топлива 2
		telemetry_values[38][1].byte = 2;
		telemetry_values[38][2].type = { "U16" };
		telemetry_values[39][0].value = { "LevelRS485_3" }; // 40 Уровень топлива 3
		telemetry_values[39][1].byte = 2;
		telemetry_values[39][2].type = { "U16" };
		telemetry_values[40][0].value = { "LevelRS485_4" }; // 41 Уровень топлива 4 
		telemetry_values[40][1].byte = 2;
		telemetry_values[40][2].type = { "U16" };
		telemetry_values[41][0].value = { "LevelRS485_5" }; // 42 Уровень топлива 5
		telemetry_values[41][1].byte = 2;
		telemetry_values[41][2].type = { "U16" };
		telemetry_values[42][0].value = { "LevelRS485_6" }; // 43 Уровень топлива 6 
		telemetry_values[42][1].byte = 2;
		telemetry_values[42][2].type = { "U16" };
		telemetry_values[43][0].value = { "LevelRS232" }; // 44 Уровень топлива, измеренный датчиком уровня топлива RS - 232
		telemetry_values[43][1].byte = 2;
		telemetry_values[43][2].type = { "U16" };
		telemetry_values[44][0].value = { "Temp1" }; // 45 датчика 1 (в градусах Цельсия)
		telemetry_values[44][1].byte = 1;
		telemetry_values[44][2].type = { "I8" };
		telemetry_values[45][0].value = { "Temp2" }; // 46 датчика 2 (в градусах Цельсия)
		telemetry_values[45][1].byte = 1;
		telemetry_values[45][2].type = { "I8" };
		telemetry_values[46][0].value = { "Temp3" }; // 47 датчика 3 (в градусах Цельсия)
		telemetry_values[46][1].byte = 1;
		telemetry_values[46][2].type = { "I8" };
		telemetry_values[47][0].value = { "Temp4" }; // 48 датчика 4 (в градусах Цельсия)
		telemetry_values[47][1].byte = 1;
		telemetry_values[47][2].type = { "I8" };
		telemetry_values[48][0].value = { "Temp5" }; // 49 датчика 5 (в градусах Цельсия)
		telemetry_values[48][1].byte = 1;
		telemetry_values[48][2].type = { "I8" };
		telemetry_values[49][0].value = { "Temp6" }; // 50 датчика 6 (в градусах Цельсия)
		telemetry_values[49][1].byte = 1;
		telemetry_values[49][2].type = { "I8" };
		telemetry_values[50][0].value = { "Temp7" }; // 51 датчика 7 (в градусах Цельсия)
		telemetry_values[50][1].byte = 1;
		telemetry_values[50][2].type = { "I8" };
		telemetry_values[51][0].value = { "Temp8" }; // 52 датчика 8 
		telemetry_values[51][1].byte = 1;
		telemetry_values[51][2].type = { "I8" };
		telemetry_values[52][0].value = { "CAN_FuelLevel" }; // 53 Уровень топлива в баке
		telemetry_values[52][1].byte = 2;
		telemetry_values[52][2].type = { "U16" };
		telemetry_values[53][0].value = { "CAN_FuelConsumption" }; // 54 Полный расход топлива(флоат)
		telemetry_values[53][1].byte = 4;
		telemetry_values[53][2].type = { "Float" };
		telemetry_values[54][0].value = { "CAN_EngineTurns" }; // 55 Обороты двигателя "CAN_Temp"
		telemetry_values[54][1].byte = 2;
		telemetry_values[54][2].type = { "U16" };
		telemetry_values[55][0].value = { "CAN_Temp" }; // 56 Температура охлаждающей жидкости (двигателя)
		telemetry_values[55][1].byte = 1;
		telemetry_values[55][2].type = { "I8" };
		telemetry_values[56][0].value = { "CAN_FullRun" }; // 57 Полный пробег ТС
		telemetry_values[56][1].byte = 4;
		telemetry_values[56][2].type = { "Float" };
		telemetry_values[57][0].value = { "CAN_AxleLoad_1" }; // 58 Нагрузка на ось 1
		telemetry_values[57][1].byte = 2;
		telemetry_values[57][2].type = { "U16" };
		telemetry_values[58][0].value = { "CAN_AxleLoad_2" }; // 59 Нагрузка на ось 2
		telemetry_values[58][1].byte = 2;
		telemetry_values[58][2].type = { "U16" };
		telemetry_values[59][0].value = { "CAN_AxleLoad_3" }; // 60 Нагрузка на ось 3
		telemetry_values[59][1].byte = 2;
		telemetry_values[59][2].type = { "U16" };
		telemetry_values[60][0].value = { "CAN_AxleLoad_4" }; // 61 Нагрузка на ось 4
		telemetry_values[60][1].byte = 2;
		telemetry_values[60][2].type = { "U16" };
		telemetry_values[61][0].value = { "CAN_AxleLoad_5" }; // 62 Нагрузка на ось 5
		telemetry_values[61][1].byte = 2;
		telemetry_values[61][2].type = { "U16" };
		telemetry_values[62][0].value = { "CAN_PedalAccel" }; // 63 Положение педали газа
		telemetry_values[62][1].byte = 1;
		telemetry_values[62][2].type = { "U8" };
		telemetry_values[63][0].value = { "CAN_PedalStop" }; // 64  Положение педали тормоза
		telemetry_values[63][1].byte = 1;
		telemetry_values[63][2].type = { "U8" };
		telemetry_values[64][0].value = { "CAN_EngineLoad" }; // 65  Нагрузка на двигатель 
		telemetry_values[64][1].byte = 1;
		telemetry_values[64][2].type = { "U8" };
		telemetry_values[65][0].value = { "CAN_LevelFiltr" }; // 66  Уровень жидкости в дизельном фильтре выхлопных газов
		telemetry_values[65][1].byte = 2;
		telemetry_values[65][2].type = { "U16" };
		telemetry_values[66][0].value = { "CAN_EngineTime" }; // 67 Полное время работы двигателя
		telemetry_values[66][1].byte = 4;
		telemetry_values[66][2].type = { "U32" };
		telemetry_values[67][0].value = { "CAN_TimeTO" };  // 68 Расстояние до ТО
		telemetry_values[67][1].byte = 2;
		telemetry_values[67][2].type = { "I16" };
		telemetry_values[68][0].value = { "CAN_Speed" }; // 69 Скорость ТС
		telemetry_values[68][1].byte = 1;
		telemetry_values[68][2].type = { "U8" };

		int x = 0;
		for (int i = 0; i < 69; i++) {
			if ((bool)bitfield[i] == 1) {
				x += telemetry_values[i][1].byte;
			}
		}
		cout << "x:= " << x << " bytes" << endl;

		char buff_3_1[2];									// 3 пакет с текущим состоянием
		result = recv(client_socket, buff_3_1, 2, 0);		//связали сокет с буфером 3
		char index[2];
		copy(buff_3_1, buff_3_1 + 2, index);			// перенесли первые 2 байта из сообщения
		for (int i = 0; i < 2; i++) {
			cout << index[i];
		}
		cout << endl;

		//char *buff_3_2 = new char[x];
		vector<unsigned char>buff_3_2(x);
		// char *buff_3_2 = (char *)malloc(x * sizeof(char));
		result = recv(client_socket, buff_3_2.data, buff_3_2.size(), 0);

		/*char test[29];
		copy(buff_3_2, buff_3_2 + 29, test);
		cout << test << endl;*/

		/*for (int i = 0; i < 69; i++) {
			int l = i;
			if ((bool)bitfield[i] == 1) {
				if (telemetry_values[i][2].byte == 4) {
					int32_t a = ((int8_t)buff_3_2[l = l + 3] << 24) + ((int8_t)buff_3_2[l = l - 1] << 16) + ((int8_t)buff_3_2[l = l - 1] << 8) + (int8_t)buff_3_2[l = l - 1];
					cout << telemetry_values[i][2].value << ' = ' << a << endl;
				}
				else if (telemetry_values[i][2].byte == 2) {
					int16_t b = ((int8_t)buff_3_2[l = l + 1] << 8) + (int8_t)buff_3_2[l = l - 1];
					cout << telemetry_values[i][2].value << ' = ' << b << endl;
				}
				else if (telemetry_values[i][2].byte == 1) {
					uint8_t c = (uint8_t)buff_3_2[l];
					cout << telemetry_values[i][2].value << ' = ' << c << endl;
				}
				else {
					cout << "test";
				}
			
			}
			else {
				cout << "test1";
			}
		}*/

		char buff_3_3[1];									
		result = recv(client_socket, buff_3_3, 1, 0);
		uint8_t crc8 = (uint8_t)buff_3_3[1];
		cout << crc8 << endl;

	}

}
