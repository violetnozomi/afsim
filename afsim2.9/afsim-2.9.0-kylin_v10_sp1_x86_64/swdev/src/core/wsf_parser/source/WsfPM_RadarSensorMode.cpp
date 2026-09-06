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

#include "WsfPM_RadarSensorMode.hpp"

WsfPM_RadarSensorMode::WsfPM_RadarSensorMode() {}

WsfPM_RadarSensorMode::WsfPM_RadarSensorMode(const WsfPProxyNode& aNode)
   : WsfPProxyNode(aNode)
{
}

WsfPM_Transmitter WsfPM_RadarSensorMode::transmitter() const
{
   const WsfPM_ObjectMap beams(*this + "beams");
   const WsfPProxyNode&  xmtrAntenna = beams.At(beams.begin()) + "xmtrRcvr";
   return WsfPM_Transmitter(xmtrAntenna + "transmitter");
}
