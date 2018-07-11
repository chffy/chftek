#include "object_buffer.h"
#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <cstdio>

void push(std::shared_ptr<ObjectBuffer<int> > a) {
    a->push(1);
}

void pop(std::shared_ptr<ObjectBuffer<int> > a) {
    std::cout << a->pop() << std::endl;
}


int main() 
{
    auto a = std::make_shared<ObjectBuffer<int> >(100);
    std::thread t1(pop, a);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::thread t2(push, a);
    t1.join();
    t2.join();
}
