#include <iostream>
#include <QtSql/QtSql>
#include <QtGui>

using namespace std;

struct dataStruct {
    QString alias; // англ название (numPage)
    int byte; // кол-во байт
    QString type; // тип предоставления данных
    QString name; // название датчика на русском (температура блока цилиндров)
    QString unit; // мера измерения (км в час, цельясия...)
    QString filter; // перевод значений
    bool enable; // значение из сокета
    QString value; // значение датчика
    QString color;
};

QString derive(QString tab, dataStruct *telemetry_values, int i){
    QSqlQuery query;
    if (!query.exec(("SELECT val, color FROM " + tab + " WHERE " + telemetry_values[i].value +
                     " BETWEEN min AND max;"))) {
        cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
    }
    if (query.next()) {
        cout << "Start derive telemetry_values[" << i << "]." << telemetry_values[i].name.toStdString() << endl;
        telemetry_values[i].value = query.value(0).toString();
        telemetry_values[i].color = query.value(1).toString();
    }
    else{
        cout << "telemetry_values[" << i << "]." << telemetry_values[i].name.toStdString() << " not included in the range" << endl;
        telemetry_values[i].color = "lightGray";
    }
    return(telemetry_values[i].value,telemetry_values[i].color);
}

