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
    bool enable; // значение из сокета
    QString value; // значение из сокета

};

QSqlDatabase sdb;

void connect() {
    sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("gr.db");

    if (!sdb.open()) {
        cout << "Filed sql lite" << endl;
        exit(0);
    }
}

dataStruct getTelemetry(dataStruct *telemetry_values) {
    cout << "Connect to db" << endl;
    QSqlQuery query;
    if (!query.exec(("SELECT * FROM `telemetry`"))) {
        cout << "SQL Query filed: " <<  query.lastError().text().toStdString() << endl;
    }
    int i=0;
    while (query.next()) {
        QString row = query.value(1).toString();
        telemetry_values[i].alias=row;
        int row_byte = query.value(2).toInt();
        telemetry_values[i].byte=row_byte;
        row = query.value(3).toString();
        telemetry_values[i].type=row;
        row = query.value(4).toString();
        telemetry_values[i].name=row;
        row = query.value(5).toString();
        telemetry_values[i].unit=row;
        telemetry_values[i].enable=false;
        i+=1;
    }
    cout << "End connect" << endl;
    return(*telemetry_values);
}

