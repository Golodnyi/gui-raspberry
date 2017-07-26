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

extern void *update(void *arg);

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
    cout << "Start get telemetry" << endl;
    QSqlQuery query;
    if (!query.exec(("SELECT * FROM telemetry"))) {
        cout << "SQL Query filed: " <<  query.lastError().text().toStdString() << endl;
    }
    int i=0;
    while (query.next()) {
        telemetry_values[i].alias=query.value(0).toString();
        telemetry_values[i].byte=query.value(1).toInt();
        telemetry_values[i].type=query.value(2).toString();
        telemetry_values[i].name=query.value(3).toString();
        telemetry_values[i].unit=query.value(4).toString();
        telemetry_values[i].filter=query.value(5).toString();
        telemetry_values[i].enable=false;
        i+=1;
    }
    cout << "End get" << endl;
    return(*telemetry_values);
}

void * TelemetryConvert(dataStruct *telemetry_values) {
    cout << "Start telemetry convert" << endl;
    QSqlQuery query;
    for (int i = 0; i < 85; i++) {
        if (telemetry_values[i].enable) {
            if (telemetry_values[i].filter == "air_pressure") {
                if (!query.exec(("SELECT val, color FROM air_pressure WHERE " + telemetry_values[i].value +
                                 " BETWEEN min AND max;"))) {
                    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
                }
                while (query.next()) {
                    telemetry_values[i].value = query.value(0).toString();
                    telemetry_values[i].color = query.value(1).toString();
                }
            }
            else if (telemetry_values[i].filter.toStdString() == "oil_pressure_k19") {
                if (!query.exec(("SELECT val,color FROM oil_pressure_k19 WHERE " + telemetry_values[i].value +
                                 " BETWEEN min AND max;"))) {
                    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
                }
                while (query.next()) {
                    telemetry_values[i].value = query.value(0).toString();
                    telemetry_values[i].color = query.value(1).toString();
                }
            }
            else if (telemetry_values[i].filter.toStdString() == "oil_pressure_k50") {
                if (!query.exec(("SELECT val,color FROM oil_pressure_k50 WHERE " + telemetry_values[i].value +
                                 " BETWEEN min AND max;"))) {
                    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
                }
                while (query.next()) {
                    telemetry_values[i].value = query.value(0).toString();
                    telemetry_values[i].color = query.value(1).toString();
                }
            }
            else if (telemetry_values[i].filter.toStdString() == "vacuum_k19") {
                if (!query.exec(("SELECT val,color FROM vacuum_k19 WHERE " + telemetry_values[i].value +
                                 " BETWEEN min AND max;"))) {
                    cout << "SQL Query filed 2 : " << query.lastError().text().toStdString() << endl;
                }
                while (query.next()) {
                    telemetry_values[i].value = query.value(0).toString();
                    telemetry_values[i].color = query.value(1).toString();
                }
            }
        }
    }
    cout << "End telemetry convert" << endl;
    update(telemetry_values);
}

