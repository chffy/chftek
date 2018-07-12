#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include "object_buffer.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>

class ThreadPool {
public:
    enum ThreadPoolMode {
        PRE_ALLOC_THREAD = 0,
        RUNTIME_ALLOC_THREAD
    };

    ThreadPool(size_t size, ThreadPoolMode mode=ThreadPoolMode::PRE_ALLOC_THREAD);

    void Stop();

    template<typename T>
    void SubmitTask(std::function<T()> &&func);

    //template<class Func>
    //auto enqueue(Func&& func) -> std::future<typename std::result_of<Func()>::type>; 

    ~ThreadPool() {
        for (auto &worker: mPermanentWorkers) {
            worker.join();
        }
    };
private:

    void AllocPermanentWorkers(size_t size);
    
    void addTaskToBuffer(std::packaged_task<void()> &&task);

    std::shared_ptr<ObjectBuffer<std::packaged_task<void()> > > mTaskBufferPtr;
    std::vector<std::thread> mPermanentWorkers;
    std::mutex mMutex; 
    std::condition_variable mBufferCond;
    std::condition_variable mTaskCond;
    bool mStop;
    ThreadPoolMode mMode;
    size_t mSize;
};

ThreadPool::ThreadPool(size_t size, ThreadPoolMode mode) {
    mSize = size;
    mStop = false;
    mTaskBufferPtr = std::make_shared<ObjectBuffer<std::packaged_task<void()> > >(mSize);
    mMode = mode;
    switch (mMode) {
        case ThreadPoolMode::PRE_ALLOC_THREAD: {
            AllocPermanentWorkers(mSize);
            break;
        }
        default: {
            throw std::runtime_error("can't reach here");
        }
    }
}

void ThreadPool::AllocPermanentWorkers(size_t size) {
    mPermanentWorkers.clear();
    for (size_t index = 0; index < size; ++index) {
        auto worker = [this] {
            while (mStop == false) {
                std::packaged_task<void()> task;
                {
                    std::unique_lock<std::mutex> _(mMutex);
                    mTaskCond.wait(_, [this] {
                        return mStop == true || mTaskBufferPtr->IsEmpty() == false;
                        });
                    if (mStop == true && mTaskBufferPtr->IsEmpty() == true) {
                        return ;
                    }
                    task = mTaskBufferPtr->Pop();
                    mBufferCond.notify_one();
                }
                task();
            }
        };
        mPermanentWorkers.emplace_back(worker);
    }
}

void ThreadPool::Stop() {
    std::unique_lock<std::mutex> _(mMutex);
    mStop = true;
    mTaskCond.notify_all();
}

template<typename T>
void ThreadPool::SubmitTask(std::function<T()> &&func) {
    auto rowTask = std::make_shared<std::packaged_task<T()> >(func);
    auto task = std::move(std::packaged_task<void()>([rowTask] { (*rowTask)();}));
    addTaskToBuffer(std::move(task));
}

void ThreadPool::addTaskToBuffer(std::packaged_task<void()> &&task) {
    std::unique_lock<std::mutex> _(mMutex);
    if (mStop == true) {
        throw std::runtime_error("thread pool is dead...");
    }
    mBufferCond.wait(_, [this] {return mTaskBufferPtr->IsFull() == false;});
    mTaskBufferPtr->Push(std::move(task));
    mTaskCond.notify_one();
    
}
#endif /* ThreadPool */
