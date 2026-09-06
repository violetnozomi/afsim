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

#ifndef UTPACKMESSAGE_HPP
#define UTPACKMESSAGE_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Types used in generated classes
//////////////////////////////////////////////////////////////////////////
class UtPackReflector;

class UtPackListInterface
{
public:
   virtual ~UtPackListInterface()  = default;
   virtual int   GetSize() const   = 0;
   virtual void  Resize(int aSize) = 0;
   virtual void* GetPtr()          = 0;
};

// List implementation.  A wrapper around std::vector to provide serialization
template<typename T>
class UtPackList : public UtPackListInterface
{
public:
   ~UtPackList() override = default;

   std::vector<T>& GetVector() { return mVector; }

   // std::vector interface
   using iterator       = typename std::vector<T>::iterator;
   using const_iterator = typename std::vector<T>::const_iterator;
   using value_type     = typename std::vector<T>::value_type;
   T&             operator[](size_t aIndex) { return mVector[aIndex]; }
   const T&       operator[](size_t aIndex) const { return mVector[aIndex]; }
   void           push_back(const T& aValue) { mVector.push_back(aValue); }
   void           push_back(T&& aValue) { mVector.push_back(std::move(aValue)); }
   const T&       back() const { return mVector.back(); }
   T&             back() { return mVector.back(); }
   const T&       front() const { return mVector.front(); }
   T&             front() { return mVector.front(); }
   iterator       begin() { return mVector.begin(); }
   const_iterator begin() const { return mVector.begin(); }
   iterator       end() { return mVector.end(); }
   const_iterator end() const { return mVector.end(); }
   iterator       insert(const_iterator aWhere, T& aValue) { return mVector.insert(aWhere, aValue); }
   size_t         size() const { return mVector.size(); }
   bool           empty() const { return mVector.empty(); }
   // UtPackListInterface:
   int   GetSize() const override { return (int)mVector.size(); }
   void  Resize(int aSize) override { return mVector.resize(aSize); }
   void* GetPtr() override { return mVector.empty() ? nullptr : &mVector[0]; }

private:
   std::vector<T> mVector;
};

// Similar to std::bitset, but provides other accessors
template<int BYTE_COUNT>
class UtPackBitset
{
   static constexpr int cBITS_PER_WORD = 32;
   static constexpr int cWORD_COUNT    = (BYTE_COUNT + 3) / 4;

public:
   UtPackBitset() { memset(mWords, 0, cWORD_COUNT * 4); }

   UtPackBitset(const UtPackBitset<BYTE_COUNT>& aSrc) { memcpy(mWords, aSrc.mWords, cWORD_COUNT * 4); }

   UtPackBitset<BYTE_COUNT>& operator=(const UtPackBitset<BYTE_COUNT>& aRhs)
   {
      memcpy(mWords, aRhs.mWords, cWORD_COUNT * 4);
      return *this;
   }
   void set(int aBitIndex, bool aValue)
   {
      const int cWORD_INDEX = aBitIndex / cBITS_PER_WORD;
      const int cBIT        = (1u << (aBitIndex % cBITS_PER_WORD));
      if (aValue)
      {
         mWords[cWORD_INDEX] |= cBIT;
      }
      else
      {
         mWords[cWORD_INDEX] &= ~cBIT;
      }
   }
   void wordOr(unsigned int aMask) { mWords[0] |= aMask; }
   void wordAnd(unsigned int aMask) { mWords[0] &= aMask; }
   void wordXor(unsigned int aMask) { mWords[0] ^= aMask; }
   bool test(int aBitIndex) const
   {
      return 0 != (mWords[aBitIndex / cBITS_PER_WORD] & (1u << (aBitIndex % cBITS_PER_WORD)));
   }

private:
   unsigned int mWords[cWORD_COUNT];
};

// structs marked as messages in the schema will implement this interface
class UtPackMessage
{
public:
   virtual ~UtPackMessage() = default;
   // returns unique ID of the message type
   virtual int            GetMessageId() const   = 0;
   virtual const char*    GetMessageName() const = 0;
   virtual UtPackMessage* Clone() const          = 0;

private:
};

#endif
