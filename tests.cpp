#include "FreeListAllocator.h"
#include <gtest/gtest.h>

namespace
{
    /**
     * @brief Allocate Tests
     * 
     */
    TEST(Allocate, AllocateOneChunk)
    {
        FreeListAllocator allocator(2048);
        allocator.Init();
        void* ptr512 = allocator.Allocate(512);
        ASSERT_EQ(allocator.GetUsedBytes(), 512 + FreeListAllocator::AllocationHeaderSize);
    }

    TEST(Allocate, AllocateFewChunks)
    {
        FreeListAllocator allocator(2048);
        allocator.Init();
        void* ptr512 = allocator.Allocate(512);
        void* ptr1024 = allocator.Allocate(1024);
        ASSERT_EQ(ptr512 + 512 + FreeListAllocator::AllocationHeaderSize, ptr1024);
    }

    /**
     * @brief Free Tests
     * 
     */

    TEST(Free, FreeOneChunk)
    {
        FreeListAllocator allocator(2048);
        allocator.Init();
        void* ptr2048 = allocator.Allocate(2048);
        allocator.Free(ptr2048);
        ASSERT_EQ(allocator.GetUsedBytes(), 0);
    }

    TEST(Free, MergeAfterFree)
    {
        FreeListAllocator allocator(2048);
        allocator.Init();
        
        void* ptr1024 = allocator.Allocate(1024);
        void* ptr512 = allocator.Allocate(512);

        allocator.Free(ptr512);
        auto freeList = allocator.GetFreeList();
        ASSERT_EQ(freeList.size(), 1);
        ASSERT_EQ(freeList.begin()->blockSize, 1024);
    }

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}