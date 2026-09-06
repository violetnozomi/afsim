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

#ifndef WSFXIO_PLATFORMID_HPP
#define WSFXIO_PLATFORMID_HPP

#include "wsf_export.h"

#include <iosfwd>

class WsfXIO_PlatformId
{
public:
   WsfXIO_PlatformId()
      : mIndex(0)
      , mConnectionId(0)
   {
   }

   WsfXIO_PlatformId(unsigned int aIndex, int aConnectionId)
      : mIndex(aIndex)
      , mConnectionId(aConnectionId)
   {
   }

   bool operator<(const WsfXIO_PlatformId& aRhs) const
   {
      if (mIndex < aRhs.mIndex)
      {
         return true;
      }
      if (mIndex > aRhs.mIndex)
      {
         return false;
      }
      return mConnectionId < aRhs.mConnectionId;
   }

   bool operator==(const WsfXIO_PlatformId& aRhs) const
   {
      return mIndex == aRhs.mIndex && mConnectionId == aRhs.mConnectionId;
   }

   bool operator!=(const WsfXIO_PlatformId& aRhs) const { return !operator==(aRhs); }

   // stream extractor
   friend WSF_EXPORT std::istream& operator>>(std::istream& aIn, WsfXIO_PlatformId& aId);

   // stream inserter
   friend WSF_EXPORT std::ostream& operator<<(std::ostream& aOut, const WsfXIO_PlatformId& aId);

   unsigned int mIndex;
   int          mConnectionId;
};

#endif
