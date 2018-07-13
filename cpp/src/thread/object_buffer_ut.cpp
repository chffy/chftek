#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>
#include "object_buffer.h"

TEST(ObjectBufferTest, NormalFunc) {
    std::shared_ptr<ObjectBuffer<int> > buffer = std::make_shared<ObjectBuffer<int> >(10);
    for (int index = 0; index < 9; ++index) {
        buffer->Push(index);
    }
    ASSERT_EQ(buffer->Size(), 9);
    for (int index = 0; index < 9; ++index) {
        int data = buffer->Pop();
        ASSERT_EQ(data, index);
    }
    ASSERT_EQ(buffer->Size(), 0);
    ASSERT_EQ(buffer->IsEmpty(), true);
    ASSERT_EQ(buffer->IsFull(), false);
    for (int index = 0; index < 10; ++index) {
        buffer->Push(index);
    }
    ASSERT_EQ(buffer->IsEmpty(), false);
    ASSERT_EQ(buffer->IsFull(), true);
}

void PushFunc(std::shared_ptr<ObjectBuffer<int> > buffer,
              const std::vector<int> &dataVec,
              double &costTime) {
    auto start = std::chrono::system_clock::now();
    for (const auto &data : dataVec) {
        buffer->Push(data);
    }
    auto end = std::chrono::system_clock::now();
    auto internalTime = end - start;
    costTime = internalTime.count() / 1000000.0;
}

void PopFunc(std::shared_ptr<ObjectBuffer<int> > buffer,
             int num,
             std::vector<int> &resultDataVec,
             double &costTime) {
    auto start = std::chrono::system_clock::now();
    resultDataVec.clear();
    for (int index = 0; index < num; ++index) {
        resultDataVec.push_back(buffer->Pop());
    }
    auto end = std::chrono::system_clock::now();
    auto internalTime = end - start;
    costTime = internalTime.count() / 1000000.0;
}

TEST(ObjectBufferTest, BlockPushAndPop) {
    std::shared_ptr<ObjectBuffer<int> > buffer = std::make_shared<ObjectBuffer<int> >(10);
    std::vector<int> inputDataVec;
    // empty block test
    inputDataVec.push_back(1);
    inputDataVec.push_back(2);
    std::vector<int> resultDataVec;
    double pushTime = 0.0;
    double popTime = 0.0;
    std::thread popThread1(PopFunc, buffer, 2, std::ref(resultDataVec), std::ref(popTime));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::thread pushThread1(PushFunc, buffer, std::cref(inputDataVec), std::ref(pushTime));
    popThread1.join();
    pushThread1.join();
    ASSERT_GT(popTime, 1);
    ASSERT_LT(popTime, 3);
    ASSERT_LT(pushTime, 2);
    // full block test
    inputDataVec.clear();
    resultDataVec.clear();
    for (int index = 0; index < 12; ++index) {
        inputDataVec.push_back(index);
    }
    pushTime = 0.0;
    popTime = 0.0;
    std::thread pushThread2(PushFunc, buffer, std::cref(inputDataVec), std::ref(pushTime));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::thread popThread2(PopFunc, buffer, 12, std::ref(resultDataVec), std::ref(popTime));
    popThread2.join();
    pushThread2.join();
    ASSERT_GT(pushTime, 1);
    ASSERT_LT(pushTime, 3);
    ASSERT_LT(popTime, 2);
 
    ASSERT_EQ(inputDataVec.size(), resultDataVec.size());
    for (int index = 0; index < inputDataVec.size(); ++index) {
        ASSERT_EQ(inputDataVec[index], resultDataVec[index]);
    }
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
