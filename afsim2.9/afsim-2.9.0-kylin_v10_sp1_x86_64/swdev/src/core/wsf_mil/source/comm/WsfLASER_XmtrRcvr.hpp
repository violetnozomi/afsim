// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFLASER_XMTRRCVR_HPP
#define WSFLASER_XMTRRCVR_HPP

#include "wsf_mil_export.h"

#include "WsfComm.hpp"
#include "WsfRadioXmtrRcvr.hpp"
class WsfScenario;

namespace wsf
{
namespace comm
{

//! An implementation of a laser-based comm.  Signal-to-noise calculations are based on
//! actual laser propagation effects (See WsfLASER_XmtrComponent, WsfLASER_RcvrComponent,
//! and WsfCommComponentLASER_HW).
class WSF_MIL_EXPORT LASER_XmtrRcvr : public RadioXmtrRcvr
{
public:
   LASER_XmtrRcvr(WsfScenario& aScenario, CommType aCommType = cCT_XMT_RCV);

   ~LASER_XmtrRcvr() override = default;
   LASER_XmtrRcvr& operator=(const LASER_XmtrRcvr& aRhs) = delete;

   //! @name Common infrastructure methods.
   //@{
   Comm* Clone() const override;
   //@}

protected:
   //! Copy Constructor
   LASER_XmtrRcvr(const LASER_XmtrRcvr& aSrc) = default;
};

} // namespace comm
} // namespace wsf

#endif
