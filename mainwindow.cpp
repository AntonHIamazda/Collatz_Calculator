#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), logic(nullptr), maxLimit(200000000)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    if (logic)
    {
        logic->requestInterruption();
        logicThread.quit();
        logicThread.wait();
        delete logic;
    }
    delete ui;
}

void MainWindow::setupUI() {
    ui->range->setMinimum(1);
    ui->range->setMaximum(maxLimit);
    ui->range->setValue(1);

    int maxThreads = QThread::idealThreadCount();
    ui->threadNumbers->setMinimum(1);
    ui->threadNumbers->setMaximum(maxThreads);
    ui->threadNumbers->setValue(maxThreads / 2);


    threadCount = ui->threadNumbers->value();
    ui->threadDisp->setText(QString::number(threadCount));

    connect(ui->threadNumbers, QOverload<int>::of(&QSlider::valueChanged), this, &MainWindow::updateThreadCount);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(ui->ExitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    ui->stopButton->setEnabled(false);
    threadCount = ui->threadNumbers->value();
}

void MainWindow::updateThreadCount() {
    threadCount = ui->threadNumbers->value();
    ui->threadDisp->setText(QString::number(threadCount));
}

void MainWindow::onStartClicked() {
    int limit = ui->range->value();
    threadCount = ui->threadNumbers->value();

    if (logic) {
        logic->requestInterruption();
        logicThread.quit();
        logicThread.wait();
        delete logic;
    }

    logic = new CollatzLogic(limit, threadCount);
    logic->moveToThread(&logicThread);

    connect(&logicThread, &QThread::started, logic, &CollatzLogic::process);
    connect(logic, &CollatzLogic::resultReady, this, &MainWindow::handleResult);
    connect(logic, &CollatzLogic::finished, this, &MainWindow::onComputationFinished);

    logicThread.start();

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->outText->appendPlainText(QString("Computation started for limit %1...").arg(limit));
}

void MainWindow::onStopClicked() {
    if (logic) {
        logic->requestInterruption();
        ui->outText->appendPlainText("Computation stopped.");
    }
    ui->startButton->setEnabled(true);    ui->stopButton->setEnabled(false);
}

void MainWindow::onComputationFinished() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void MainWindow::handleResult(int number, int length, qint64 elapsedTime) {
    ui->outText->appendPlainText(QString("Longest Collatz sequence for number %1: %2 steps").arg(number).arg(length));
    ui->outText->appendPlainText(QString("Execution time: %1 ms").arg(elapsedTime));
}
