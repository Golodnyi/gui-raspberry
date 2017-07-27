#include <iostream>
#include <QtSql/QtSql>
#include <QtGui>
#include <bitset>

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

extern QString derive(QString tab, dataStruct *telemetry_values, int i);
extern void *update(void *arg, bitset<85> bitfield);

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
        telemetry_values[i].enable=query.value(6).toBool();;
        i+=1;
    }
    cout << "End get" << endl;
    return(*telemetry_values);
}

void * TelemetryConvert(dataStruct *telemetry_values, bitset<85> bitfield) {
    cout << "Start telemetry convert" << endl;
    QSqlQuery query;
    QString tab1= "air_pressure";
    QString tab2="oil_pressure_k19";
    QString tab3="oil_pressure_k50";
    QString tab4="vacuum_k19";
    for (int i = 0; i < 85; i++) {
        if (telemetry_values[i].enable and (bool) bitfield[i] == 1) {
            if (telemetry_values[i].filter == tab1) {
                derive(tab1, telemetry_values , i );
            }
            else if (telemetry_values[i].filter == tab2) {
                derive(tab2, telemetry_values , i );
            }
            else if (telemetry_values[i].filter == tab3) {
                derive(tab3, telemetry_values , i );
            }
            else if (telemetry_values[i].filter == tab4) {
                derive(tab4, telemetry_values , i );
            }
        }
    }
    cout << "End telemetry convert" << endl;
    update(telemetry_values, bitfield);
}


