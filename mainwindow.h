#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "collatzlogic.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();
    void onStopClicked();
    void handleResult(int number, int length, qint64 elapsedTime);


private:
    Ui::MainWindow *ui;
    QThread logicThread;
    CollatzLogic *logic;
    int maxLimit;
    int threadCount;

    QVector<int> sharedCache;

    void setupUI();
    void updateThreadCount();
    void onComputationFinished();
};

#endif // MAINWINDOW_H
