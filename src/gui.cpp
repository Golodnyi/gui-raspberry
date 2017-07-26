#include <iostream>
#include <QApplication>
#include <QtGui>
#include <QListWidget>
#include <QGridLayout>

using namespace std;

QListWidget *listWidget;
QListWidget *listWidget1;
struct dataStruct {
    QString alias; // англ название (numPage)
    int byte;
    QString type;
    QString name; // название датчика на русском (температура блока цилиндров)
    QString unit; // мера измерения (км в час, цельясия...)
    QString filter; // перевод значений
    bool enable; // значение из сокета
    QString value; // значение датчика

};

extern void * flex(void *arg);
extern void close_socket();
extern void connect();
extern dataStruct getTelemetry(dataStruct *telemetry_values);

void * update(void *arg) {
    dataStruct* telemetry_values = (dataStruct *)arg;
    cout << "update" << endl;
    short int x=0;
    short int y=0;
    listWidget->clear();
    listWidget1->clear();
    for (int i = 0; i < 85; i++) {
        if (telemetry_values[i].enable) {
            x=listWidget->count();
            y=listWidget1->count();
            if(x>y){
                QListWidgetItem *Item1 = new QListWidgetItem;
                Item1->setText((telemetry_values[i].name)
                                  + ": " + (telemetry_values[i].value)
                                  + " " + (telemetry_values[i].unit));

                listWidget1->insertItem(i, Item1);
                if (i == 2) {
                    Item1->setBackground(Qt::red);
                } else {
                    Item1->setBackground(Qt::green);
                }
            }
            else {
                QListWidgetItem *Item = new QListWidgetItem;
                Item->setText((telemetry_values[i].name)
                              + ": " + (telemetry_values[i].value)
                              + " " + (telemetry_values[i].unit));

                listWidget->insertItem(i, Item);
                if (i == 2) {
                    Item->setBackground(Qt::red);
                } else {
                    Item->setBackground(Qt::green);
                }
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
    listWidget = new QListWidget;
    listWidget->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item = new QListWidgetItem;
    Item->setText(QString::fromStdString("Ожидание данных"));
    listWidget->insertItem(0, Item);

    listWidget1 = new QListWidget;
    listWidget1->setFont(QFont("Times", 16, QFont::Normal));
    QListWidgetItem *Item1 = new QListWidgetItem;
    Item1->setText(QString::fromStdString("Ожидание данных"));
    listWidget1->insertItem(0, Item1);

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
