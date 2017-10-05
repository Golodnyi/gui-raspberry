#include <QtGui>
#include <QtSql/QtSql>
#include <iostream>
#include <src/struct.h>

using namespace std;

void derive(QString tab, dataStruct *telemetry_values, int i)
{
  if (!telemetry_values[i].filter.length())
  {
    return;
  }
  int value = telemetry_values[i].value.toInt();
  if (telemetry_values[i].filter == "motochas")
  {
    telemetry_values[i].value = QString::number(value / 3600);
    return;
  }
  else if (telemetry_values[i].filter == "air_pressure")
  {
    if (value < 500)
    {
      telemetry_values[i].value = QString::number(0);
    }
    else
    {
      telemetry_values[i].value = QString::number((value / 1000 - 0.5) * 2);
    }
    return;
  }
  else if (telemetry_values[i].filter == "vacuum_k19")
  {
    telemetry_values[i].value = QString::number((0.2 * value - 2.8) / 1000);
    return;
  }
  else if (telemetry_values[i].filter == "oil_pressure_k19")
  {
    if (value < 1900) {
      telemetry_values[i].value = QString::number(0);
    } else {
      telemetry_values[i].value = QString::number(-1E-10 * pow(value, 3) + 2E-06 * pow(value, 2) - 0.0112 * value + 24.666);
    }
    return;
  }

  QSqlQuery query;
  if (!query.exec(("SELECT val, color FROM " + tab + " WHERE " + telemetry_values[i].value + " BETWEEN min AND max;")))
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