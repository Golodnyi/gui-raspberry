#include <QtGui>
#include <QtSql/QtSql>
#include <iostream>
#include <src/struct.h>

using namespace std;

QString derive(QString tab, dataStruct *telemetry_values, int i) {
  QSqlQuery query;
  if (!query.exec(("SELECT val, color FROM " + tab + " WHERE " +
                   telemetry_values[i].value + " BETWEEN min AND max;"))) {
    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString()
         << endl;
  }

  if (query.next()) {
    telemetry_values[i].value = query.value(0).toString();
    telemetry_values[i].color = query.value(1).toString();
  } else {
    cout << "telemetry_values[" << i << "]."
         << telemetry_values[i].name.toStdString()
         << " not included in the range" << endl;
    telemetry_values[i].color = "lightGray";
  }
  return (telemetry_values[i].value, telemetry_values[i].color);
}