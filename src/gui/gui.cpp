#include <QApplication>
#include <QGridLayout>
#include <QListWidget>
#include <QtGui>
#include <QLabel>
#include <QStatusBar>
#include <bitset>
#include <iostream>
#include <src/struct.h>

using namespace std;

QListWidget *listWidget;
QListWidget *listWidget1;

extern void *flex(void *arg);
extern void connect(QLabel *label);
extern dataStruct getTelemetry(dataStruct *telemetry_values, QLabel *label);
extern int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item);

void *update(void *arg, bitset<85> bitfield) {
  dataStruct *telemetry_values = (dataStruct *)arg;
  cout << "update" << endl;
  short int x = 0;
  short int y = 0;
  listWidget->clear();
  listWidget1->clear();
  for (int i = 0; i < 85; i++) {
    if (telemetry_values[i].enable and (bool) bitfield[i] == 1) {
      x = listWidget->count();
      y = listWidget1->count();
      if (x > y) {
        QListWidgetItem *Item1 = new QListWidgetItem;
        Item1->setText((telemetry_values[i].name) + ": " +
                       (telemetry_values[i].value) + " " +
                       (telemetry_values[i].unit));
        listWidget1->insertItem(i, Item1);

        color(telemetry_values, i, Item1);
      } else {
        QListWidgetItem *Item = new QListWidgetItem;
        Item->setText((telemetry_values[i].name) + ": " +
                      (telemetry_values[i].value) + " " +
                      (telemetry_values[i].unit));
        listWidget->insertItem(i, Item);

        color(telemetry_values, i, Item);
      }
      cout << telemetry_values[i].value.toStdString() << endl;
    }
  }
  cout << "end update" << endl;
}
int gui_init(int argc, char *argv[]) {
  QApplication app(argc, argv); //(постоянная) приложение

  QWidget *window = new QWidget;
  QLabel *label = new QLabel("Загрузка приложения");
  QStatusBar *statusBar = new QStatusBar(window);
  statusBar->addWidget(label);

  dataStruct telemetry_values[85];

  connect(label);
  getTelemetry((dataStruct *)telemetry_values, label);

  listWidget = new QListWidget;
  listWidget->setFont(QFont("Times", 16, QFont::Normal));
  QListWidgetItem *Item = new QListWidgetItem;
  Item->setText(QString::fromStdString("Ожидание данных"));
  listWidget->insertItem(0, Item);

  listWidget1 = new QListWidget;
  listWidget1->setFont(QFont("Times", 16, QFont::Normal));
  QListWidgetItem *Item1 = new QListWidgetItem;
  listWidget1->insertItem(0, Item1);

  QGridLayout *MainLayout = new QGridLayout();
  MainLayout->addWidget(listWidget, 0, 0);
  MainLayout->addWidget(listWidget1, 0, 1);
  MainLayout->addWidget(statusBar, 1, 0);

  window->setLayout(MainLayout);
  window->showFullScreen();

  pthread_t thread;
  int result = pthread_create(&thread, NULL, flex, &telemetry_values);
  {
    if (result != 0) {
      perror("Создание первого потока!");
      return EXIT_FAILURE;
    }
    pthread_detach(thread);
  }

  qRegisterMetaType<QVector<int>>("QVector<int>");
  int code = app.exec();
  return code;
}
