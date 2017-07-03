// ConsoleApplication3.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")   //���������� ��� �������
#include <conio.h>                   //��� ������������� getch()
#include <winsock2.h>
#include <WS2tcpip.h>
#include <bitset>
#include <vector>

using namespace std;

extern unsigned char xor_sum(unsigned char *buffer, unsigned int length);

int main()
{
	WSADATA wsaData;                // ����� ������������� ���������� �������
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)				// �������� �� ������ ��������
	{
		cerr << "WSAStartup failed: " << result << endl;
		return result;
	}
	struct addrinfo* addr = NULL;		// ���������, �������� ���������� �� IP-������  ���������� ������
	struct addrinfo hints;				// ������ ��� ������������� ��������� ������
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;			// ������������� ���� ��� ������ � ��������
	hints.ai_socktype = SOCK_STREAM;	// ������ ��������� ��� ������
	hints.ai_protocol = IPPROTO_TCP;	// ���������� �������� TCP
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, "9000", &hints, &addr);	// ������ ����������� �����: IP � ����� �����
	if (result != 0)					// �������� �� ������ ������������� �������
	{
		cerr << "getaddrinfo failed: " << result << endl;
		WSACleanup();					// �������� ���������� Ws2_32.dll
		return 1;						// ����� �� ���������
	}

	int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);	// ����������� �����

	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen); // ����������� ����� �� IP-������ �� getaddrinfo
	listen(listen_socket, SOMAXCONN);	// �������� ����������

	int client_socket;
	for (;;)
	{
		char head[16];
		client_socket = accept(listen_socket, NULL, NULL);
		result = recv(client_socket, head, 16, 0);		//������� ����� � head

		char preamble[4];
		copy(head, head + 4, preamble);			// ��������� ������ 4 ����� � ��������� 
		for (int i = 0; i <= 3; i++) {
			cout << preamble[i];
		}
		cout << endl;

		// ������� ��������, ������� ������ �������
		uint32_t IDr = ((uint8_t)head[7] << 24) + ((uint8_t)head[6] << 16) + ((uint8_t)head[5] << 8) + (uint8_t)head[4];	// ������������� ����������
		cout << IDr << endl;
		uint32_t IDs = ((uint8_t)head[11] << 24) + ((uint8_t)head[10] << 16) + ((uint8_t)head[9] << 8) + (uint8_t)head[8]; //������������� �����������
		cout << IDs << endl;
		uint16_t size = ((uint8_t)head[13] << 8) + (uint8_t)head[12];		//������ 
		cout << size << endl;
		uint8_t CSd = (uint8_t)head[14];		// ����������� ����� ����������
		cout << CSd << endl;
		uint8_t CSp = (uint8_t)head[15];		// ����������� ����� ���������
		cout << CSp << endl;

		char buff[19];										// ������ ��� ������1
		result = recv(client_socket, buff, size, 0);		// ������� ������, ��� �������� ������ � ��������� � �������

		unsigned char buff_val = xor_sum((unsigned char*)buff, 19);
		if (buff_val == CSd) {
			cout << "CSd success" << endl;
		}
		else {
			cout << "CSd fail" << endl;			// ��������� ����� ���������� (CSd-�����.����� ����������, buff-����� ����� ��������� ������)
		}

		unsigned char CSp_val = xor_sum((unsigned char*)head, 15);

		if (CSp_val == CSp) {
			cout << "CSp success" << endl;
		}
		else {
			cout << "CSp fail" << endl;		// ��������� ��������� (CSp-�����.����� ���������, head-16 �������� ��������� ������)
		}

		char s[3];
		copy(buff, buff + 3, s);
		for (int i = 0; i <= 2; i++) {
			cout << s[i];					// ������ 3 �����-������������� �������
		}

		char znak = buff[3];
		cout << znak;						// �������������� ���� :

		char IMEI[15];						// ������������� ���������� GSM ������
		copy(buff + 4, buff + 19, IMEI);
		for (int i = 0; i <= 14; i++) {
			cout << IMEI[i];
		}
		cout << endl;

		cout << "Send request" << endl;
		char answer[19];				// ������ ������
		char answer_body[3] = { '*', '<', 'S' };
		uint16_t body_size = 3;

		copy(preamble, preamble + 4, answer);	// ���������� ��������� � �����
		int k = 0;
		for (int i = 4; i < 8; i++) {			// ���������� IDs �����
			answer[i] = (IDs >> k) & 0xFF;
			k = k + 8;
		}
		k = 0;
		for (int i = 8; i < 12; i++) {			// ���������� IDr � �����
			answer[i] = (IDr >> k) & 0xFF;
			k = k + 8;
		}
		k = 0;
		for (int i = 12; i < 14; i++) {				// ���������� ������ � �����
			answer[i] = (body_size << k) & 0xFF;
			k = k + 8;
		}
		unsigned char body_sign = xor_sum((unsigned char*)answer_body, 3);
		answer[14] = body_sign;

		unsigned char head_sign = xor_sum((unsigned char*)answer, 15);
		answer[15] = head_sign;

		copy(answer_body, answer_body + 3, answer + 16);

		int bytes = send(client_socket, answer, 19, 0);
		cout << "Send " << bytes << " bytes" << endl;		// ������� ���������� ������������ ����

		char head_2[16];									// 2 ��������� � �����������
		result = recv(client_socket, head_2, 16, 0);		//������� ����� � head2

		char preamble_2[4];
		copy(head_2, head_2 + 4, preamble_2);			// ��������� ������ 4 ����� � ��������� 
		for (int i = 0; i <= 3; i++) {
			cout << preamble_2[i];
		}
		cout << endl;

		// ������� ��������, ������� ������ ������� 2 c���������
		uint32_t IDr_2 = ((uint8_t)head_2[7] << 24) + ((uint8_t)head_2[6] << 16) + ((uint8_t)head_2[5] << 8) + (uint8_t)head_2[4];	// ������������� ���������� 2 (�� �������)
		cout << IDr_2 << endl;
		uint32_t IDs_2 = ((uint8_t)head_2[11] << 24) + ((uint8_t)head_2[10] << 16) + ((uint8_t)head_2[9] << 8) + (uint8_t)head_2[8]; //������������� ����������� 2 (�� �������)
		cout << IDs_2 << endl;
		uint16_t size_2 = ((uint8_t)head_2[13] << 8) + (uint8_t)head_2[12];		//������ 2 ���������
		cout << size_2 << endl;
		uint8_t CSd_2 = (uint8_t)head_2[14];		// ����������� ����� ���������� 2
		cout << CSd_2 << endl;
		uint8_t CSp_2 = (uint8_t)head_2[15];		// ����������� ����� ��������� 2
		cout << CSp_2 << endl;

		char buff_2[19];										// ������ ��� 2 ���������
		result = recv(client_socket, buff_2, size_2, 0);		// ��������� � �������

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

		uint8_t protocol = (uint8_t)buff_2[6];			// �������� ����������� ���������, � ������� ���������� �������� ���������� 
		cout << protocol << endl;
		uint8_t protocol_version = (uint8_t)buff_2[7];	// ������ ���������
		cout << protocol_version << endl;
		uint8_t struct_version = (uint8_t)buff_2[8];	// ������ ��������� ������
		cout << struct_version << endl;
		uint8_t data_size = (uint8_t)buff_2[9];		// ������ ������������ ����������������� ���� 
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
		char answer_2[25];				// ������ ������ 2
		char body[6] = { '*', '<', 'F', 'L', 'E', 'X' };
		char answer_body_2[9];
		copy(body, body + 6, answer_body_2);
		answer_body_2[6] = protocol;
		answer_body_2[7] = protocol_version;
		answer_body_2[8] = struct_version;
		uint16_t body_size_2 = 9;

		copy(preamble_2, preamble_2 + 4, answer_2);		// ���������� ��������� �� 2 �����
		int j = 0;
		for (int i = 4; i < 8; i++) {			// ���������� IDs_2 �� 2 �����
			answer_2[i] = (IDs_2 >> j) & 0xFF;
			j = j + 8;
		}
		j = 0;
		for (int i = 8; i < 12; i++) {			// ���������� IDr_2 �� 2 �����
			answer_2[i] = (IDr_2 >> j) & 0xFF;
			j = j + 8;
		}
		j = 0;
		for (int i = 12; i < 14; i++) {				// ���������� ������ �� 2 �����
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
		telemetry_values[0][0].value = { "numPage" }; // 1 id ������ � ������ �����
		telemetry_values[0][1].byte = 4;
		telemetry_values[0][2].type = { "U32" };
		telemetry_values[1][0].value = { "Code" }; // 2 ��� �������
		telemetry_values[1][1].byte = 2;
		telemetry_values[1][2].type = { "U16" };
		telemetry_values[2][0].value = { "Time" }; // 3 ����� �������
		telemetry_values[2][1].byte = 4;
		telemetry_values[2][2].type = { "U32" };
		telemetry_values[3][0].value = { "State" }; // 4 ������ ���������� (�������)
		telemetry_values[3][1].byte = 1;
		telemetry_values[3][2].type = { "U8" };
		telemetry_values[4][0].value = { "Module1" }; // 5 ������ �������������� ������� 1 (�������)
		telemetry_values[4][1].byte = 1;
		telemetry_values[4][2].type = { "U8" };
		telemetry_values[5][0].value = { "Module2" }; // 6 ������ �������������� ������� 2 (�������)
		telemetry_values[5][1].byte = 1;
		telemetry_values[5][2].type = { "U8" };
		telemetry_values[6][0].value = { "GSM" }; // 7 ������� gsm
		telemetry_values[6][1].byte = 1;
		telemetry_values[6][2].type = { "U8" };
		telemetry_values[7][0].value = { "StateGauge" }; // 8 ��������� �������������� ������� GPS/������� (�������)
		telemetry_values[7][1].byte = 1;
		telemetry_values[7][2].type = { "U8" };
		telemetry_values[8][0].value = { "LastTime" }; // 9 ����� ��������� �������� ���������
		telemetry_values[8][1].byte = 4;
		telemetry_values[8][2].type = { "U32" };
		telemetry_values[9][0].value = { "Lat" }; // 10 ��������� �������� ������
		telemetry_values[9][1].byte = 4;
		telemetry_values[9][2].type = { "I32" };
		telemetry_values[10][0].value = { "Lon" }; // 11 �������
		telemetry_values[10][1].byte = 4;
		telemetry_values[10][2].type = { "I32" };
		telemetry_values[11][0].value = { "Alt" }; // 12 ������
		telemetry_values[11][1].byte = 4;
		telemetry_values[11][2].type = { "I32" };
		telemetry_values[12][0].value = { "Speed" }; // 13 �������� (�����)
		telemetry_values[12][1].byte = 4;
		telemetry_values[12][2].type = { "Float" };
		telemetry_values[13][0].value = { "Course" };  // 14 ����
		telemetry_values[13][1].byte = 2;
		telemetry_values[13][2].type = { "U16" };
		telemetry_values[14][0].value = { "Mileage" }; // 15 ������� ������ (�����)
		telemetry_values[14][1].byte = 4;
		telemetry_values[14][2].type = { "Float" };
		telemetry_values[15][0].value = { "Way" };  // 16 ��������� ������� ���� (�����)
		telemetry_values[15][1].byte = 4;
		telemetry_values[15][2].type = { "Float" };
		telemetry_values[16][0].value = { "AllSeconds" }; // 17 ����� ���-�� ��� �� ��������� �������
		telemetry_values[16][1].byte = 2;
		telemetry_values[16][2].type = { "U16" };
		telemetry_values[17][0].value = { "SecondLast" }; // 18 ���������� ������ �� ��������� ������� ����, �� ������� ���������� ������
		telemetry_values[17][1].byte = 2;
		telemetry_values[17][2].type = { "U16" };
		telemetry_values[18][0].value = { "Power" }; // 19 ���������� �� �������� ��������� �������
		telemetry_values[18][1].byte = 2;
		telemetry_values[18][2].type = { "U16" };
		telemetry_values[19][0].value = { "Reserv" }; // 20 ���������� �� ��������� ��������� �������
		telemetry_values[19][1].byte = 2;
		telemetry_values[19][2].type = { "U16 " };
		telemetry_values[20][0].value = { "StateU_Ain1" }; // 21 ���������� �� ���������� ����� 1 (Ain1)
		telemetry_values[20][1].byte = 2;
		telemetry_values[20][2].type = { "U16" };
		telemetry_values[21][0].value = { "StateU_Ain2" }; // 22 �� ����� 2 (Ain2)
		telemetry_values[21][1].byte = 2;
		telemetry_values[21][2].type = { "U16" };
		telemetry_values[22][0].value = { "StateU_Ain3" }; // 23 �� ����� 3 (Ain3)
		telemetry_values[22][1].byte = 2;
		telemetry_values[22][2].type = { "U16" };
		telemetry_values[23][0].value = { "StateU_Ain4" }; // 24 �� ����� 4 (Ain4)
		telemetry_values[23][1].byte = 2;
		telemetry_values[23][2].type = { "U16" };
		telemetry_values[24][0].value = { "StateU_Ain5" }; // 25 �� ����� 5 (Ain5)
		telemetry_values[24][1].byte = 2;
		telemetry_values[24][2].type = { "U16" };
		telemetry_values[25][0].value = { "StateU_Ain6" }; // 26 �� ����� 6 (Ain6)
		telemetry_values[25][1].byte = 2;
		telemetry_values[25][2].type = { "U16" };
		telemetry_values[26][0].value = { "StateU_Ain7" }; // 27 �� ����� 7 (Ain7)
		telemetry_values[26][1].byte = 2;
		telemetry_values[26][2].type = { "U16" };
		telemetry_values[27][0].value = { "StateU_Ain8" }; // 28 �� ����� 8 (Ain8)
		telemetry_values[27][1].byte = 2;
		telemetry_values[27][2].type = { "U16" };
		telemetry_values[28][0].value = { "StateIn1" }; // 29 ������� ��������� ���������� �������� 1
		telemetry_values[28][1].byte = 1;
		telemetry_values[28][2].type = { "U8" };
		telemetry_values[29][0].value = { "StateIn2" }; // 30  �������� 2
		telemetry_values[29][1].byte = 1;
		telemetry_values[29][2].type = { "U8" };
		telemetry_values[30][0].value = { "stateOut1" }; // 31 ������� ��������� ������� 1
		telemetry_values[30][1].byte = 1;
		telemetry_values[30][2].type = { "U8" };
		telemetry_values[31][0].value = { "stateOut2" }; // 32  ������� 2
		telemetry_values[31][1].byte = 1;
		telemetry_values[31][2].type = { "U8" };
		telemetry_values[32][0].value = { "StateInImp1" }; // 33 ��������� �������� ��������� 1
		telemetry_values[32][1].byte = 4;
		telemetry_values[32][2].type = { "U32" };
		telemetry_values[33][0].value = { "StateInImp2" }; // 34 2
		telemetry_values[33][1].byte = 4;
		telemetry_values[33][2].type = { "U32" };
		telemetry_values[34][0].value = { "Frequency1" }; // 35 ������� �� ���������-��������� ������� ������ ������� 1
		telemetry_values[34][1].byte = 2;
		telemetry_values[34][2].type = { "U16" };
		telemetry_values[35][0].value = { "Frequency2" }; // 36 �������  2
		telemetry_values[35][1].byte = 2;
		telemetry_values[35][2].type = { "U16" };
		telemetry_values[36][0].value = { "Motochas" }; // 37 ��������, ������������ �� ����� ������������ ������� ������ ����������
		telemetry_values[36][1].byte = 4;
		telemetry_values[36][2].type = { "U32" };
		telemetry_values[37][0].value = { "LevelRS485_1" }; // 38 ������� �������, ���������� �������� ������ ������� 1 RS - 485
		telemetry_values[37][1].byte = 2;
		telemetry_values[37][2].type = { "U16" };
		telemetry_values[38][0].value = { "LevelRS485_2" }; // 39 ������� ������� 2
		telemetry_values[38][1].byte = 2;
		telemetry_values[38][2].type = { "U16" };
		telemetry_values[39][0].value = { "LevelRS485_3" }; // 40 ������� ������� 3
		telemetry_values[39][1].byte = 2;
		telemetry_values[39][2].type = { "U16" };
		telemetry_values[40][0].value = { "LevelRS485_4" }; // 41 ������� ������� 4 
		telemetry_values[40][1].byte = 2;
		telemetry_values[40][2].type = { "U16" };
		telemetry_values[41][0].value = { "LevelRS485_5" }; // 42 ������� ������� 5
		telemetry_values[41][1].byte = 2;
		telemetry_values[41][2].type = { "U16" };
		telemetry_values[42][0].value = { "LevelRS485_6" }; // 43 ������� ������� 6 
		telemetry_values[42][1].byte = 2;
		telemetry_values[42][2].type = { "U16" };
		telemetry_values[43][0].value = { "LevelRS232" }; // 44 ������� �������, ���������� �������� ������ ������� RS - 232
		telemetry_values[43][1].byte = 2;
		telemetry_values[43][2].type = { "U16" };
		telemetry_values[44][0].value = { "Temp1" }; // 45 ������� 1 (� �������� �������)
		telemetry_values[44][1].byte = 1;
		telemetry_values[44][2].type = { "I8" };
		telemetry_values[45][0].value = { "Temp2" }; // 46 ������� 2 (� �������� �������)
		telemetry_values[45][1].byte = 1;
		telemetry_values[45][2].type = { "I8" };
		telemetry_values[46][0].value = { "Temp3" }; // 47 ������� 3 (� �������� �������)
		telemetry_values[46][1].byte = 1;
		telemetry_values[46][2].type = { "I8" };
		telemetry_values[47][0].value = { "Temp4" }; // 48 ������� 4 (� �������� �������)
		telemetry_values[47][1].byte = 1;
		telemetry_values[47][2].type = { "I8" };
		telemetry_values[48][0].value = { "Temp5" }; // 49 ������� 5 (� �������� �������)
		telemetry_values[48][1].byte = 1;
		telemetry_values[48][2].type = { "I8" };
		telemetry_values[49][0].value = { "Temp6" }; // 50 ������� 6 (� �������� �������)
		telemetry_values[49][1].byte = 1;
		telemetry_values[49][2].type = { "I8" };
		telemetry_values[50][0].value = { "Temp7" }; // 51 ������� 7 (� �������� �������)
		telemetry_values[50][1].byte = 1;
		telemetry_values[50][2].type = { "I8" };
		telemetry_values[51][0].value = { "Temp8" }; // 52 ������� 8 
		telemetry_values[51][1].byte = 1;
		telemetry_values[51][2].type = { "I8" };
		telemetry_values[52][0].value = { "CAN_FuelLevel" }; // 53 ������� ������� � ����
		telemetry_values[52][1].byte = 2;
		telemetry_values[52][2].type = { "U16" };
		telemetry_values[53][0].value = { "CAN_FuelConsumption" }; // 54 ������ ������ �������(�����)
		telemetry_values[53][1].byte = 4;
		telemetry_values[53][2].type = { "Float" };
		telemetry_values[54][0].value = { "CAN_EngineTurns" }; // 55 ������� ��������� "CAN_Temp"
		telemetry_values[54][1].byte = 2;
		telemetry_values[54][2].type = { "U16" };
		telemetry_values[55][0].value = { "CAN_Temp" }; // 56 ����������� ����������� �������� (���������)
		telemetry_values[55][1].byte = 1;
		telemetry_values[55][2].type = { "I8" };
		telemetry_values[56][0].value = { "CAN_FullRun" }; // 57 ������ ������ ��
		telemetry_values[56][1].byte = 4;
		telemetry_values[56][2].type = { "Float" };
		telemetry_values[57][0].value = { "CAN_AxleLoad_1" }; // 58 �������� �� ��� 1
		telemetry_values[57][1].byte = 2;
		telemetry_values[57][2].type = { "U16" };
		telemetry_values[58][0].value = { "CAN_AxleLoad_2" }; // 59 �������� �� ��� 2
		telemetry_values[58][1].byte = 2;
		telemetry_values[58][2].type = { "U16" };
		telemetry_values[59][0].value = { "CAN_AxleLoad_3" }; // 60 �������� �� ��� 3
		telemetry_values[59][1].byte = 2;
		telemetry_values[59][2].type = { "U16" };
		telemetry_values[60][0].value = { "CAN_AxleLoad_4" }; // 61 �������� �� ��� 4
		telemetry_values[60][1].byte = 2;
		telemetry_values[60][2].type = { "U16" };
		telemetry_values[61][0].value = { "CAN_AxleLoad_5" }; // 62 �������� �� ��� 5
		telemetry_values[61][1].byte = 2;
		telemetry_values[61][2].type = { "U16" };
		telemetry_values[62][0].value = { "CAN_PedalAccel" }; // 63 ��������� ������ ����
		telemetry_values[62][1].byte = 1;
		telemetry_values[62][2].type = { "U8" };
		telemetry_values[63][0].value = { "CAN_PedalStop" }; // 64  ��������� ������ �������
		telemetry_values[63][1].byte = 1;
		telemetry_values[63][2].type = { "U8" };
		telemetry_values[64][0].value = { "CAN_EngineLoad" }; // 65  �������� �� ��������� 
		telemetry_values[64][1].byte = 1;
		telemetry_values[64][2].type = { "U8" };
		telemetry_values[65][0].value = { "CAN_LevelFiltr" }; // 66  ������� �������� � ��������� ������� ��������� �����
		telemetry_values[65][1].byte = 2;
		telemetry_values[65][2].type = { "U16" };
		telemetry_values[66][0].value = { "CAN_EngineTime" }; // 67 ������ ����� ������ ���������
		telemetry_values[66][1].byte = 4;
		telemetry_values[66][2].type = { "U32" };
		telemetry_values[67][0].value = { "CAN_TimeTO" };  // 68 ���������� �� ��
		telemetry_values[67][1].byte = 2;
		telemetry_values[67][2].type = { "I16" };
		telemetry_values[68][0].value = { "CAN_Speed" }; // 69 �������� ��
		telemetry_values[68][1].byte = 1;
		telemetry_values[68][2].type = { "U8" };

		int x = 0;
		for (int i = 0; i < 69; i++) {
			if ((bool)bitfield[i] == 1) {
				x += telemetry_values[i][1].byte;
			}
		}
		cout << "x:= " << x << " bytes" << endl;

		char buff_3_1[2];									// 3 ����� � ������� ����������
		result = recv(client_socket, buff_3_1, 2, 0);		//������� ����� � ������� 3
		char index[2];
		copy(buff_3_1, buff_3_1 + 2, index);			// ��������� ������ 2 ����� �� ���������
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
