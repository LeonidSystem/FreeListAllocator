#include "FreeListAllocator.h"
#include <cstdlib>
#include <iostream>

FreeListAllocator::FreeListAllocator(const std::size_t totalSize) : 
    m_totalSize(totalSize),
    m_usedBytes(0),
    m_startPtr(nullptr) 
{}

FreeListAllocator::~FreeListAllocator()
{
    free(m_startPtr);
    m_startPtr = nullptr;
    //std::cout << "~FreeListAllocator" << std::endl;
}

void FreeListAllocator::Init()
{
    if (m_startPtr)
    {
        free(m_startPtr);
        m_startPtr = nullptr;
    }

    m_startPtr = malloc(m_totalSize + AllocationHeaderSize);
    // set header
    *((std::size_t*)m_startPtr) = m_totalSize;
    void* data = m_startPtr + AllocationHeaderSize;

    m_freeList.push_front(FreeBlock(m_totalSize, data));
}

void* FreeListAllocator::Allocate(const std::size_t size)
{
    std::list<FreeListAllocator::FreeBlock>::iterator nodeIter = m_freeList.end();
    Find(size, nodeIter);

    if (nodeIter == m_freeList.end())
    {
        //std::cout << "not enough memory" << std::endl;
        return nullptr;
    }

    if ((*nodeIter).blockSize > size)
    {
        std::size_t restSize = (*nodeIter).blockSize - size;
        // change header for old block
        *((std::size_t*)((*nodeIter).data - AllocationHeaderSize)) = size; 

        // set header for new block
        void* data = (*nodeIter).data + size;
        *((std::size_t*)data) = restSize;
        data = data + AllocationHeaderSize;

        m_freeList.insert(nodeIter, FreeBlock{restSize, data});
    }

    m_usedBytes += size + AllocationHeaderSize;
    m_freeList.erase(nodeIter);

    return (*nodeIter).data;
}

void FreeListAllocator::Free(void* ptr)
{
    std::size_t blockSize = *((std::size_t*)(ptr - AllocationHeaderSize));

    for (auto it = m_freeList.begin(); it != m_freeList.end(); ++it)
    {
        if (ptr < (*it).data)
        {
            auto prev = m_freeList.insert(it, FreeBlock{blockSize, ptr});
            m_usedBytes -= (blockSize + AllocationHeaderSize);
            Merge(prev, it);
            return;
        }
    }

    // if m_freeList is empty
    m_freeList.push_front(FreeBlock{blockSize, ptr});
    m_usedBytes -= (blockSize + AllocationHeaderSize);
}

void FreeListAllocator::Find(const std::size_t size, std::list<FreeListAllocator::FreeBlock>::iterator& nodeIter)
{
    for (auto it = m_freeList.begin(); it != m_freeList.end(); ++it)
    {
        if ((*it).blockSize >= size)
        {
            nodeIter = it;
            //std::cout << "find: " << (*it).data << std::endl;
            return;
        }
    }
}

void FreeListAllocator::Merge(std::list<FreeBlock>::iterator& prev, std::list<FreeBlock>::iterator& next)
{
    //std::cout << "prev.data: " << (*prev).data << std::endl;
    //std::cout << "next.data: " << (*next).data << std::endl;

    if ((*prev).data + (*prev).blockSize + AllocationHeaderSize == (*next).data)
    {
        // set header
        (*(std::size_t*)((*prev).data - AllocationHeaderSize)) = (*prev).blockSize + (*next).blockSize;
        (*prev).blockSize += (*next).blockSize;
        m_freeList.erase(next);
    }
}

void FreeListAllocator::DisplayList()
{
    for (const auto& node : m_freeList)
    {
        std::cout << "data: " << node.data << std::endl;
        std::cout << "blockSize: " << node.blockSize << std::endl;
    }
}

std::size_t FreeListAllocator::GetTotalSize() const { return m_totalSize; }
std::size_t FreeListAllocator::GetUsedBytes() const { return m_usedBytes; }
const std::list<FreeListAllocator::FreeBlock>& FreeListAllocator::GetFreeList() { return m_freeList; }