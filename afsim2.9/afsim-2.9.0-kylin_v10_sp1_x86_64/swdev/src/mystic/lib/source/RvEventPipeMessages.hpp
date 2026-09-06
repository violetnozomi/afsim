// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RvEventPipeMESSAGES_HPP
#define RvEventPipeMESSAGES_HPP

#include <string>

#include <QAtomicInt>

#include "UtPackMessage.hpp"
#include "UtVec3.hpp"

namespace rv
{
// specific class injected to add reference counting for messages
class MessageBase : public UtPackMessage
{
public:
   MessageBase() = default;

   void Ref() const { mRefCount.ref(); }
   void Unref() const
   {
      if (!mRefCount.deref())
      {
         delete this;
      }
   }
   unsigned int GetMessageIndex() const { return mMessageIndex; }

   void SetMessageIndex(unsigned int aIndex) { mMessageIndex = aIndex; }

   struct PtrOrderByIndex
   {
      bool operator()(const MessageBase* aLhs, const MessageBase* aRhs) const
      {
         return aLhs->mMessageIndex < aRhs->mMessageIndex;
      }
   };

   // MESSAGE Field IDs for display in tabular form
   enum
   {
      cFIELD_TIME      = 0,
      cFIELD_SIM       = 1,
      cFIELD_TYPE      = 2,
      cFIELD_OWNER     = 3,
      cFIELD_TARGET    = 4,
      cFIELD_TRACK_ID  = 5,
      cFIELD_COMPONENT = 6,
      cFIELD_DATA      = 7
      // fields beyond here (cFIELD_DATA + 1, etc...) will be used for additional data
   };

private:
   mutable QAtomicInt mRefCount{1};
   unsigned int       mMessageIndex{0};
};

//! a ref-counted reference to a message
template<typename T>
class MsgPtr
{
public:
   MsgPtr()
      : mPtr(0)
   {
   }
   ~MsgPtr()
   {
      if (mPtr)
         mPtr->Unref();
   }
   MsgPtr(const MsgPtr& aSrc)
      : mPtr(aSrc.mPtr)
   {
      if (mPtr)
         mPtr->Ref();
   }
   MsgPtr& operator=(const T* aPtr)
   {
      if (mPtr != aPtr)
      {
         if (mPtr)
            mPtr->Unref();
         mPtr = aPtr;
         if (mPtr)
            mPtr->Ref();
      }
      return *this;
   }
   const T* operator->() const { return mPtr; }
   const T* Get() const { return mPtr; }
   bool     IsValid() const { return mPtr != 0; }

private:
   const T* mPtr;
};
} // namespace rv

#endif
