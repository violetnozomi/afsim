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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfMilCommTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommTypes.hpp"
#include "WsfScenario.hpp"

// Include files for built-in types
#include "WsfJTIDS_Terminal.hpp"
#include "WsfLASER_XmtrRcvr.hpp"
#include "WsfSubsurfaceRadioXmtrRcvr.hpp"

void WsfMilCommTypes::Register(WsfScenario& aScenario)
{
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_JTIDS_TERMINAL", ut::make_unique<WsfJTIDS_Terminal>(aScenario));

   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_SUBSURFACE_RADIO_RCVR",
                                                ut::make_unique<WsfSubsurfaceRadioXmtrRcvr>(aScenario,
                                                                                            wsf::comm::Comm::cCT_RCV_ONLY));
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_SUBSURFACE_RADIO_XMTR",
                                                ut::make_unique<WsfSubsurfaceRadioXmtrRcvr>(aScenario,
                                                                                            wsf::comm::Comm::cCT_XMT_ONLY));
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_SUBSURFACE_RADIO_XMTR_RCVR",
                                                ut::make_unique<WsfSubsurfaceRadioXmtrRcvr>(aScenario));
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_SUBSURFACE_RADIO_TRANSCEIVER",
                                                ut::make_unique<WsfSubsurfaceRadioXmtrRcvr>(
                                                   aScenario)); // An alias for WSF_SUBSURFACE_RADIO_XMTR_RCVR

   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_LASER_RCVR",
                                                ut::make_unique<wsf::comm::LASER_XmtrRcvr>(aScenario,
                                                                                           wsf::comm::Comm::cCT_RCV_ONLY));
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_LASER_XMTR",
                                                ut::make_unique<wsf::comm::LASER_XmtrRcvr>(aScenario,
                                                                                           wsf::comm::Comm::cCT_XMT_ONLY));
   wsf::comm::Types::Get(aScenario).AddCoreType("WSF_LASER_XMTR_RCVR",
                                                ut::make_unique<wsf::comm::LASER_XmtrRcvr>(aScenario,
                                                                                           wsf::comm::Comm::cCT_XMT_RCV));
   wsf::comm::Types::Get(aScenario).AddCoreType(
      "WSF_LASER_TRANSCEIVER",
      ut::make_unique<wsf::comm::LASER_XmtrRcvr>(aScenario, wsf::comm::Comm::cCT_XMT_RCV)); // An alias for WSF_LASER_XMTR_RCVR
}
