#include <QtGui>
#include <QtSql/QtSql>
#include <iostream>
#include <src/struct.h>

using namespace std;

void derive(QString tab, dataStruct *telemetry_values, int i)
{
  if (!telemetry_values[i].filter.length()) {
    return;
  }

  if (telemetry_values[i].filter == "motochas")
  {
    telemetry_values[i].value = QString::number((int)(telemetry_values[i].value.toInt() / 3600));
    return;
  } 

  QSqlQuery query;
  if (!query.exec(("SELECT val, color FROM " + tab + " WHERE " +
                   telemetry_values[i].value + " BETWEEN min AND max;")))
  {
    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString()
         << endl;
  }

  if (query.next())
  {
    telemetry_values[i].value = query.value(0).toString();
    telemetry_values[i].color = query.value(1).toString();
  }
  else
  {
    telemetry_values[i].color = "lightGray";
  }
  return;
}