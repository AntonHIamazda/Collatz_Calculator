#ifndef COLLATZLOGIC_H
#define COLLATZLOGIC_H

#include <QObject>
#include <QVector>
#include <QFuture>
#include <QtConcurrent>
#include <QElapsedTimer>

class CollatzLogic : public QObject {
    Q_OBJECT

public:
    explicit CollatzLogic(int limit, int threadCount, QObject *parent = nullptr);
    void requestInterruption();

signals:
    void resultReady(int number, int length, qint64 elapsedTime);
    void finished();
    void calculationError(int number, QString message);

public slots:
    void process();

private:
    int limit;
    int threadCount;
    bool interrupted;

    QPair<int, bool> collatzLength(long long n);
};

#endif // COLLATZLOGIC_H
