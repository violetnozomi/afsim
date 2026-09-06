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

#include "WsfWeaponTaskResource.hpp"

#include <cfloat>
#include <cmath>

// virtual
WsfStringId WsfWeaponTaskResource::GetTypeName() const
{
   const WsfStringId typeNames[] = {WsfStringId("None"),
                                    WsfStringId("Sensor"),
                                    WsfStringId("Processor"),
                                    WsfStringId("Weapon"),
                                    WsfStringId("Jammer"),
                                    WsfStringId("Uplink")};
   return typeNames[mType];
}

WsfTaskResource* WsfWeaponTaskResource::Clone() const
{
   return new WsfWeaponTaskResource(*this);
}

// virtual
WsfStringId WsfJammerTaskResource::GetTypeName() const
{
   const WsfStringId typeNames[] = {WsfStringId("None"),
                                    WsfStringId("Sensor"),
                                    WsfStringId("Processor"),
                                    WsfStringId("Weapon"),
                                    WsfStringId("Jammer"),
                                    WsfStringId("Uplink")};
   return typeNames[mType];
}

WsfTaskResource* WsfJammerTaskResource::Clone() const
{
   return new WsfJammerTaskResource(*this);
}

bool WsfJammerTaskResource::IsMatch(const WsfTaskResource& aRhs) const
{
   WsfJammerTaskResource& rhs = (WsfJammerTaskResource&)aRhs;
   return (WsfTaskResource::IsMatch(aRhs) &&
           (!(aRhs.GetType() == GetType()) || ((fabs(rhs.mFrequency - mFrequency) < DBL_EPSILON || rhs.mFrequency == 0) &&
                                               (fabs(rhs.mBandwidth - mBandwidth) < DBL_EPSILON || rhs.mBandwidth == 0))));
}

// virtual
WsfStringId WsfUplinkTaskResource::GetTypeName() const
{
   const WsfStringId typeNames[] = {WsfStringId("None"),
                                    WsfStringId("Sensor"),
                                    WsfStringId("Processor"),
                                    WsfStringId("Weapon"),
                                    WsfStringId("Jammer"),
                                    WsfStringId("Uplink")};
   return typeNames[mType];
}

WsfTaskResource* WsfUplinkTaskResource::Clone() const
{
   return new WsfUplinkTaskResource(*this);
}
