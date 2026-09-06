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

#ifndef UTGAPBUFFER_HPP
#define UTGAPBUFFER_HPP

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

#include <memory.h>

//! Implements a gap buffer.
//! Gap buffers provide random access and efficient insertion/deletion in most scenarios.
//! See http://en.wikipedia.org/wiki/Gap_buffer
template<typename T>
class UtGapBuffer;

// Random access iterator for UtGapBuffer
template<typename T>
class UtGapBufferIterator
{
public:
   using iterator_category = std::random_access_iterator_tag;
   using difference_type   = std::ptrdiff_t;
   using value_type        = T;
   using pointer           = T*;
   using reference         = T&;

   UtGapBufferIterator()
      : mDataPtr(nullptr)
      , mOffset()
      , mGapPos(0)
      , mGapSize()
   {
   }
   UtGapBufferIterator(UtGapBuffer<T>& aBufferPtr, size_t aPos);

   reference           operator*() const { return mDataPtr[mOffset]; }
   value_type*         operator->() const { return mDataPtr + mOffset; }
   value_type&         operator[](difference_type n) { return mDataPtr[MoveOffset(mOffset, n)]; }
   const value_type&   operator[](difference_type n) const { return mDataPtr[MoveOffset(mOffset, n)]; }
   UtGapBufferIterator operator+(difference_type n) const
   {
      UtGapBufferIterator rval(*this);
      rval.mOffset = MoveOffset(mOffset, n);
      return rval;
   }
   UtGapBufferIterator& operator+=(difference_type n)
   {
      mOffset = MoveOffset(mOffset, n);
      return *this;
   }
   UtGapBufferIterator operator-(difference_type n) const
   {
      UtGapBufferIterator rval(*this);
      rval.mOffset = MoveOffset(mOffset, -n);
      return rval;
   }
   UtGapBufferIterator& operator-=(difference_type n)
   {
      mOffset = MoveOffset(mOffset, -n);
      return *this;
   }
   bool            operator==(const UtGapBufferIterator<T>& aRhs) const { return mOffset == aRhs.mOffset; }
   bool            operator!=(const UtGapBufferIterator<T>& aRhs) const { return mOffset != aRhs.mOffset; }
   bool            operator<(const UtGapBufferIterator<T>& aRhs) const { return mOffset < aRhs.mOffset; }
   bool            operator<=(const UtGapBufferIterator<T>& aRhs) const { return mOffset < aRhs.mOffset; }
   bool            operator>(const UtGapBufferIterator<T>& aRhs) const { return mOffset > aRhs.mOffset; }
   bool            operator>=(const UtGapBufferIterator<T>& aRhs) const { return mOffset >= aRhs.mOffset; }
   difference_type operator-(const UtGapBufferIterator<T>& aRhs) const
   {
      size_t diff = mOffset - aRhs.mOffset;

      if ((aRhs.mOffset > mGapPos) != (mOffset > mGapPos))
      {
         if (mOffset > aRhs.mOffset)
         {
            diff -= mGapSize;
         }
         else
         {
            diff += mGapSize;
         }
      }
      return diff;
   }
   UtGapBufferIterator& operator++()
   {
      if (++mOffset == mGapPos)
      {
         mOffset += mGapSize;
      }
      return *this;
   }

   UtGapBufferIterator operator++(int)
   {
      UtGapBufferIterator rval(*this);
      ++(*this);
      return rval;
   }
   UtGapBufferIterator& operator--()
   {
      if (mOffset != mGapPos + mGapSize)
      {
         --mOffset;
      }
      else
      {
         mOffset = mGapPos - 1;
      }
      return *this;
   }
   UtGapBufferIterator operator--(int)
   {
      UtGapBufferIterator<T> rval(*this);
      --(*this);
      return rval;
   }

   size_t GetIndex() const { return mOffset < mGapPos ? mOffset : mOffset - mGapSize; }

protected:
   size_t MoveOffset(size_t aOffset, difference_type aDistance) const
   {
      if (aOffset >= mGapPos)
      {
         aOffset += aDistance;
         if (aOffset < mGapPos + mGapSize)
         {
            aOffset -= mGapSize;
         }
      }
      else
      {
         aOffset += aDistance;
         if (aOffset >= mGapPos)
         {
            aOffset += mGapSize;
         }
      }
      return aOffset;
   }

   T*     mDataPtr;
   size_t mOffset;
   size_t mGapPos;
   size_t mGapSize;
};

template<typename T>
class UtGapBuffer
{
public:
   friend class UtGapBufferIterator<T>;
   using iterator = UtGapBufferIterator<T>;

   UtGapBuffer()
      : mDataPtr(nullptr)
      , mSize(0)
      , mGapPosition(0)
      , mAllocationSize(0)
      , mGapSize(0)
      , mGrowSize(8)
   {
   }

   // Copy construct
   UtGapBuffer(const UtGapBuffer<T>& aRhs)
      : mDataPtr(new T[aRhs.mAllocationSize])
      , mSize(aRhs.mSize)
      , mGapPosition(aRhs.mGapPosition)
      , mAllocationSize(aRhs.mAllocationSize)
      , mGapSize(aRhs.mGapSize)
      , mGrowSize(aRhs.mGrowSize)
   {
      for (size_t i = 0; i < Size(); ++i)
      {
         (*this)[i] = aRhs[i];
      }
   }

   UtGapBuffer<T>& operator=(const UtGapBuffer<T>& aRhs)
   {
      mDataPtr        = new T[aRhs.mAllocationSize];
      mSize           = aRhs.mSize;
      mGapPosition    = aRhs.mGapPosition;
      mAllocationSize = aRhs.mAllocationSize;
      mGapSize        = aRhs.mGapSize;
      mGrowSize       = aRhs.mGrowSize;
      for (size_t i = 0; i < Size(); ++i)
      {
         (*this)[i] = aRhs[i];
      }
      return *this;
   }

   ~UtGapBuffer() { delete[] mDataPtr; }

   T& operator[](size_t aPos)
   {
      if (aPos < mGapPosition)
      {
         return mDataPtr[aPos];
      }
      return mDataPtr[mGapSize + aPos];
   }

   const T& operator[](size_t aPos) const
   {
      if (aPos < mGapPosition)
      {
         return mDataPtr[aPos];
      }
      return mDataPtr[mGapSize + aPos];
   }
   //! Inserts a single value prior to aPosition
   void Insert(size_t aPosition, T aVal)
   {
      assert(aPosition != static_cast<size_t>(-1));
      Reserve(mSize + 1);
      MoveGap(aPosition);
      mDataPtr[mGapPosition++] = aVal;
      ++mSize;
      --mGapSize;
   }

   //! Inserts a sequence of values prior to aPosition
   void Insert(size_t aPosition, const T* aValPtr, size_t aCount)
   {
      if (aCount > 0)
      {
         assert(aPosition != static_cast<size_t>(-1));
         Reserve(mSize + aCount);
         MoveGap(aPosition);
         std::copy(aValPtr, aValPtr + aCount, mDataPtr + aPosition);
         mSize += aCount;
         mGapPosition += aCount;
         mGapSize -= aCount;
      }
   }

   //! Erases a sequence of values starting at aPositionStart
   void Erase(size_t aPositionStart, size_t aCount = 1)
   {
      assert(aPositionStart != static_cast<size_t>(-1));
      if (aCount >= mSize)
      {
         mSize        = 0;
         mGapSize     = mAllocationSize;
         mGapPosition = 0;
      }
      else
      {
         MoveGap(aPositionStart);
         mGapSize += aCount;
         mSize -= aCount;
      }
   }

   //! Returns true if the size of the buffer is 0
   bool Empty() const { return mSize == 0; }

   // Return a pointer to a position in the buffer, ensuring at least aLength is valid
   T* GetPointer(size_t aPosition = 0, size_t aLengthValid = 0x7ffffff)
   {
      if (!mDataPtr)
      {
         Reserve(10); // need memory to return a pointer
      }
      size_t length = std::min(aLengthValid, mSize);
      T*     ptr    = nullptr;
      if (aPosition + length <= mGapPosition)
      {
         // data valid before gap
         ptr = mDataPtr + aPosition;
      }
      else if (aPosition >= mGapPosition)
      {
         // data valid after gap
         ptr = mDataPtr + aPosition + mGapSize;
      }
      else // crosses the gap
      {
         if ((aPosition == 0) && (aLengthValid == 0x7ffffff))
         {
            // move gap to end
            MoveGap(aPosition + length);
            ptr = mDataPtr + aPosition;
         }
         else
         {
            // move gap to position
            MoveGap(aPosition);
            ptr = mDataPtr + aPosition + mGapSize;
         }
      }
      return ptr;
   }

   //! Resizes the buffer by truncation or appending undefined values
   void Resize(size_t aSize)
   {
      Reserve(aSize);
      if (aSize > mSize)
      {
         MoveGap(mSize);
      }
      else
      {
         MoveGap(aSize);
      }
      mGapSize = mAllocationSize - aSize;
      mSize    = aSize;
   }
   //! Returns the number of values contained in the buffer
   size_t Size() const { return mSize; }

   //! Returns the position of the gap.
   int GetGapPosition() const { return mGapPosition; }

   //! Returns an iterator to the beginning of the sequence
   UtGapBufferIterator<T> begin() { return UtGapBufferIterator<T>(*this, 0); }

   //! Returns an iterator to the end of the sequence
   UtGapBufferIterator<T> end() { return UtGapBufferIterator<T>(*this, mSize); }

   //! Clears the document.  Memory is not freed.
   void Clear()
   {
      mSize        = 0;
      mGapPosition = 0;
      mGapSize     = mAllocationSize;
   }

   //! Clears the document and frees all memory
   void FreeMemory()
   {
      Clear();
      delete[] mDataPtr;
      mDataPtr        = nullptr;
      mAllocationSize = 0;
      mGrowSize       = 8;
   }
   //! Grows the buffer if necessary to accommodate at least aSize values
   void Reserve(size_t aSize)
   {
      if (mAllocationSize < aSize)
      {
         size_t allocSize = mAllocationSize;
         while (allocSize < aSize)
         {
            allocSize += mGrowSize;
            mGrowSize *= 2;
         }
         Reallocate(allocSize);
      }
   }
   //! Provides access to the underlying memory
   void DirectAccess(T*& aFirstBlock, size_t& aFirstBlockSize, T*& aSecondBlock, size_t& aSecondBlockSize)
   {
      aFirstBlock      = mDataPtr;
      aFirstBlockSize  = mGapPosition;
      aSecondBlock     = mDataPtr + mGapPosition + mGapSize;
      aSecondBlockSize = mSize - mGapPosition;
   }

   void Swap(UtGapBuffer<T>& aRhs)
   {
      std::swap(mDataPtr, aRhs.mDataPtr);
      std::swap(mSize, aRhs.mSize);
      std::swap(mGapPosition, aRhs.mGapPosition);
      std::swap(mAllocationSize, aRhs.mAllocationSize);
      std::swap(mGapSize, aRhs.mGapSize);
      std::swap(mGrowSize, aRhs.mGrowSize);
   }

   size_t GetAllocationSize() const { return mAllocationSize; }

protected:
   void MoveGap(size_t aPosition)
   {
      if (aPosition != mGapPosition)
      {
         T*     source      = nullptr;
         T*     destination = nullptr;
         size_t size        = 0;

         if (aPosition < mGapPosition)
         {
            source      = (mDataPtr + aPosition);
            destination = (mDataPtr + aPosition + mGapSize);
            size        = (mGapPosition - aPosition);
         }
         else
         {
            source      = (mDataPtr + mGapPosition + mGapSize);
            destination = (mDataPtr + mGapPosition);
            size        = (aPosition - mGapPosition);
         }

         if (source && destination)
         {
            if (source >= destination)
            {
               std::copy(source, source + size, destination);
            }
            else
            {
               std::copy_backward(source, source + size, destination + size);
            }

            mGapPosition = aPosition;
         }
      }
   }

   void Reallocate(size_t aSize)
   {
      if (aSize > mAllocationSize)
      {
         // Move the gap to the end
         MoveGap(mSize);
         // Keep 1 more element to allow null-terminator
         T* newBody = new T[aSize + 1];
         if ((aSize != 0) && (mDataPtr != nullptr))
         {
            memmove(newBody, mDataPtr, sizeof(T) * mSize);
            delete[] mDataPtr;
         }
         mDataPtr        = newBody;
         mAllocationSize = aSize;
         mGapSize        = mAllocationSize - mSize;
         assert(mGapSize == mAllocationSize - mSize);
      }
   }

   T*     mDataPtr;
   size_t mSize;
   size_t mGapPosition;
   size_t mAllocationSize;
   size_t mGapSize;
   size_t mGrowSize;
};

namespace std
{
template<typename T>
inline void swap(UtGapBuffer<T>& aLhs, UtGapBuffer<T>& aRhs)
{
   aLhs.Swap(aRhs);
}
} // namespace std

template<typename T>
inline UtGapBufferIterator<T>::UtGapBufferIterator(UtGapBuffer<T>& aBuffer, size_t aPosition)
   : mDataPtr(aBuffer.mDataPtr)
   , mGapPos(aBuffer.mGapPosition)
   , mGapSize(aBuffer.mGapSize)
{
   if (aPosition < mGapPos)
   {
      mOffset = aPosition;
   }
   else
   {
      mOffset = aPosition + mGapSize;
   }
}

#endif
