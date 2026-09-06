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

#include "WsfTaskResource.hpp"

#include <cmath>
#include <limits>

//! Returns true if this is a match for resource 'aRhs'.
bool WsfTaskResource::IsMatch(const WsfTaskResource& aRhs) const
{
   return ((GetType() == aRhs.GetType() || aRhs.GetType() == cTASK_RESOURCE_TYPE_NONE) &&
           (mNameId == aRhs.mNameId || aRhs.mModeNameId == nullptr));
}

WsfTaskResource* WsfTaskResource::Clone() const
{
   return new WsfTaskResource(*this);
}

// virtual
WsfStringId WsfTaskResource::GetTypeName() const
{
   const WsfStringId typeNames[] = {WsfStringId("None"),
                                    WsfStringId("Sensor"),
                                    WsfStringId("Processor"),
                                    WsfStringId("Unknown3"),
                                    WsfStringId("Unknown4"),
                                    WsfStringId("Unknown5")};
   return typeNames[mType];
}
