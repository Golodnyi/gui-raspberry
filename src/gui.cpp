#include <QApplication>
#include <QtGui>
#include <QTableWidget>

#define ROWS 10 // ���������� �����
#define COLS 10 // ���������� �������� �������

int gui_init(int argc, char *argv[]) {
    QApplication app(argc, argv); //(����������) ����������
    QTableWidget table(ROWS, COLS);
    table.setGridStyle(Qt::SolidLine);
    table.setFont(QFont("Times", 14, QFont::Normal));
    table.showMaximized();
    for (int i=0; i<COLS; i++) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A' + i)));
        table.setHorizontalHeaderItem(i, item);
        for (int j=0; j<ROWS; j++) {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString("(%1, %2)")
            .arg(QChar('A'+i)) .arg(j+1));
            table.setItem(j, i, item);
        }
    }
    //table.resizeRowsToContents();
    //table.resizeColumnsToContents();
    table.show();
    return app.exec();
}