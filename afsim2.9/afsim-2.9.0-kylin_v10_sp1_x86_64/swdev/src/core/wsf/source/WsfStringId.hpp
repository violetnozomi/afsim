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

#ifndef WSFSTRINGID_HPP
#define WSFSTRINGID_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>

#include "UtStringId.hpp"

//! WsfStringId -- the same thing as UtStringId
using WsfStringId = UtStringId;

//! Like WsfStringId, but can be converted to and from integer.  Added when WsfStringId
//! became more strict.  In general, this class should not be used.  It is only here
//! for backwards compatibility
class WsfStringInt : public WsfStringId
{
public:
   WsfStringInt(const std::string& aSrc)
      : WsfStringId(aSrc)
   {
   }
   WsfStringInt(const char* aSrc)
      : WsfStringId(aSrc)
   {
   }
   WsfStringInt(const WsfStringId& aRhs)
      : WsfStringId(aRhs)
   {
   }
   explicit WsfStringInt(int aId)
      : WsfStringId()
   {
      mId = aId;
   }
   WsfStringInt()
      : WsfStringId()
   {
   }

                 operator int() const { return mId; }
   WsfStringInt& operator=(int aRhs)
   {
      mId = aRhs;
      return *this;
   }
   bool operator==(const WsfStringInt& aRhs) const { return (mId == aRhs.mId); }
   bool operator!=(const WsfStringInt& aRhs) const { return (mId != aRhs.mId); }

   friend bool operator==(int aId, const WsfStringInt& aRhs);
   friend bool operator!=(int aId, const WsfStringInt& aRhs);

   operator const std::string&() const { return (const WsfStringId&)*this; }
};

//! Equal relational operator for the case of an integer on the LHS and a StringId on the RHS.
inline bool operator==(int aId, const WsfStringInt& aRhs)
{
   return (aId == aRhs.GetId());
}

//! Not equal relational operator for the case of an integer on the LHS and a StringId on the RHS.
inline bool operator!=(int aId, const WsfStringInt& aRhs)
{
   return (aId != aRhs.GetId());
}

#endif
