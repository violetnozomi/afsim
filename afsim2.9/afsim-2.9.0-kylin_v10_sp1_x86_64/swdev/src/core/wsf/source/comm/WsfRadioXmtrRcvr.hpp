// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFRADIOXMTRRCVR_HPP
#define WSFRADIOXMTRRCVR_HPP

#include "WsfComm.hpp"

namespace wsf
{
namespace comm
{

class WSF_EXPORT RadioXmtrRcvr : public Comm
{
public:
   RadioXmtrRcvr(WsfScenario& aScenario, CommType aCommType = cCT_XMT_RCV);
   ~RadioXmtrRcvr() override = default;

   //! @name Common infrastructure methods.
   //@{
   Comm* Clone() const override;
   //@}

   //! @name Send/Receive methods
   //@{
   bool ReceiveSetup(double aSimTime, Comm* aXmtrPtr, Message& aMessage) override;
   //@}

   bool CanSendTo(double aSimTime, Comm* aRcvrPtr, Message* aMessagePtr = nullptr) override;

protected:
   //! Copy Constructor
   RadioXmtrRcvr(const RadioXmtrRcvr& aSrc) = default;

private:
   //! Prevent use of operator= by declaring, but not defining.
   RadioXmtrRcvr& operator=(const RadioXmtrRcvr& aRhs) = delete;
};

} // namespace comm
} // namespace wsf

#endif
