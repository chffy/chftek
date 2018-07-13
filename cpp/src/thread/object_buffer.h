#ifndef OBJECT_BUFFER_H_
#define OBJECT_BUFFER_H_
#include <mutex>
#include <deque>
#include <condition_variable>

template<typename T>
class ObjectBuffer {
private:
    std::mutex mMutex;
    std::mutex tmp;
    std::condition_variable mPushCond;
    std::condition_variable mPopCond;
    std::deque<T> mData;
    int mMaxSize;

    bool IsEmptyWithoutLock();

    bool IsFullWithoutLock();

public:
    ObjectBuffer(int maxSize);

    bool IsEmpty();

    bool IsFull();
    
    int Size();

    void Push(T element);

    T Pop();
};

template<typename T>
bool ObjectBuffer<T>::IsEmptyWithoutLock() {
    return mData.empty();
}

template<typename T>
bool ObjectBuffer<T>::IsFullWithoutLock() {
    return mData.size() >= mMaxSize;
}

template<typename T>
ObjectBuffer<T>::ObjectBuffer(int maxSize): mMaxSize(maxSize) {
}

template<typename T>
bool ObjectBuffer<T>::IsEmpty() {
    std::unique_lock<std::mutex> _(mMutex);
    return IsEmptyWithoutLock();
}

template<typename T>
bool ObjectBuffer<T>::IsFull() {
    std::unique_lock<std::mutex> _(mMutex);
    return IsFullWithoutLock();
}

template<typename T>
int ObjectBuffer<T>::Size() {
    std::unique_lock<std::mutex> _(mMutex);
    return mData.size();
}

template<typename T>
void ObjectBuffer<T>::Push(T element) {
    std::unique_lock<std::mutex> _(mMutex);
    mPopCond.wait(_, [this] {return IsFullWithoutLock() == false; });
    mData.emplace_back(std::move(element));
    mPushCond.notify_one();
}

template<typename T>
T ObjectBuffer<T>::Pop() {
    std::unique_lock<std::mutex> _(mMutex);
    mPushCond.wait(_, [this] {return IsEmptyWithoutLock() == false; });
    T element = std::move(mData.front());
    mData.pop_front();
    mPopCond.notify_one();
    return std::move(element);
}

#endif /* ObjectBuffer */
