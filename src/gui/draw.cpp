#include <iostream>
#include <QtGui>
#include <QListWidget>
#include <src/struct.h>
#include <QApplication>

using namespace std;

extern int gpio();

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item) {
    QTime time;
    time.setHMS (0, 0, 0);
    time.start();
    if (telemetry_values[i].color == "red") {
        Item->setBackground(Qt::red);
        gpio();
        cout<< "звук" << endl;
    }
    else if (telemetry_values[i].color == "yellow") {
        Item->setBackground(Qt::yellow);
    }
    else if (telemetry_values[i].color == "lightGray") {
        Item->setBackground(Qt::lightGray);
        Item->setText(((telemetry_values[i].name) + ": " + (telemetry_values[i].value) + " (не переведено)"));
    }
    else {
        Item->setBackground(Qt::green);
    }
}
