#include <iostream>
#include <QApplication>
#include <QtGui>
#include <QGridLayout>
#include <QListWidget>
#include <bitset>

QListWidget *listWidget;
QListWidget *listWidget1;

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
extern void color(dataStruct *telemetry_values, int i, QListWidgetItem *Item);
extern void * flex(void *arg);
extern void close_socket();
extern void connect();
extern dataStruct getTelemetry(dataStruct *telemetry_values);

void * update(void *arg, bitset<85> bitfield) {
    dataStruct* telemetry_values = (dataStruct *)arg;
    cout << "update" << endl;
    short int x=0;
    short int y=0;
    listWidget->clear();
    listWidget1->clear();
    cout << "update1" << endl;
    for (int i = 0; i < 85; i++) {
        if (telemetry_values[i].enable and (bool) bitfield[i] == 1) {
            cout << "update1" << endl;
            x=listWidget->count();
            y=listWidget1->count();
            cout << "update2" << endl;
            if(x>y){
                QListWidgetItem *Item1 = new QListWidgetItem;
                Item1->setText((telemetry_values[i].name)
                                  + ": " + (telemetry_values[i].value)
                                  + " " + (telemetry_values[i].unit));
                listWidget1->insertItem(i, Item1);
                color(telemetry_values, i, Item1);
            }
            else {
                QListWidgetItem *Item = new QListWidgetItem;
                Item->setText((telemetry_values[i].name)
                              + ": " + (telemetry_values[i].value)
                              + " " + (telemetry_values[i].unit));
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

    dataStruct telemetry_values[85];

    connect();
    getTelemetry((dataStruct*) telemetry_values);

    QWidget *window = new QWidget;
    QListWidget *listWidget = new QListWidget;
    listWidget->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item = new QListWidgetItem;
    Item->setText(QString::fromStdString("Ожидание данных"));
    listWidget->insertItem(0, Item);
    *listWidget;

    QListWidget *listWidget1 = new QListWidget;
    listWidget1->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item1 = new QListWidgetItem;
    Item1->setText(QString::fromStdString("Ожидание данных"));
    listWidget1->insertItem(0, Item1);
    *listWidget1;

    QGridLayout *MainLayout = new QGridLayout();
    MainLayout->addWidget(listWidget, 0, 0);
    MainLayout->addWidget(listWidget1, 0, 1);

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

    qRegisterMetaType<QVector<int> >("QVector<int>");

    int code = app.exec();
    close_socket();
    return code;
}

