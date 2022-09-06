#include "threadsafevector.h"

template <typename T>
ThreadSafeVector<T>::ThreadSafeVector()
    : mutex(new QMutex())
{
}

template <typename T>
void ThreadSafeVector<T>::pushBack(T data)
{
    mutex->lock();
    vec.push_back(data);
    mutex->unlock();
}

template <typename T>
T ThreadSafeVector<T>::popFront()
{
    mutex->lock();
    auto data = vec.at(0);
    vec.pop_front();
    mutex->unlock();
    return data;
}

template <typename T>
uint32_t ThreadSafeVector<T>::size() const
{
    mutex->lock();
    auto size = vec.size();
    mutex->unlock();
    return size;
}

template <typename T>
QMutex* ThreadSafeVector<T>::getMutex()
{
    return this->mutex;
}

template <typename T>
QVector<T>* ThreadSafeVector<T>::getVector()
{
    return &vec;
}

template <typename T>
void ThreadSafeVector<T>::setMaxSize(uint32_t size)
{
    mutex->lock();
    this->maxSize = size;
    mutex->unlock();
}

class AVPacket;
template class ThreadSafeVector<AVPacket*>;
