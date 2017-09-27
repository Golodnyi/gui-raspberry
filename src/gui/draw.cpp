#include <QApplication>
#include <QListWidget>
#include <QtGui>
#include <iostream>
#include <src/struct.h>
#include <string>

using namespace std;

extern void *warning(void *arg);
extern void *danger(void *arg);

string timeConv(int s)
{
  int m, h;
  m = (s / 60) % 60;
  h = (s / 360) % 24;
  s = s % 60;
  if (h)
  {
    return to_string(h) + " ч. " + to_string(m) + " м. " + to_string(s) + " с. назад";
  }
  else if (m)
  {
    return to_string(m) + " м. " + to_string(s) + " с. назад";
  }

  return to_string(s) + " с. назад";
}

int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item,
          bool sound)
{
  if (telemetry_values[i].color == "red")
  {
    if (!telemetry_values[i].problemTime)
    {
      telemetry_values[i].problemTime = time(NULL);
    }
    int seconds = (int)(difftime(time(NULL), telemetry_values[i].problemTime) + 1);
    Item->setText(telemetry_values[i].name + ": " + telemetry_values[i].value + " " + telemetry_values[i].unit + "\n" + QString::fromStdString(timeConv(seconds)));
    Item->setBackground(Qt::red);
  }
  else if (telemetry_values[i].color == "yellow")
  {
    Item->setBackground(Qt::yellow);
  }
  else if (telemetry_values[i].color == "lightGray")
  {
    Item->setBackground(Qt::lightGray);
    Item->setText(((telemetry_values[i].name) + ": " +
                   (telemetry_values[i].value) + " (не переведено)"));
  }
  if (telemetry_values[i].color != "red" && telemetry_values[i].problemTime)
  {
    telemetry_values[i].problemTime = 0;
  }

  if (sound)
  {
    pthread_t danger_thread;
    pthread_t warning_thread;

    if (telemetry_values[i].color == "red")
    {
      int result = pthread_create(&danger_thread, NULL, danger, NULL);
      {
        if (result != 0)
        {
          return EXIT_FAILURE;
        }
        pthread_detach(danger_thread);
      }
    }
    else if (telemetry_values[i].color == "yellow")
    {
      int result = pthread_create(&warning_thread, NULL, warning, NULL);
      {
        if (result != 0)
        {
          return EXIT_FAILURE;
        }
        pthread_detach(warning_thread);
      }
    }
  }
}
