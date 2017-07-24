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
    cout << "Connect to db" << endl;
}

dataStruct getTelemetry(dataStruct *telemetry_values) {
    cout << "Connect to telemetry" << endl;
    QSqlQuery query;
    if (!query.exec(("SELECT alias,byte,type,name,unit FROM telemetry"))) {
        cout << "SQL Query filed: " <<  query.lastError().text().toStdString() << endl;
    }
    int i=0;
    while (query.next()) {
        telemetry_values[i].alias=query.value(0).toString();
        telemetry_values[i].byte=query.value(1).toInt();
        telemetry_values[i].type=query.value(2).toString();
        telemetry_values[i].name=query.value(3).toString();
        telemetry_values[i].unit=query.value(4).toString();
        telemetry_values[i].enable=false;
        i+=1;
    }
    cout << "End connect" << endl;
    return(*telemetry_values);
}

