// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2014 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTCIRCULARBUFFERINDEX_HPP
#define UTCIRCULARBUFFERINDEX_HPP

#include "ut_export.h"

#include <cassert>


// Manages indexing of a circular buffer, but does not store any data
// to be used in conjunction with a container class.
// Indexing is done as follows:
//  buffer slots:    _  _  _  _  _  _  _  _  _  _  _  _
//  valid items:     .  .  .  .        .  .  .  .  .  .
//                                     ^
//                                  Start=6,
//    Size=10, BufferSize=12
//    End1()=12, End2()=4, FrontIndex()=6, BackIndex()=3
//
class UtCircularBufferIndex
{
public:
   UtCircularBufferIndex()
      : mStart(0)
      , mSize(0)
      , mBufferSize(0)
   {
   }

   // Does the valid data segment wrap around the end of the buffer
   bool Wraps() const { return mStart + mSize > mBufferSize; }
   // this is the end index of the first segment
   int End1() const
   {
      int end = mStart + mSize;
      if (end > mBufferSize)
      {
         return mBufferSize;
      }
      return end;
   }

   // if Wraps(), this is the end of the second segment
   // otherwise, there is no second segment
   int End2() const { return mStart + mSize - mBufferSize; }

   // Index of the front element
   int FrontIndex() const { return mStart; }
   // Index of the back element
   int BackIndex() const
   {
      if (mStart + mSize > mBufferSize)
      {
         return mStart + mSize - mBufferSize - 1;
      }
      return mStart + mSize - 1;
   }

   // Add an element to the front
   int PushFront()
   {
      ++mSize;
      assert(mSize <= mBufferSize);
      --mStart;
      if (mStart < 0)
      {
         mStart = mBufferSize - 1;
      }
      return FrontIndex();
   }

   // Add an element to the back
   int PushBack()
   {
      ++mSize;
      assert(mSize <= mBufferSize);
      return BackIndex();
   }

   // Remove the front element
   void PopFront()
   {
      assert(mSize > 0);
      ++mStart;
      if (mStart >= mBufferSize)
      {
         mStart = 0;
      }
      --mSize;
   }

   void PopFrontN(int aCount)
   {
      if (aCount <= 0)
         return;
      assert(mSize >= aCount);
      mStart = (mStart + aCount) % mBufferSize;
      mSize -= aCount;
   }

   // Remove the back element
   void PopBack()
   {
      assert(mSize > 0);
      --mSize;
   }

   void PopBackN(int aCount)
   {
      if (aCount <= 0)
         return;
      assert(mSize >= aCount);
      mSize -= aCount;
   }

   // Returns the number of elements in the circular buffer
   int GetSize() const { return mSize; }

   int IndexToOffset(int aIndex) const
   {
      int offset = mStart + aIndex;
      if (offset >= mBufferSize)
      {
         return offset - mBufferSize;
      }
      return offset;
   }
   // Given an offset, convert to the index.  Returns -1 if no valid index corresponds to the offset
   int OffsetToIndex(int aOffset) const
   {
      int idx = -1;
      if (aOffset > mStart)
      {
         idx = aOffset - mStart;
      }
      else
      {
         idx = mBufferSize - mStart + aOffset;
      }
      if (idx > mSize)
      {
         idx = -1;
      }
      return idx;
   }

   // index of the first element
   int mStart;
   // Number of active elements
   int mSize;
   // Number of slots for elements
   int mBufferSize;

   // iterator-style interface to iterating a circular buffer
   struct iterator
   {
      iterator(int aIndex, const UtCircularBufferIndex& aIndexing)
         : mIndex(aIndex)
      {
         mBegin        = aIndexing.mStart;
         mSize         = aIndexing.mSize;
         mSegment1Size = aIndexing.End1() - mBegin;
      }
      iterator& operator++()
      {
         ++mIndex;
         return *this;
      }
      iterator& operator--()
      {
         --mIndex;
         return *this;
      }
      bool IsEnd() const { return mIndex >= mSize; }
      bool IsBegin() const { return mIndex == 0; }

      int GetOffset() const { return mIndex < mSegment1Size ? mBegin + mIndex : mIndex - mSegment1Size; }
      int GetIndex() const { return mIndex; }
      int mIndex;
      int mBegin;
      int mSegment1Size;
      int mSize;
   };
};
#endif
