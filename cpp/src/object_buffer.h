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
    std::condition_variable mCond;
    std::deque<T> mData;
    int mSize;

    bool isEmptyWithoutLock();

    bool isFullWithoutLock();

public:
    ObjectBuffer(int size);

    bool isEmpty();

    bool isFull();

    void push(T element);

    T pop();
};

template<typename T>
bool ObjectBuffer<T>::isEmptyWithoutLock() {
    return mData.empty();
}

template<typename T>
bool ObjectBuffer<T>::isFullWithoutLock() {
    return mData.size() >= mSize;    
}

template<typename T>
ObjectBuffer<T>::ObjectBuffer(int size) {
    mSize = size;
}

template<typename T>
bool ObjectBuffer<T>::isEmpty() {
    std::unique_lock<std::mutex> _(mMutex);
    return isEmptyWithoutLock();
}

template<typename T>
bool ObjectBuffer<T>::isFull() {
    std::unique_lock<std::mutex> _(mMutex);
    return isFullWithoutLock();
}

template<typename T>
void ObjectBuffer<T>::push(T element) {
    std::unique_lock<std::mutex> _(mMutex);
    mCond.wait(_, [this] {return isFullWithoutLock() == false; });
    mData.push_back(element);
    mCond.notify_all();
}

template<typename T>
T ObjectBuffer<T>::pop() {
    std::unique_lock<std::mutex> _(mMutex);
    mCond.wait(_, [this] {return isEmptyWithoutLock() == false; });
    T element = mData.front();
    mData.pop_front();
    mCond.notify_all();
    return element;
}

#endif /* ObjectBuffer */
