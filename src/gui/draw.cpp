#include <QApplication>
#include <QListWidget>
#include <QtGui>
#include <iostream>
#include <src/struct.h>

using namespace std;

extern int gpio(dataStruct *telemetry_values, int i, bool sound);

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item,
          bool sound) {
  if (telemetry_values[i].color == "red") {
    if (!telemetry_values[i].problemTime) {
      telemetry_values[i].problemTime = time(NULL);
    }

    Item->setText(
      telemetry_values[i].name + ": " + telemetry_values[i].value +
      " " + telemetry_values[i].unit + "\n" +
      QString::fromStdString(
        to_string(
          (int)difftime(time(NULL), telemetry_values[i].problemTime)
        )
      ) +
      " сек. Назад"
    );
    Item->setBackground(Qt::red);
  } else if (telemetry_values[i].color == "yellow") {
    Item->setBackground(Qt::yellow);
  } else if (telemetry_values[i].color == "lightGray") {
    Item->setBackground(Qt::lightGray);
    Item->setText(((telemetry_values[i].name) + ": " +
                   (telemetry_values[i].value) + " (не переведено)"));
  }
  if (telemetry_values[i].color != "red" && telemetry_values[i].problemTime) {
    telemetry_values[i].problemTime = 0;
  }
  gpio(telemetry_values, i, sound);
}
