#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(mainwindow);

    QApplication a(argc, argv);

    /*Field::__print({10475, 1200, 300, 200, 200, 200, 200, 170, 150},\
    {1000, 900, 800, 700, 600, 500, 400, 300,
     90, 80, 40, 40, 40, 40, 40, 40,
     20, 10, 9, 8, 6, 6, 3, 3},\
     {128, 64, 32, 16, 8, 5, 3, 3});*/



    MainWindow w;
    w.show();

    return a.exec();
}
