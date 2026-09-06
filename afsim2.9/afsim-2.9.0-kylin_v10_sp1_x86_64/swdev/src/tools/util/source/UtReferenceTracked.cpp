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
#include "UtReferenceTracked.hpp"

UtReferenceTracked::UtReferenceTracked()
   : mTrackedReferencePtr(nullptr)
{
}

UtReferenceTracked::UtReferenceTracked(const UtReferenceTracked& /*aRhs*/)
   : mTrackedReferencePtr(nullptr)
{
}

//! Returns the reference count for this object.  One is created if necessary.
//! Users may use UtReferenceCount::IsValid() at a later time to determine if this object still available.
UtReferenceCount* UtReferenceTracked::GetReferenceCount() const
{
   if (mTrackedReferencePtr == nullptr)
   {
      mTrackedReferencePtr = new UtReferenceCount(1);
   }
   return mTrackedReferencePtr;
}

//! FreeReference() must be called before the object is deleted.
void UtReferenceTracked::FreeReference()
{
   if (mTrackedReferencePtr != nullptr)
   {
      mTrackedReferencePtr->RemoveStrongRef();
      mTrackedReferencePtr = nullptr;
   }
}

UtReferenceCounted::UtReferenceCounted()
   : UtReferenceTracked()
{
   mTrackedReferencePtr = new UtReferenceCount(1);
}

UtReferenceCounted::UtReferenceCounted(const UtReferenceTracked& aRhs)
   : UtReferenceTracked(aRhs)
{
   mTrackedReferencePtr = new UtReferenceCount(1);
}
