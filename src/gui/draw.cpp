#include <QApplication>
#include <QListWidget>
#include <QtGui>
#include <iostream>
#include <src/struct.h>

using namespace std;

extern int gpio(dataStruct *telemetry_values, int i);

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item) {
  QTime time;
  time.setHMS(0, 0, 0);
  time.start();
  if (telemetry_values[i].color == "red") {
    Item->setBackground(Qt::red);
    gpio(telemetry_values, i);
    cout << "звук" << endl;
  } else if (telemetry_values[i].color == "yellow") {
    gpio(telemetry_values, i);
    Item->setBackground(Qt::yellow);
  } else if (telemetry_values[i].color == "lightGray") {
    gpio(telemetry_values, i);
    Item->setBackground(Qt::lightGray);
    Item->setText(((telemetry_values[i].name) + ": " +
                   (telemetry_values[i].value) + " (не переведено)"));
  } else {
    Item->setBackground(Qt::green);
  }
}
