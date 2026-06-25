#include "collatzlogic.h"
#include <algorithm>

CollatzLogic::CollatzLogic(int limit, int threadCount, QVector<int> &sharedCache, QObject *parent)
    : QObject(parent),
      limit(limit),
      threadCount(threadCount),
      interrupted(false),
      cache(sharedCache){}

void CollatzLogic::requestInterruption() {
    interrupted = true;
}

void CollatzLogic::process() {
    QElapsedTimer timer;
    timer.start();
    int globalMaxLength = 1;
    int globalMaxNumber = 1;

    int currentCacheSize = 0;

    if(limit > 100000){
        currentCacheSize = 300000;
        currentCacheSize = std::min(limit, currentCacheSize);
    }

    if(currentCacheSize > 0 && cache.size() <= currentCacheSize){
        cache.resize(currentCacheSize + 1);
        cache.fill(0);

        cache[1] = 1;
        for(int i = 2; i <= currentCacheSize; ++i){
            if (interrupted) break;

            if (cache[i] == 0) {
                cache[i] = collatzLength(static_cast<uint64_t>(i));
            }
        }

    }

    if(limit > currentCacheSize && !interrupted){
        QVector<QFuture<QPair<int, int>>> futures;
        auto currentTask = std::make_shared<std::atomic<int>>(currentCacheSize + 1);

        for(int i = 0; i < threadCount; ++i){
            futures.append(QtConcurrent::run([this, currentTask, limit = this->limit]() -> QPair<int, int> {
                int maxNumber = 1;
                int maxLength = 1;
                int j;

                while((j = currentTask->fetch_add(1, std::memory_order_relaxed)) <= limit){
                    if (interrupted) return qMakePair(-1, -1);

                    int length = collatzLength(static_cast<uint64_t>(j));

                    if(length > maxLength){
                        maxNumber = j;
                        maxLength = length;
                    }

                }
                return qMakePair(maxNumber, maxLength);
            }));
        }

        for(auto &future: futures){
            future.waitForFinished();
            QPair<int, int> result = future.result();

            if(result.second > globalMaxLength){
                globalMaxLength = result.second;
                globalMaxNumber = result.first;
            }
        }
    }


    if (!interrupted) {
        emit resultReady(globalMaxNumber, globalMaxLength, timer.elapsed());
    }

    emit finished();
}





int CollatzLogic::collatzLength(uint64_t n) {

    int length = 0;
    int cacheSize = cache.size() - 1;

    while(n != 1){
        if(cacheSize > 0 && n <= cacheSize && cache[n] != 0){
            return length + cache[n];
        }

        if(n & 1){
            n = n * 3 + 1;
            n >>= 1;
            length += 2;
        }
        else{
            n >>=1;
            length ++;
        }
    }
    return length + 1;






}
