#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_
#include <functional>

class ThreadTask {
public:

    template<class callback, class F, class... Args>
    ThreadTask(F&& callback, F&& func, Args&&... args);
private:

}

#endif /* ThreadPool */
