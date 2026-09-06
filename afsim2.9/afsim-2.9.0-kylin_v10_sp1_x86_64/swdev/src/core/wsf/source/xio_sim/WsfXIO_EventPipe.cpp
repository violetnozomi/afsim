// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfXIO_EventPipe.hpp"

#include "WsfXIO_Extension.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_PacketRegistry.hpp"

// Note: There is an include order dependency with these generated headers.
#include "UtPackReflect.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfEventPipeSchema.hpp"
#include "WsfSimulation.hpp"

namespace WsfEventPipe
{
void Register_MsgXioHeartbeat(UtPackReflector& aReflector);
}

// static
void wsf::xio::EventPipe::RegisterEvents(WsfEventPipeExtension& aEventPipeExtension)
{
   aEventPipeExtension.RegisterEventOption("XIO", true);
}

wsf::xio::EventPipe::EventPipe(WsfEventPipeInterface& aEventPipeInterface)
   : mSimulation(aEventPipeInterface.GetSimulation())
   , mEventPipe(aEventPipeInterface)
   , cXIO(aEventPipeInterface.GetEventId("XIO"))
   , mXioEnabled(false)
{
   WsfXIO_Interface* xioInterface = WsfXIO_Extension::Find(mSimulation);
   if (xioInterface != nullptr)
   {
      mCallbacks += xioInterface->OnHeartbeatUpdate.Connect(&EventPipe::HeartbeatImpl, this);
   }
}

void wsf::xio::EventPipe::UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                                              const WsfEventPipeOptions& aPreviousCombinedOptions)
{
   mXioEnabled = aNewCombinedOptions.IsOptionEnabled(cXIO);
}

double wsf::xio::EventPipe::GetSimTime() const
{
   return mSimulation.GetSimTime();
}

void wsf::xio::EventPipe::HeartbeatImpl(double aTimeStamp, GenUniqueId aApplicationId, bool aReceived)
{
   if (!mEventPipe.GetOutputFileName().empty() && mXioEnabled)
   {
      auto msgPtr = ut::make_unique<WsfEventPipe::MsgXioHeartbeat>();
      msgPtr->timeStamp(static_cast<float>(aTimeStamp));
      msgPtr->applicationId().a(aApplicationId.GetData(0));
      msgPtr->applicationId().b(aApplicationId.GetData(1));
      msgPtr->applicationId().c(aApplicationId.GetData(2));
      mEventPipe.SendImmediate(GetSimTime(), aReceived, std::move(msgPtr));
   }
}
