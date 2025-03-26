#include "collatzlogic.h"
#include <limits>

CollatzLogic::CollatzLogic(int limit, int threadCount, QObject *parent)
    : QObject(parent), limit(limit), threadCount(threadCount), interrupted(false) {}

void CollatzLogic::requestInterruption() {
    interrupted = true;
}

void CollatzLogic::process() {
    QElapsedTimer timer;
    timer.start();

    int maxNumber = 1;
    int maxLength = 1;

    QVector<QFuture<QPair<int, int>>> futures;
    int chunkSize = limit / threadCount;

    for (int i = 0; i < threadCount; ++i) {
        int start = i * chunkSize + 1;
        int end = (i == threadCount - 1) ? limit : (start + chunkSize - 1);

        futures.append(QtConcurrent::run([this, start, end]() -> QPair<int, int> {
            int localMaxNumber = 1;
            int localMaxLength = 1;

            for (int j = start; j <= end; ++j) {
                if (interrupted) return qMakePair(-1, -1);

                int length;
                bool overflow;
                std::tie(length, overflow) = collatzLength(j);

                if (overflow) {
                    emit calculationError(j, "Overflow detected in Collatz sequence.");
                    return qMakePair(-1, -1);
                }

                if (length > localMaxLength) {
                    localMaxNumber = j;
                    localMaxLength = length;
                }
            }

            return qMakePair(localMaxNumber, localMaxLength);
        }));
    }

    for (auto &future : futures) {
        future.waitForFinished();
        QPair<int, int> result = future.result();
        if (result.second > maxLength) {
            maxNumber = result.first;
            maxLength = result.second;
        }
    }

    if (!interrupted) {
        emit resultReady(maxNumber, maxLength, timer.elapsed());
    }

    emit finished();
}

QPair<int, bool> CollatzLogic::collatzLength(long long n) {
    int length = 1;
    while (n != 1) {
        if (n < 0 || n > std::numeric_limits<long long>::max() / 3) {
            return qMakePair(-1, true); // Overflow detected
        }
        n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
        ++length;
    }
    return qMakePair(length, false);
}
