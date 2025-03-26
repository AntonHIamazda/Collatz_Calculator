#include "mainwindow.h"
#include "darkmode.h"
#include <QWidget>
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString iconPath = isDarkMode() ? ":/icons/black_icon.ico": ":/icons/white_icon.ico";
    QIcon icon(iconPath);
    a.setWindowIcon(icon);

    MainWindow w;
    w.show();
    return a.exec();
}
