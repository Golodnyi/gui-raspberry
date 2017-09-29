#include <QLabel>
#include <QtGui>
#include <QtSql/QtSql>
#include <bitset>
#include <iostream>
#include <src/struct.h>

using namespace std;

extern QString derive(QString tab, dataStruct *telemetry_values, int i);
extern void *update(void *arg, bitset<85> bitfield);
extern int PIN;

QSqlDatabase sdb;

void getPin(QLabel *label)
{
  QSqlQuery query;
  if (!query.exec(("SELECT port FROM raspberry WHERE funk='speaker'")))
  {
    cout << "SQL Query filed: " << query.lastError().text().toStdString()
         << endl;
  }
  while (query.next())
  {
    PIN = query.value(0).toInt();
    label->setText("Номер PIN загружен.");
  }
}

void connect(QLabel *label, string path)
{
  cout << path << endl;
  sdb = QSqlDatabase::addDatabase("QSQLITE");
  sdb.setDatabaseName(QString::fromStdString(path + "gr.db"));

  if (!sdb.open())
  {
    cout << "Filed sql lite" << endl;
    label->setText("Ошибка: база данных не найдена.");
  }
  cout << "Connect to db" << endl;
  label->setText("База данных загружена.");
  getPin(label);
}

dataStruct getTelemetry(dataStruct *telemetry_values, QLabel *label)
{
  cout << "Start get telemetry" << endl;
  QSqlQuery query;
  if (!query.exec(("SELECT * FROM telemetry")))
  {
    label->setText(QString::fromStdString(
        "Ошибка: SQL Query filed: " + query.lastError().text().toStdString()));
    cout << "SQL Query filed: " << query.lastError().text().toStdString()
         << endl;
  }
  label->setText("База данных загружена");
  int i = 0;
  while (query.next())
  {
    telemetry_values[i].alias = query.value(0).toString();
    telemetry_values[i].byte = query.value(1).toInt();
    telemetry_values[i].type = query.value(2).toString();
    telemetry_values[i].name = query.value(3).toString();
    telemetry_values[i].unit = query.value(4).toString();
    telemetry_values[i].filter = query.value(5).toString();
    telemetry_values[i].disable = query.value(6).toBool();
    telemetry_values[i].position = query.value(7).toInt();
    ;
    i += 1;
  }
  cout << "End get" << endl;
  return (*telemetry_values);
}

void *TelemetryConvert(dataStruct *telemetry_values, bitset<85> bitfield)
{
  cout << "Start telemetry convert" << endl;
  QSqlQuery query;
  for (int i = 0; i < 85; i++)
  {
    if (telemetry_values[i].disable || !(bool)bitfield[i])
    {
      continue;
    }
    /**
       * TODO: Моточасы переводятся в часы хардкодом
       **/
    if (telemetry_values[i].alias == "Motochas")
    {
      telemetry_values[i].value = QString::number((int)(telemetry_values[i].value.toInt() / 3600));
    }
    if (telemetry_values[i].filter.length())
    {
      derive(telemetry_values[i].filter, telemetry_values, i);
    }
  }
  cout << "End telemetry convert" << endl;
  update(telemetry_values, bitfield);
}
