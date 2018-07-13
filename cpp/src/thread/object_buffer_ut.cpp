#include <gtest/gtest.h>
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

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
