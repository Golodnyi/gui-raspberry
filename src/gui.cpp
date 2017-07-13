#include <iostream>
#include <vector>
#include <QApplication>
#include <QtGui>
#include <QTableWidget>
#include <pthread.h>
#include <unistd.h>

#define ROWS 10 // количество строк
#define COLS 10 // количество столбцов таблицы
using namespace std;

extern void * flex(void *arg);
void * draw(void *arg) {
    vector<char> *const temp_vector = (vector<char> *)arg;

    while (true) {
        cout << "Draw size: " << temp_vector->size() << endl;
        cout << "Draw address: " << temp_vector << endl;
        usleep(1000000);
    }
}
int gui_init(int argc, char *argv[]) {
    QApplication app(argc, argv); //(постоянная) приложение
    QTableWidget table(ROWS, COLS);
    table.setGridStyle(Qt::SolidLine);
    table.setFont(QFont("Times", 14, QFont::Normal));

    vector<char> temp_vector;
    pthread_t thread;
    int result = pthread_create(&thread, NULL, flex, (void*)&temp_vector);
    if (result != 0) {
        perror("Создание первого потока!");
        return EXIT_FAILURE;
    }

    pthread_t thread2;
    int result2 = pthread_create(&thread2, NULL, draw, (void*)&temp_vector);
    if (result2 != 0) {
        perror("Создание второго потока!");
        return EXIT_FAILURE;
    }


    QTableWidgetItem *item = new QTableWidgetItem;

    for (int i = 0; i < COLS; i++) {
        item->setText(QString(QChar('A' + i)));
        table.setHorizontalHeaderItem(i, item);
        for (int j = 0; j < ROWS; j++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString("(%1, %2)")
                                  .arg(QChar('A' + i)).arg(j + 1));
            table.setItem(j, i, item);
        }
    }
    table.showMaximized();
    return app.exec();
}