#include <iostream>
#include <QtGui>
#include <QListWidget>
#include <QGridLayout>

using namespace std;

struct dataStruct {
    QString alias; // англ название (numPage)
    int byte;
    QString type;
    QString name; // название датчика на русском (температура блока цилиндров)
    QString unit; // мера измерения (км в час, цельясия...)
    QString filter; // перевод значений
    bool enable; // значение из сокета
    QString value; // значение датчика
    QString color;

};

void color(dataStruct *telemetry_values, int i, QListWidgetItem *Item) {
    if (telemetry_values[i].color == "red") {
        Item->setBackground(Qt::red);
    } else if (telemetry_values[i].color == "yellow") {
        Item->setBackground(Qt::yellow);
    } else if (telemetry_values[i].color == "lightGray") {
        Item->setBackground(Qt::lightGray);
        Item->setText(((telemetry_values[i].name) + ": " + (telemetry_values[i].value) + " (не переведено)"));
    } else {
        Item->setBackground(Qt::green);
    }
}
