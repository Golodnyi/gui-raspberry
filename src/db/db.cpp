#include <QLabel>
#include <QtGui>
#include <QtSql/QtSql>
#include <bitset>
#include <iostream>
#include <src/struct.h>

using namespace std;

extern QString derive(QString tab, dataStruct *telemetry_values, int i);
extern void *update(void *arg, bitset<85> bitfield);

QSqlDatabase sdb;

void connect(QLabel *label, string path) {
  cout << path << endl;
  sdb = QSqlDatabase::addDatabase("QSQLITE");
  sdb.setDatabaseName(QString::fromStdString(path + "gr.db"));

  if (!sdb.open()) {
    cout << "Filed sql lite" << endl;
    label->setText("Ошибка: база данных не найдена.");
    exit(1);
  }
  cout << "Connect to db" << endl;
  label->setText("База данных загружена.");
}

dataStruct getTelemetry(dataStruct *telemetry_values, QLabel *label) {
  cout << "Start get telemetry" << endl;
  QSqlQuery query;
  if (!query.exec(("SELECT * FROM telemetry"))) {
    label->setText(QString::fromStdString(
        "Ошибка: SQL Query filed: " + query.lastError().text().toStdString()));
    cout << "SQL Query filed: " << query.lastError().text().toStdString()
         << endl;
    exit(1);
  }
  label->setText("Телеметрические данные загружены");
  int i = 0;
  while (query.next()) {
    telemetry_values[i].alias = query.value(0).toString();
    telemetry_values[i].byte = query.value(1).toInt();
    telemetry_values[i].type = query.value(2).toString();
    telemetry_values[i].name = query.value(3).toString();
    telemetry_values[i].unit = query.value(4).toString();
    telemetry_values[i].filter = query.value(5).toString();
    telemetry_values[i].enable = query.value(6).toBool();
    ;
    i += 1;
  }
  cout << "End get" << endl;
  return (*telemetry_values);
}

void *TelemetryConvert(dataStruct *telemetry_values, bitset<85> bitfield) {
  cout << "Start telemetry convert" << endl;
  QSqlQuery query;
  for (int i = 0; i < 85; i++) {
    if (telemetry_values[i].enable and (bool) bitfield[i] == 1) {
      derive(telemetry_values[i].filter, telemetry_values, i);
    }
  }
  cout << "End telemetry convert" << endl;
  update(telemetry_values, bitfield);
}
