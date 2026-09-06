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

#ifndef WSFCOMMXMTRRCVR_HPP
#define WSFCOMMXMTRRCVR_HPP

#include "WsfComm.hpp"

namespace wsf
{
namespace comm
{

class WSF_EXPORT XmtrRcvr : public Comm
{
public:
   XmtrRcvr(const WsfScenario& aScenario, CommType aCommType = cCT_XMT_RCV);

   ~XmtrRcvr() override = default;
   XmtrRcvr& operator=(const XmtrRcvr& aRhs) = delete;

   Comm* Clone() const override;

protected:
   XmtrRcvr(const XmtrRcvr& aSrc) = default;
};

} // namespace comm
} // namespace wsf

#endif
