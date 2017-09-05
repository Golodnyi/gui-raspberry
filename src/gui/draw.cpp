#include <QApplication>
#include <QListWidget>
#include <QtGui>
#include <iostream>
#include <src/struct.h>

using namespace std;

extern int gpio(dataStruct *telemetry_values, int i, bool sound);

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item,
          bool sound) {
  QTime time;
  time.setHMS(0, 0, 0);
  time.start();
  Item->setFlags(Item->flags() & ~Qt::ItemIsSelectable);
  gpio(telemetry_values, i, sound);
  if (telemetry_values[i].color == "red") {
    Item->setBackground(Qt::red);
  } else if (telemetry_values[i].color == "yellow") {
    Item->setBackground(Qt::yellow);
  } else if (telemetry_values[i].color == "lightGray") {
    Item->setBackground(Qt::lightGray);
    Item->setText(((telemetry_values[i].name) + ": " +
                   (telemetry_values[i].value) + " (не переведено)"));
  }
}
