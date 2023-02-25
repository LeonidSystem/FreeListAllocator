#ifndef _FREE_LIST_ALLOCATOR_H_
#define _FREE_LIST_ALLOCATOR_H_

#include <list>

class FreeListAllocator
{
    public:
        static const unsigned long AllocationHeaderSize = sizeof(std::size_t);

        struct FreeBlock 
        {
            std::size_t blockSize;
            void* data;

            FreeBlock(std::size_t _blockSize, void* _data) : 
                blockSize(_blockSize),
                data(_data)
            {}

            bool operator==(const FreeBlock& header)
            {
                return blockSize == header.blockSize && data == header.data;
            }
        };

    private:
        std::size_t m_totalSize;
        std::size_t m_usedBytes;
        void* m_startPtr;
        std::list<FreeBlock> m_freeList;

    public:
        FreeListAllocator(const std::size_t totalSize);
        ~FreeListAllocator();
        void* Allocate(const std::size_t size);
        void Free(void* ptr);
        void Init();
        void DisplayList();

        std::size_t GetTotalSize() const;
        std::size_t GetUsedBytes() const;
        const std::list<FreeBlock>& GetFreeList();

    private:
        void Find(const std::size_t size, std::list<FreeListAllocator::FreeBlock>::iterator& nodeIter);
        void Merge(std::list<FreeBlock>::iterator& prev, std::list<FreeBlock>::iterator& next);
};

#endif