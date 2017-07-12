#include <iostream>
#include <vector>
#include <QApplication>
#include <QtGui>
#include <QTableWidget>
#include <pthread.h>
#include <pthread.h>

#define ROWS 10 // количество строк
#define COLS 10 // количество столбцов таблицы
using namespace std;

extern void * flex(void *arg);

int gui_init(int argc, char *argv[]) {
    QApplication app(argc, argv); //(постоянная) приложение
    QTableWidget table(ROWS, COLS);
    table.setGridStyle(Qt::SolidLine);
    table.setFont(QFont("Times", 14, QFont::Normal));
    table.showMaximized();

    vector<char> temp_vector;
    pthread_t thread;
    int result = pthread_create(&thread, NULL, flex, &temp_vector);
    if (result != 0) {
        perror("Создание первого потока!");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < COLS; i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A' + i)));
        table.setHorizontalHeaderItem(i, item);
        for (int j = 0; j < ROWS; j++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString("(%1, %2)")
                                  .arg(QChar('A' + i)).arg(j + 1));
            table.setItem(j, i, item);
        }
    }
    //table.resizeRowsToContents();
    //table.resizeColumnsToContents();
    table.show();
    return app.exec();
}