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

#include "WsfPM_PlatformPart.hpp"

WsfPM_PlatformPart::WsfPM_PlatformPart() {}

WsfPM_PlatformPart::WsfPM_PlatformPart(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

bool WsfPM_PlatformPart::on()
{
   return GetValue().GetAttr("on").GetBoolValue();
}

void WsfPM_PlatformPart::setOn(bool on)
{
   GetValue().GetAttr("on").SetBoolValue(on);
}

bool WsfPM_PlatformPart::operational()
{
   return GetValue().GetAttr("operational").GetBoolValue();
}

void WsfPM_PlatformPart::setOperational(bool operational)
{
   GetValue().GetAttr("operational").SetBoolValue(operational);
}

bool WsfPM_PlatformPart::restorable()
{
   return GetValue().GetAttr("restorable").GetBoolValue();
}

void WsfPM_PlatformPart::setRestorable(bool restorable)
{
   GetValue().GetAttr("restorable").SetBoolValue(restorable);
}
