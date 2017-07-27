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
    cout << "Start derive " << endl;
    QSqlQuery query;
    if (!query.exec(("SELECT val, color FROM " + tab + " WHERE " + telemetry_values[i].value +
                     " BETWEEN min AND max;"))) {
        cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
    }
    while (query.next()) {
        telemetry_values[i].value = query.value(0).toString();
        telemetry_values[i].color = query.value(1).toString();
    }
    cout << "color= " << telemetry_values[i].color.toStdString() << endl;
    cout << "value= " << telemetry_values[i].value.toStdString() << endl;

    return(telemetry_values[i].color);
}

