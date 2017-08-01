#include <iostream>
#include <QtGui>
#include <QListWidget>
#include <src/struct.h>

using namespace std;

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item) {
    QTime time;
    time.setHMS (0, 0, 0);
    time.start();
    if (telemetry_values[i].color == "red") {
        //while (true) {
        //   Item->setText((telemetry_values[i].name)
        //                + ": " + (telemetry_values[i].value)
        //               + " " + (telemetry_values[i].unit) + " " + (QString:: fromStdString(to_string(time.elapsed()))));
        //  QApplication::beep;
        //  usleep(1000000);
        // }
        Item->setBackground(Qt::red);
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
