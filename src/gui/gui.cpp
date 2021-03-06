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

QListWidget *leftWidget;
QListWidget *rightWidget;
QWidget *window;
bool sound = true;

extern void *flex(void *arg);
extern void connect(QLabel *label, string path);
extern dataStruct getTelemetry(dataStruct *telemetry_values, QLabel *label);
extern int color(dataStruct *telemetry_values, int i, QListWidgetItem *Item,
                 bool sound);
extern string PROGRAM_PATH;

void *update(void *arg, bitset<85> bitfield)
{
  window->setUpdatesEnabled(false);
  dataStruct *telemetry_values = (dataStruct *)arg;
  leftWidget->clear();
  rightWidget->clear();
  for (int i = 0; i < 85; i++)
  {
    if (telemetry_values[i].disable || !(bool)bitfield[i])
    {
      continue;
    }
    QListWidgetItem *Item = new QListWidgetItem;
    Item->setFlags(Item->flags() & ~Qt::ItemIsSelectable);
    Item->setText((telemetry_values[i].name) + ": " +
                  (telemetry_values[i].value) + " " +
                  (telemetry_values[i].unit));
    if (!telemetry_values[i].position)
    {
      leftWidget->insertItem(i, Item);
    }
    else
    {
      rightWidget->insertItem(i, Item);
    }
    color(telemetry_values, i, Item, sound);
  }
  window->setUpdatesEnabled(true);
}

string path(char *argv[])
{
  char buff[PATH_MAX];
  ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
  buff[len] = '\0';
  string path = string(buff);
  char *cstr = new char[path.length() + 1];
  strcpy(cstr, path.c_str());
  string aux(cstr);
  int pos = aux.rfind('/');
  path = aux.substr(0, pos + 1);
  PROGRAM_PATH = path;
  return path;
}

int gui_init(int argc, char *argv[])
{
  QApplication app(argc, argv); //(постоянная) приложение
  app.setOverrideCursor(Qt::BlankCursor);
  window = new QWidget;

  QLabel *leftLabel = new QLabel("Загрузка приложения");
  leftLabel->setFont(QFont("Times", 22, QFont::Normal));
  QStatusBar *leftStatusBar = new QStatusBar(window);
  leftStatusBar->addWidget(leftLabel);

  QLabel *rightLabel = new QLabel("");
  rightLabel->setFont(QFont("Times", 22, QFont::Normal));
  QStatusBar *rightStatusBar = new QStatusBar(window);
  rightStatusBar->addWidget(rightLabel);
  
  flex_args dataFlex;
  
  connect(leftLabel, path(argv));
  getTelemetry((dataStruct *)dataFlex.telemetry_values, leftLabel);

  leftWidget = new QListWidget;
  leftWidget->setFont(QFont("Times", 30, QFont::Normal));
  QListWidgetItem *Item = new QListWidgetItem;
  Item->setText(QString::fromStdString("Ожидание данных"));
  leftWidget->insertItem(0, Item);

  rightWidget = new QListWidget;
  rightWidget->setFont(QFont("Times", 30, QFont::Normal));

  QGridLayout *MainLayout = new QGridLayout();
  MainLayout->addWidget(leftWidget, 0, 0);
  MainLayout->addWidget(rightWidget, 0, 1);
  MainLayout->addWidget(leftStatusBar, 1, 0);
  MainLayout->addWidget(rightStatusBar, 1, 1);

  window->setContentsMargins(28, 30, 28, 30);
  window->setLayout(MainLayout);
  window->showFullScreen();

  pthread_t thread;
  dataFlex.leftLabel = leftLabel;
  dataFlex.rightLabel = rightLabel;

  int result = pthread_create(&thread, NULL, flex, &dataFlex);
  {
    if (result != 0)
    {
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
