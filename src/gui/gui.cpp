#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QPushButton>
#include <QStatusBar>
#include <QtGui>
#include <bitset>
#include <iostream>
#include <limits.h>
#include <src/struct.h>
#include <string>
#include <unistd.h>

using namespace std;

QListWidget *listWidget;
QListWidget *listWidget1;
QPushButton *soundBtn;
bool sound = true;

extern void *flex(void *arg);
extern void connect(QLabel *label, string path);
extern dataStruct getTelemetry(dataStruct *telemetry_values, QLabel *label);
extern int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item,
                 bool sound);
void soundBtnClick() {
  sound = !sound;
  if (sound) {
    soundBtn->setText("Отключить звук");
  } else {
    soundBtn->setText("Включить звук");
  }
}
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

        color(telemetry_values, i, Item1, sound);
      } else {
        QListWidgetItem *Item = new QListWidgetItem;
        Item->setText((telemetry_values[i].name) + ": " +
                      (telemetry_values[i].value) + " " +
                      (telemetry_values[i].unit));
        listWidget->insertItem(i, Item);

        color(telemetry_values, i, Item, sound);
      }
      cout << telemetry_values[i].value.toStdString() << endl;
    }
  }
  cout << "end update" << endl;
}

string path(char *argv[]) {
  char buff[PATH_MAX];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
  if (len == -1) {
    exit(1);
  }
  buff[len] = '\0';
  string path = string(buff);
  char *cstr = new char[path.length() + 1];
  strcpy(cstr, path.c_str());
  string aux(cstr);
  int pos = aux.rfind('/');
  path = aux.substr(0, pos + 1);
  return path;
}

int gui_init(int argc, char *argv[]) {
  QApplication app(argc, argv); //(постоянная) приложение
  app.changeOverrideCursor(Qt::BlankCursor);
  QWidget *window = new QWidget;

  QLabel *label = new QLabel("Загрузка приложения");
  QStatusBar *statusBar = new QStatusBar(window);
  statusBar->addWidget(label);

  soundBtn = new QPushButton("Отключить звук", window);
  soundBtn->setMinimumHeight(40);
  QObject::connect(soundBtn, &QPushButton::clicked, soundBtnClick);
  QStatusBar *buttonsBar = new QStatusBar(window);
  buttonsBar->addWidget(soundBtn);

  dataStruct telemetry_values[85];

  connect(label, path(argv));
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
  MainLayout->addWidget(buttonsBar, 1, 1);

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
  app.restoreOverrideCursor();
  return code;
}
