#ifndef THREADSAFEVECTOR_H
#define THREADSAFEVECTOR_H

#include <QMutex>
#include <QObject>
#include <QVector>

template <typename T>
class ThreadSafeVector {
public:
    explicit ThreadSafeVector();

    // blocking push data
    void pushBack(T data);

    /*  blocking pop data
        don't check size vec
    */
    T popFront();

    // blocking size
    uint32_t size() const;

    QMutex* getMutex();

    QVector<T>* getVector();

    void setMaxSize(uint32_t size);

signals:
    void maxSizeReached();
    void dataWritten();

private:
    QMutex*    mutex { nullptr };
    QVector<T> vec;
    uint32_t   maxSize { 1 };
};

#endif // THREADSAFEVECTOR_H
