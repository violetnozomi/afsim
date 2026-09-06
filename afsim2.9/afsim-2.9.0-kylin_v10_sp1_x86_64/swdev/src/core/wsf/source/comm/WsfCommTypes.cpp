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

#include "WsfCommTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

// Include files for built-in types
#include "WsfCommXmtrRcvr.hpp"
#include "WsfRadioXmtrRcvr.hpp"

namespace
{
//! Component factory to process platform input.
class CommTypesComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      wsf::comm::Types& types(wsf::comm::Types::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<wsf::comm::Comm>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      wsf::comm::Types& types(wsf::comm::Types::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<wsf::comm::Comm>());
   }
};
} // namespace

namespace wsf
{
namespace comm
{

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
Types& Types::Get(WsfScenario& aScenario)
{
   return aScenario.GetCommTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const Types& Types::Get(const WsfScenario& aScenario)
{
   return aScenario.GetCommTypes();
}

// =================================================================================================
Types::Types(WsfScenario& aScenario)
   : WsfObjectTypeList<Comm>(aScenario, cREDEFINITION_ALLOWED, "comm")
{
   aScenario.RegisterComponentFactory(ut::make_unique<CommTypesComponentFactory>());

   AddCoreType("WSF_COMM_RCVR", ut::make_unique<XmtrRcvr>(aScenario, Comm::cCT_RCV_ONLY));
   AddCoreType("WSF_COMM_XMTR", ut::make_unique<XmtrRcvr>(aScenario, Comm::cCT_XMT_ONLY));
   AddCoreType("WSF_COMM_XMTR_RCVR", ut::make_unique<XmtrRcvr>(aScenario));
   AddCoreType("WSF_COMM_TRANSCEIVER", ut::make_unique<XmtrRcvr>(aScenario)); // An alias for WSF_COMM_XMTR_RCVR

   AddCoreType("WSF_RADIO_RCVR", ut::make_unique<RadioXmtrRcvr>(aScenario, Comm::cCT_RCV_ONLY));
   AddCoreType("WSF_RADIO_XMTR", ut::make_unique<RadioXmtrRcvr>(aScenario, Comm::cCT_XMT_ONLY));
   AddCoreType("WSF_RADIO_XMTR_RCVR", ut::make_unique<RadioXmtrRcvr>(aScenario));
   AddCoreType("WSF_RADIO_TRANSCEIVER", ut::make_unique<RadioXmtrRcvr>(aScenario));
}

} // namespace comm
} // namespace wsf
