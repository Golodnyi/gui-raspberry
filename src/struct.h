#ifndef GUIRASPBERRY_STRUCT_H
#define GUIRASPBERRY_STRUCT_H

#include <QtGui>
#include <QLabel>
#include <iostream>
using namespace std;

struct dataStruct
{
  QString alias;  // англ название (numPage)
  int byte;       // кол-во байт
  QString type;   // тип предоставления данных
  QString name;   // название датчика на русском (температура блока цилиндров)
  QString unit;   // мера измерения (км в час, цельясия...)
  QString filter; // перевод значений
  int position; //позиция в listview (0 - слева, 1 - справа)
  bool disable;    // значение из сокета
  QString value;  // значение датчика
  QString color;
  time_t problemTime = 0;
};

struct TResult
{
  char preamble[4];
  uint32_t IDr;
  uint32_t IDs;
  uint16_t size;
  uint8_t CSd;
  uint8_t CSp;
  char buff[19]; //массив для пакета данных
};

struct flex_args
{
  dataStruct telemetry_values[85];
  QLabel *leftLabel;
  QLabel *rightLabel;
};

#endif // GUIRASPBERRY_STRUCT_H
