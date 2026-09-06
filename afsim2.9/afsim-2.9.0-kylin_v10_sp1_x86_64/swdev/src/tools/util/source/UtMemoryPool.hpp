// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTMEMORYPOOL_HPP
#define UTMEMORYPOOL_HPP
#include <cstdlib>
#include <cstring>

//! A very simple memory pool.  Memory is allocated in chunks for improved efficiency.
//! The user must use placement-new to construct new objects.  Destructors will not be called.
//! When the memory pool is destroyed, the memory is freed.
class UtMemoryPool
{
   // singly linked list pointing to allocated memory.
   struct Block
   {
      Block() = default;
      ~Block() { free(mMemoryPtr); }
      Block* mNextPtr{nullptr};
      void*  mMemoryPtr{nullptr};
      size_t mAllocCount{0};
      size_t mUsedCount{0};
   };

   // No implementation:
   UtMemoryPool& operator=(const UtMemoryPool& aRhs) = delete;
   UtMemoryPool(const UtMemoryPool& aSrc)            = delete;

public:
   //! @param aAllocSize The size in bytes of the objects being allocated
   //! @param aNextSize  The pool is sized to hold this many objects initially.
   UtMemoryPool(size_t aAllocSize, size_t aNextSize = 32)
      // Allocation size is rounded up to the nearest pointer size.
      : mAllocSize(aAllocSize / sizeof(void*) * sizeof(void*) + aAllocSize % sizeof(void*))
      , mNextSize(aNextSize)
   {
      mFirstBlockPtr = mBlockPtr = nullptr;
   }

   virtual ~UtMemoryPool()
   {
      while (mFirstBlockPtr != nullptr)
      {
         Block* tmpPtr  = mFirstBlockPtr;
         mFirstBlockPtr = mFirstBlockPtr->mNextPtr;
         delete tmpPtr;
      }
   }
   // Begin to re-use memory.  Any existing objects should no longer be referenced.
   void Reset()
   {
      mBlockPtr = mFirstBlockPtr;
      if (mBlockPtr != nullptr)
      {
         mBlockPtr->mUsedCount = 0;
      }
   }

   void DebugClear()
   {
      Block* bPtr = mFirstBlockPtr;
      while (bPtr != nullptr)
      {
         memset(bPtr->mMemoryPtr, 0xfefefefe, bPtr->mAllocCount * mAllocSize);
         bPtr = bPtr->mNextPtr;
      }
   }

   // Allocate a block of memory.  Use placement new to construct objects in the memory returned:
   // void* memPtr = pool->Alloc()
   // MyClass* instancePtr = new (memPtr) MyClass(args...);
   //
   // Do not call delete[] or free() on the returned pointer.
   void* Alloc()
   {
      if ((mBlockPtr != nullptr) && mBlockPtr->mAllocCount > mBlockPtr->mUsedCount)
      {
         return (void*)(((char*)(mBlockPtr->mMemoryPtr)) + (mBlockPtr->mUsedCount++ * mAllocSize));
      }
      else
      {
         if ((mBlockPtr != nullptr) && (mBlockPtr->mNextPtr != nullptr))
         {
            mBlockPtr             = mBlockPtr->mNextPtr;
            mBlockPtr->mUsedCount = 0;
            return Alloc();
         }
         else
         {
            Block* blockPtr       = new Block;
            blockPtr->mAllocCount = mNextSize;
            mNextSize             = 32 + size_t(mNextSize * 1.5);
            blockPtr->mMemoryPtr  = malloc(blockPtr->mAllocCount * mAllocSize);
            blockPtr->mUsedCount  = 1;
            if (mBlockPtr != nullptr)
            {
               mBlockPtr->mNextPtr = blockPtr;
               mBlockPtr           = blockPtr;
            }
            else
            {
               mFirstBlockPtr = mBlockPtr = blockPtr;
            }
            return blockPtr->mMemoryPtr;
         }
      }
   }

protected:
   size_t mAllocSize;
   size_t mNextSize;
   Block* mBlockPtr;
   Block* mFirstBlockPtr;
};

#endif
