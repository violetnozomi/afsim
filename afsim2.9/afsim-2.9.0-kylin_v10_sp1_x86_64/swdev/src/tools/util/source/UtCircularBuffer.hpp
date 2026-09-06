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

#ifndef UTCIRCULARBUFFER_HPP
#define UTCIRCULARBUFFER_HPP

#include <cassert>
#include <cstddef>
#include <vector>

// A circular buffer where only the previous N pushed entries are stored
template<typename T>
class UtFixedCircularBuffer
{
public:
   UtFixedCircularBuffer(size_t aSize)
      : mSize(aSize)
   {
      mData.resize(aSize);
      mPos = 1;
   }
   ~UtFixedCircularBuffer() {}
   // Accesses the n'th element, where 0 is the last element added.
   // User should ensure that n is valid
   T&       operator[](size_t n) { return mData[(mPos + n) % mSize]; }
   const T& operator[](size_t n) const { return mData[(mPos + n) % mSize]; }
   void     Push(const T& aValue)
   {
      mPos        = (mPos == 0 ? mSize - 1 : mPos - 1);
      mData[mPos] = aValue;
   }
   void           Pop(size_t aPopCount = 1) { mPos = (mPos + aPopCount) % mSize; }
   void           Clear() { mPos = 1; }
   const size_t   mSize;
   size_t         mPos;
   std::vector<T> mData;
};

template<typename T>
class UtCircularBufferIterator;

template<typename T>
class UtCircularBufferIteratorConst;

// A circular buffer that may grow.  Allows push/pop to front or back.
template<typename T>
class UtCircularBuffer
{
public:
   typedef T                                value_type;
   typedef const T&                         const_reference;
   typedef const T*                         const_pointer;
   typedef T&                               reference;
   typedef T*                               pointer;
   typedef int                              difference_type;
   typedef UtCircularBufferIterator<T>      iterator;
   typedef UtCircularBufferIteratorConst<T> const_iterator;

   UtCircularBuffer()
      : mSize(0)
      , mStartIndex(0)
   {
   }

   void push_back(const T& aValue)
   {
      GrowSize(mSize + 1);
      mBuffer[OffsetToIndex(mSize - 1)] = aValue;
   }

   void push_front(const T& aValue)
   {
      GrowSize(mSize + 1);
      if (mStartIndex > 0)
      {
         --mStartIndex;
      }
      else
      {
         mStartIndex = (int)mBuffer.size() - 1;
      }
      mBuffer[mStartIndex] = aValue;
   }

   void pop_back()
   {
      assert(mSize > 0);
      --mSize;
   }

   void pop_front()
   {
      assert(mSize > 0);
      --mSize;
      ++mStartIndex;
      if (mStartIndex >= mBuffer.size())
      {
         mStartIndex = 0;
      }
   }

   void clear()
   {
      mBuffer.clear();
      mStartIndex = 0;
      mSize       = 0;
   }

   T& operator[](size_t aIndex)
   {
      assert(aIndex < mSize);
      return mBuffer.at(OffsetToIndex(aIndex));
   }

   const T& operator[](size_t aIndex) const
   {
      assert(aIndex < mSize);
      return mBuffer.at(OffsetToIndex(aIndex));
   }

   // Returns the underlying raw arrays of elements
   // as 0, 1, or 2 pointers.  If data wraps beyond
   // the end of the buffer, 2 arrays are returned.
   // If data does not wrap, 1 array is returned.
   // if the buffer is empty, no array is returned.
   // returns the number of arrays returned
   size_t GetArrays(T*& aFirstSegmentPtr, size_t& aFirstSegmentSize, T*& aSecondSegmentPtr, size_t& aSecondSegmentSize)
   {
      aFirstSegmentPtr = aSecondSegmentPtr = 0;
      aFirstSegmentSize = aSecondSegmentSize = 0;
      if (mSize == 0)
      {
         return 0;
      }
      aFirstSegmentPtr = &mBuffer[mStartIndex];
      size_t endOffset = mStartIndex + mSize;
      if (endOffset > mBuffer.size())
      {
         aFirstSegmentSize  = mBuffer.size() - mStartIndex;
         aSecondSegmentPtr  = &mBuffer[0];
         aSecondSegmentSize = mSize - aFirstSegmentSize;
         return 2;
      }
      else
      {
         aFirstSegmentSize = mSize;
         return 1;
      }
   }

   T& front()
   {
      assert(mSize > 0);
      return mBuffer[mStartIndex];
   }
   const T& front() const
   {
      assert(mSize > 0);
      return mBuffer[mStartIndex];
   }

   T& back()
   {
      assert(mSize > 0);
      return mBuffer[OffsetToIndex(mSize - 1)];
   }
   const T& back() const
   {
      assert(mSize > 0);
      return mBuffer[OffsetToIndex(mSize - 1)];
   }

   size_t size() const { return mSize; }
   bool   empty() const { return mSize == 0; }

   iterator begin();
   iterator end();

   const_iterator begin() const;
   const_iterator end() const;

   const_iterator cbegin() const;
   const_iterator cend() const;

private:
   size_t OffsetToIndex(size_t aIndex) const
   {
      size_t L = mBuffer.size() - mStartIndex;
      if (aIndex < L)
      {
         return aIndex + mStartIndex;
      }
      return aIndex - L;
   }

   void GrowSize(size_t aSize)
   {
      size_t capacity = mBuffer.size();
      if (aSize <= capacity)
      {
         mSize = aSize;
         return;
      }
      ExpandCapacity(aSize);
      mSize = aSize;
   }

   void ExpandCapacity(size_t aNewMinSize)
   {
      size_t         newSize = (size_t)(aNewMinSize * 1.4f);
      std::vector<T> newValues;

      if (mSize > 0)
      {
         newValues.reserve(newSize);
         size_t endOffset = GetFirstSegmentEnd();
         newValues.insert(newValues.end(), mBuffer.begin() + mStartIndex, mBuffer.begin() + endOffset);
         size_t firstSegmentSize = endOffset - mStartIndex;
         if (firstSegmentSize < mSize)
         {
            newValues.insert(newValues.end(), mBuffer.begin(), mBuffer.begin() + (mSize - firstSegmentSize));
         }
         // Add filler values
         newValues.insert(newValues.end(), (size_t)(newSize - mSize), T());
      }
      else
      {
         newValues.resize(newSize);
      }
      std::swap(newValues, mBuffer);
      mStartIndex = 0;
   }

   size_t GetFirstSegmentEnd() const
   {
      size_t offset = mSize + mStartIndex;
      if (offset > mBuffer.size())
      {
         offset = mBuffer.size();
      }
      return offset;
   }
   // number of values in buffer
   size_t mSize;
   // offset of first value
   size_t mStartIndex;
   // buffer
   std::vector<T> mBuffer;
};

template<typename T>
class UtCircularBufferIteratorConst
{
public:
   typedef std::random_access_iterator_tag         iterator_category;
   typedef UtCircularBuffer<T>                     ContainerType;
   typedef typename ContainerType::value_type      value_type;
   typedef typename ContainerType::difference_type difference_type;
   typedef typename ContainerType::const_pointer   pointer;
   typedef typename ContainerType::const_reference reference;

   typedef UtCircularBufferIteratorConst<T> ThisType;

   UtCircularBufferIteratorConst()
      : mBufferPtr(0)
      , mIndex(0)
   {
   }

   UtCircularBufferIteratorConst(const ContainerType* aContainerPtr, size_t aIndex)
      : mBufferPtr(aContainerPtr)
      , mIndex(aIndex)
   {
   }

   const reference operator*() const { return mBufferPtr->operator[](mIndex); }

   const pointer operator->() const
   {
      // return pointer to class object
      return &mBufferPtr->operator[](mIndex);
   }

   // preincrement
   ThisType& operator++()
   {
      ++mIndex;
      return *this;
   }

   // postincrement
   ThisType operator++(int)
   {
      ThisType tmp = *this;
      ++mIndex;
      return tmp;
   }

   // predecrement
   ThisType& operator--()
   {
      --mIndex;
      return (*this);
   }

   // postdecrement
   ThisType operator--(int)
   {
      ThisType tmp = *this;
      --*this;
      return (tmp);
   }

   // increment by integer
   ThisType& operator+=(difference_type aOffset)
   {
      mIndex += aOffset;
      return (*this);
   }

   // return this + integer
   ThisType operator+(difference_type aOffset) const
   {
      ThisType tmp = *this;
      return (tmp += aOffset);
   }

   // decrement by integer
   ThisType& operator-=(difference_type aOffset) { return (*this += -aOffset); }

   // return this - integer
   ThisType operator-(difference_type aOffset) const
   {
      ThisType _Tmp = *this;
      return (_Tmp -= aOffset);
   }

   // return difference of iterators
   difference_type operator-(const ThisType& aRhs) const { return (int)mIndex - (int)aRhs.mIndex; }

   // subscript
   reference operator[](difference_type aOffset) const { return mBufferPtr->operator[](mIndex + aOffset); }

   bool operator==(const ThisType& aRhs) const
   {
      assert(mBufferPtr == aRhs.mBufferPtr);
      return mIndex == aRhs.mIndex;
   }

   bool operator!=(const ThisType& aRhs) const
   {
      assert(mBufferPtr == aRhs.mBufferPtr);
      return mIndex != aRhs.mIndex;
   }

   bool operator<(const ThisType& aRhs) const
   {
      assert(mBufferPtr == aRhs.mBufferPtr);
      return mIndex < aRhs.mIndex;
   }

protected:
   const UtCircularBuffer<T>* mBufferPtr;
   size_t                     mIndex;
};


template<typename T>
class UtCircularBufferIterator : public UtCircularBufferIteratorConst<T>
{
public:
   typedef std::random_access_iterator_tag         iterator_category;
   typedef UtCircularBuffer<T>                     ContainerType;
   typedef typename ContainerType::value_type      value_type;
   typedef typename ContainerType::difference_type difference_type;
   typedef typename ContainerType::pointer         pointer;
   typedef typename ContainerType::reference       reference;

   typedef UtCircularBufferIteratorConst<T> BaseType;
   typedef UtCircularBufferIterator<T>      ThisType;

   UtCircularBufferIterator() {}

   UtCircularBufferIterator(ContainerType* aContainerPtr, size_t aIndex)
      : UtCircularBufferIteratorConst<T>(aContainerPtr, aIndex)
   {
   }

   reference operator*() const { return const_cast<reference>(BaseType::operator*()); }

   // return pointer to class object
   pointer operator->() const { return const_cast<pointer>(BaseType::operator->()); }

   // preincrement
   ThisType& operator++()
   {
      ++this->mIndex;
      return *this;
   }

   // postincrement
   ThisType operator++(int)
   {
      ThisType tmp = *this;
      ++this->mIndex;
      return tmp;
   }

   // predecrement
   ThisType& operator--()
   {
      --this->mIndex;
      return *this;
   }
   // postdecrement
   ThisType operator--(int)
   {
      ThisType tmp = *this;
      --*this;
      return (tmp);
   }

   // increment by integer
   ThisType& operator+=(difference_type aOffset)
   {
      this->mIndex += aOffset;
      return (*this);
   }

   // return this + integer
   ThisType operator+(difference_type aOffset) const
   {
      ThisType tmp = *this;
      return (tmp += aOffset);
   }

   // decrement by integer
   ThisType& operator-=(difference_type aOffset) { return (*this += -aOffset); }

   // return this - integer
   ThisType operator-(difference_type aOffset) const
   {
      ThisType _Tmp = *this;
      return (_Tmp -= aOffset);
   }

   // subscript
   reference operator[](difference_type aOffset) const { return this->mBufferPtr->operator[](this->mIndex + aOffset); }

private:
};


template<typename T>
inline typename UtCircularBuffer<T>::iterator UtCircularBuffer<T>::begin()
{
   return iterator(this, 0);
}

template<typename T>
inline typename UtCircularBuffer<T>::iterator UtCircularBuffer<T>::end()
{
   return iterator(this, mSize);
}

template<typename T>
inline typename UtCircularBuffer<T>::const_iterator UtCircularBuffer<T>::begin() const
{
   return const_iterator(this, 0);
}

template<typename T>
inline typename UtCircularBuffer<T>::const_iterator UtCircularBuffer<T>::end() const
{
   return const_iterator(this, mSize);
}


template<typename T>
inline typename UtCircularBuffer<T>::const_iterator UtCircularBuffer<T>::cbegin() const
{
   return const_iterator(this, 0);
}

template<typename T>
inline typename UtCircularBuffer<T>::const_iterator UtCircularBuffer<T>::cend() const
{
   return const_iterator(this, mSize);
}


#endif
