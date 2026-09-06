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

#ifndef UTSPATIALDATUM_HPP
#define UTSPATIALDATUM_HPP

#include "ut_export.h"

#include <list>

class UtBoundingBox;
template<size_t Dim>
class UtSpatialNode;

class UtSpatialDatum;

//! The following typedef is used as an interface to retrieve the data.
//! TODO this may change to a UtMappedList.
using SpatialData = std::list<const UtSpatialDatum*>;

//! A base class for data contained in a UtSpatialTree container.
class UT_EXPORT UtSpatialDatum
{
public:
   UtSpatialDatum()
      : mId(GetNextId())
   {
   }

   UtSpatialDatum(unsigned int aId)
      : mId(aId)
   {
   }

   virtual ~UtSpatialDatum() = default;

   virtual bool IsPoint() const = 0;

   //! Should return 'true' if the datum fits entirely in aBox.  This is only used for point-datums
   //! inserted with UtSpatialTree::Insert().
   virtual bool IsWhollyContainedIn(const UtBoundingBox& aBox) const = 0;
   //! Should return 'true' if the datum is partially contained in aBox.  This is only used for area-datums
   //! inserted with UtSpatialTree::InsertArea().
   virtual bool IsPartiallyContainedIn(const UtBoundingBox& aBox) const = 0;

   unsigned int GetId() const { return mId; }

protected:
   static int GetNextId()
   {
      static int nextId = 0;
      return ++nextId;
   }

private:
   unsigned int mId;
};

using UtQuadDatum = UtSpatialDatum;
using UtOctDatum  = UtSpatialDatum;

#endif
