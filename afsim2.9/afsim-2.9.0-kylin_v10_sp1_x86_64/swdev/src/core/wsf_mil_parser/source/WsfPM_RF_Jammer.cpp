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

#include "WsfPM_RF_Jammer.hpp"

WsfPM_Transmitter WsfPM_RF_Jammer::Mode::transmitter() const
{
   WsfPProxyNode xmtrAntenna = *this + "xmtrAntenna";
   return WsfPM_Transmitter(xmtrAntenna + "transmitter");
}
