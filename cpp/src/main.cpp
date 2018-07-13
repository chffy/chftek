#include "thread/thread_pool.h"
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <cstdio>
#include <functional>
#include <thread>

int main() 
{
    std::shared_ptr<ThreadPool> pool = std::make_shared<ThreadPool>(3);
    for (int i = 0; i < 10; ++i) {
        std::function<void()> a = [i] ()-> void {std::cout << i << std::endl;};
        pool->SubmitTask(std::move(a));
    }
    pool->Stop();
}
