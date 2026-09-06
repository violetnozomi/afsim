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

#ifndef UTREFERENCETRACKED_HPP
#define UTREFERENCETRACKED_HPP

#include "ut_export.h"

#include "UtReferenceCount.hpp"

//! Maintains a reference count for derived classes.  A single strong-reference count is maintained
//! by this class, other classes may only make weak references to the object.
class UT_EXPORT UtReferenceTracked
{
public:
   UtReferenceTracked();
   UtReferenceTracked(const UtReferenceTracked& aRhs);

   UtReferenceCount* GetReferenceCount() const;

protected:
   void FreeReference();

   UtReferenceCount* GetReferenceCountP();

   mutable UtReferenceCount* mTrackedReferencePtr;
};

//! Maintains a reference count for derived classes.  A reference count is used to determine object lifetime
class UT_EXPORT UtReferenceCounted : public UtReferenceTracked
{
public:
   UtReferenceCounted();
   UtReferenceCounted(const UtReferenceTracked& aRhs);
   virtual ~UtReferenceCounted() = default;

   void AddRef() { GetReferenceCount()->AddStrongRef(); }
   void Unref()
   {
      if (GetReferenceCount()->RemoveStrongRef())
         delete this;
   }

protected:
};
#endif
