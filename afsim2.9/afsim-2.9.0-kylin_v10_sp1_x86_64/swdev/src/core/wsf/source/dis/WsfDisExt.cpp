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

#include "dis/WsfDisExt.hpp"

// static
//! Convert a WsfExtEntityType to a DisEntityType
DisEntityType WsfDisExt::ToDis(const WsfExtEntityType& aEntityType)
{
   return DisEntityType(aEntityType.mEntityKind,
                        aEntityType.mDomain,
                        aEntityType.mCountry,
                        aEntityType.mCategory,
                        aEntityType.mSubcategory,
                        aEntityType.mSpecific,
                        aEntityType.mExtra);
}

// static
//! Convert a DisEntityType to a WsfExtEntityType
WsfExtEntityType WsfDisExt::ToExt(const DisEntityType& aEntityType)
{
   return WsfExtEntityType(aEntityType.GetEntityKind(),
                           aEntityType.GetDomain(),
                           aEntityType.GetCountry(),
                           aEntityType.GetCategory(),
                           aEntityType.GetSubcategory(),
                           aEntityType.GetSpecific(),
                           aEntityType.GetExtra());
}

// static
DisEntityId WsfDisExt::ToDis(const WsfExtEntityId& aEntityId)
{
   return DisEntityId((DisUint16)aEntityId.GetSite(),
                      (DisUint16)aEntityId.GetApplication(),
                      (DisUint16)aEntityId.GetEntity());
}

// static
WsfExtEntityId WsfDisExt::ToExt(const DisEntityId& aEntityId)
{
   return WsfExtEntityId(aEntityId.GetSite(), aEntityId.GetApplication(), aEntityId.GetEntity());
}
