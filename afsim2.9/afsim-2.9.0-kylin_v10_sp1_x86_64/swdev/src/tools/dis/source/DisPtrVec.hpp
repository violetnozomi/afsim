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

// This is an internal class that should only be used within this project.
//
// This is a very special implementation of a vector-type class that is
// intended to hold only pointers...

#ifndef DISPTRVEC_HPP
#define DISPTRVEC_HPP

#include "dis_export.h"

#include <cassert>

template<class T>
class DIS_EXPORT DisPtrVec
{
private:
   typedef T* TPTR;

public:
   DisPtrVec()
      : mSize(0)
      , mAddr(nullptr)
   {
   }

   // Copy Constructor
   DisPtrVec(const DisPtrVec& aSrc) { Copy(aSrc); }

   ~DisPtrVec() { RemoveAll(); }

   // Operator=
   DisPtrVec<T>& operator=(const DisPtrVec<T>& aRhs)
   {
      // Don't let the user copy over existing object
      if (this == &aRhs)
      {
         return *this;
      }

      RemoveAll();
      Copy(aRhs);

      return *this;
   }

   // Add a pointer to an entry.
   void Add(TPTR aNewPtr)
   {
      TPTR* oldAddr = mAddr;
      mAddr         = new TPTR[mSize + 1];
      if (oldAddr != nullptr)
      {
         for (unsigned int i = 0; i < mSize; ++i)
         {
            mAddr[i] = oldAddr[i];
         }
         delete[] oldAddr;
      }
      mAddr[mSize] = aNewPtr;
      ++mSize;
   }

   unsigned int GetSize() const { return mSize; }

   // Remove the pointer and the thing it points to...
   void Remove(unsigned int aIndex)
   {
      if (aIndex < mSize)
      {
         if (mSize == 1)
         {
            RemoveAll();
         }
         else
         {
            delete mAddr[aIndex];
            TPTR* oldAddr  = mAddr;
            mAddr          = new TPTR[mSize - 1];
            unsigned int j = 0;
            for (unsigned int i = 0; i < mSize; ++i)
            {
               if (i != aIndex)
               {
                  mAddr[j] = oldAddr[i];
                  ++j;
               }
            }
            delete[] oldAddr;
            --mSize;
         }
      }
   }

   // Remove all the pointers and the thing each points to...
   void RemoveAll()
   {
      for (unsigned int i = 0; i < mSize; ++i)
      {
         delete mAddr[i];
      }
      delete[] mAddr;
      mSize = 0;
      mAddr = nullptr;
   }

   // Set the size.. ALL EXISTING ELEMENTS ARE ERASED!
   void Resize(unsigned int aNewSize)
   {
      RemoveAll();
      mSize = aNewSize;
      mAddr = new TPTR[mSize];
      for (unsigned int i = 0; i < mSize; ++i)
      {
         mAddr[i] = nullptr;
      }
   }

   TPTR& operator[](unsigned int aIndex)
   {
      assert(aIndex < mSize);
      return mAddr[aIndex];
   }

   const TPTR& operator[](unsigned int aIndex) const
   {
      assert(aIndex < mSize);
      return mAddr[aIndex];
   }

private:
   // Copy the actual data
   void Copy(const DisPtrVec& aSrc)
   {
      mSize = aSrc.mSize;
      mAddr = new TPTR[mSize];
      for (unsigned int i = 0; i < mSize; ++i)
      {
         mAddr[i] = new T(*aSrc.mAddr[i]);
         assert(mAddr[i]);
      }
   }

   unsigned int mSize;
   TPTR*        mAddr;
};

#endif
