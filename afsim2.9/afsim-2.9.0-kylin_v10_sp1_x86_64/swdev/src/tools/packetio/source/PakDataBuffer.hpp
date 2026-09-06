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

#ifndef PAKDATABUFFER_HPP
#define PAKDATABUFFER_HPP

#include <algorithm>

#include "GenMemIO.hpp"
#include "PakI.hpp"
#include "PakO.hpp"
#include "PakSerialize.hpp"

//! A buffer of data which can be 'serialized' into and out of using the '&' operator.
//! Prior to using the '&' operator, xio/WsfXIO_Serialization.hpp should be included
class PakDataBuffer;

class PakDataBuffer_InputStream
{
public:
   friend class PakDataBuffer;
   PakDataBuffer_InputStream(PakDataBuffer* aDataPtr);
   ~PakDataBuffer_InputStream() { delete mArchive; }
   mutable PakDataBuffer* mDataPtr;
   mutable PakI*          mArchive;

private:
   void operator=(const PakDataBuffer_InputStream&);
};

class PakDataBuffer_OutputStream
{
public:
   friend class PakDataBuffer;
   PakDataBuffer_OutputStream(PakDataBuffer* aDataPtr); // : mDataPtr(aDataPtr) {}
   ~PakDataBuffer_OutputStream() { delete mArchive; }
   mutable PakDataBuffer* mDataPtr;
   mutable PakO*          mArchive;
};


class PakDataBuffer
{
public:
   friend class PakDataBuffer_InputStream;
   friend class PakDataBuffer_OutputStream;
   PakDataBuffer()                          = default;
   PakDataBuffer(const PakDataBuffer& aRhs) = delete;

   //! Returns an object compatible with operator& for
   //! copying data out of this buffer
   PakDataBuffer_InputStream In();

   //! Returns an object compatible with operator& for
   //! copying data into this buffer
   PakDataBuffer_OutputStream Out();

   void Reset() { mData.SetPutPos(0); }
   template<typename AR>
   void Serialize(AR& aAr)
   {
      aAr& mData;
   }
   bool operator==(const PakDataBuffer& aRHS) const
   {
      bool ok = false;
      if (mData.GetPutPos() == aRHS.mData.GetPutPos())
      {
         ok = std::equal(mData.GetBuffer(), mData.GetBuffer() + mData.GetPutPos(), aRHS.mData.GetBuffer());
      }
      return ok;
   }
   void           Swap(PakDataBuffer& aRHS) { mData.SwapBuffer(aRHS.mData); }
   PakDataBuffer& operator=(PakDataBuffer& aRhs)
   {
      aRhs.mData.SetGetPos(0);
      size_t length = aRhs.mData.GetPutPos();
      mData.Reset();
      mData.PutRaw(aRhs.mData.GetBuffer(), length);
      return *this;
   }
   GenBuffer mData;

private:
};

inline PakDataBuffer_InputStream::PakDataBuffer_InputStream(PakDataBuffer* aDataPtr)
   : mDataPtr(aDataPtr)
   , mArchive(nullptr)
{
}

inline PakDataBuffer_OutputStream::PakDataBuffer_OutputStream(PakDataBuffer* aDataPtr)
   : mDataPtr(aDataPtr)
   , mArchive(nullptr)
{
}

inline PakDataBuffer_OutputStream PakDataBuffer::Out()
{
   return PakDataBuffer_OutputStream(this);
}
inline PakDataBuffer_InputStream PakDataBuffer::In()
{
   return PakDataBuffer_InputStream(this);
}

template<typename T>
inline PakI& operator&(const PakDataBuffer_InputStream& aStream, T& aData)
{
   aStream.mArchive = new PakI(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}
template<typename T>
inline PakI& operator>>(const PakDataBuffer_InputStream& aStream, T& aData)
{
   aStream.mArchive = new PakI(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}

template<typename T>
inline PakO& operator&(const PakDataBuffer_OutputStream& aStream, T& aData)
{
   aStream.mArchive = new PakO(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}
template<typename T>
inline PakO& operator&(const PakDataBuffer_OutputStream& aStream, const T& aData)
{
   aStream.mArchive = new PakO(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}

template<typename T>
inline PakO& operator<<(const PakDataBuffer_OutputStream& aStream, T& aData)
{
   aStream.mArchive = new PakO(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}
template<typename T>
inline PakO& operator<<(const PakDataBuffer_OutputStream& aStream, const T& aData)
{
   aStream.mArchive = new PakO(&aStream.mDataPtr->mData);
   return *aStream.mArchive & aData;
}

#endif
