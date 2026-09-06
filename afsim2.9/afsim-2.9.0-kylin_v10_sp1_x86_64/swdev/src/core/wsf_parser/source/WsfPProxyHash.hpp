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

#ifndef WSFPPROXYHASH_HPP
#define WSFPPROXYHASH_HPP
#include <cstring>

#include "UtCast.hpp"
#include "UtSHA.hpp"

//! Points to a block of data to be hashed.
//! Copying WsfPProxyHash is like copying a pointer, the original object
//! must remain valid for this to make sense.
class WsfPProxyHash
{
public:
   WsfPProxyHash()
      : mIsDigest(false)
   {
      mDigestOrBuffer.mBuffer.mPointer = nullptr;
      mDigestOrBuffer.mBuffer.mSize    = 0;
   }
   WsfPProxyHash(const void* aBufferPtr, size_t aBufferSize)
   {
      mIsDigest                        = false;
      mDigestOrBuffer.mBuffer.mPointer = aBufferPtr;
      mDigestOrBuffer.mBuffer.mSize    = ut::cast_to_int(aBufferSize);
   }
   WsfPProxyHash(const UtSHA_Digest& aDigest)
   {
      mIsDigest               = true;
      mDigestOrBuffer.mDigest = aDigest;
   }
   WsfPProxyHash(const WsfPProxyHash& aSrc) { (*this) = aSrc; }
   WsfPProxyHash& operator=(const WsfPProxyHash& aRhs)
   {
      mIsDigest = aRhs.mIsDigest;
      if (mIsDigest)
      {
         mDigestOrBuffer.mDigest = aRhs.mDigestOrBuffer.mDigest;
      }
      else
      {
         mDigestOrBuffer.mBuffer = aRhs.mDigestOrBuffer.mBuffer;
      }
      return *this;
   }
   bool operator==(const WsfPProxyHash& aRhs) const
   {
      int size = GetSize();
      if (size != aRhs.GetSize())
      {
         return false;
      }
      return memcmp(GetPointer(), aRhs.GetPointer(), size) == 0;
   }
   bool operator!=(const WsfPProxyHash& aRhs) const { return !(*this == aRhs); }
   bool operator<(const WsfPProxyHash& aRhs) const
   {
      int s1 = GetSize();
      int s2 = aRhs.GetSize();
      if (s1 < s2)
      {
         return true;
      }
      if (s1 > s2)
      {
         return false;
      }
      return memcmp(GetPointer(), aRhs.GetPointer(), s1) < 0;
   }
   //! Add this block of data to a SHA run
   int AddData(UtSHA& aSha) const
   {
      int size = GetSize();
      if (size)
      {
         aSha.AddData((const char*)GetPointer(), size);
      }
      return size;
   }
   const void* GetPointer() const
   {
      return mIsDigest ? &mDigestOrBuffer.mDigest.mBytes[0] : mDigestOrBuffer.mBuffer.mPointer;
   }
   int  GetSize() const { return mIsDigest ? UtSHA_Digest::cBUFFER_SIZE : mDigestOrBuffer.mBuffer.mSize; }
   bool mIsDigest;

private:
   struct Buffer
   {
      const void* mPointer;
      int         mSize;
   };
   union DigestOrBuffer
   {
      Buffer       mBuffer;
      UtSHA_Digest mDigest;
   };
   DigestOrBuffer mDigestOrBuffer;
};

#endif
