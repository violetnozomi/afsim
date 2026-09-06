// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSUBSURFACERADIOXMTRRCVR_HPP
#define WSFSUBSURFACERADIOXMTRRCVR_HPP

#include "wsf_mil_export.h"

#include "WsfComm.hpp"
#include "WsfCommTypes.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfRadioXmtrRcvr.hpp"

//! A specialization of WsfRadioXmtrRcvr.hpp for a radio transmitter-receiver to
//! interact with submerged submarines.
//!
//! A radio object can be instantiated with transmit/receive, transmit-only
//! or receive-only capabilities based on the argument to the constructor.
//! It cannot be changed once instantiated.
class WSF_MIL_EXPORT WsfSubsurfaceRadioXmtrRcvr : public wsf::comm::RadioXmtrRcvr
{
public:
   WsfSubsurfaceRadioXmtrRcvr(WsfScenario& aScenario, wsf::comm::Comm::CommType aCommType = wsf::comm::Comm::cCT_XMT_RCV);

   ~WsfSubsurfaceRadioXmtrRcvr() override = default;
   WsfSubsurfaceRadioXmtrRcvr& operator=(const WsfSubsurfaceRadioXmtrRcvr& aRhs) = delete;

   wsf::comm::Comm* Clone() const override;

protected:
   WsfSubsurfaceRadioXmtrRcvr(const WsfSubsurfaceRadioXmtrRcvr& aSrc) = default;
};

#endif
