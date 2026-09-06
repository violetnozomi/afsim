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

#ifndef WSFXIO_EVENTPIPE_HPP
#define WSFXIO_EVENTPIPE_HPP

#include "GenUniqueId.hpp"
#include "WsfEventPipe.hpp"

class WsfXIO_HeartbeatPkt;
class WsfSimulation;

namespace wsf
{
namespace xio
{

class EventPipe : public WsfEventPipeLogger
{
public:
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);

   explicit EventPipe(WsfEventPipeInterface& aEventPipeInterface);

   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

private:
   double GetSimTime() const;

   void HeartbeatImpl(double aTimeStamp, GenUniqueId aApplicationId, bool aReceived);

   WsfSimulation&         mSimulation;
   WsfEventPipeInterface& mEventPipe;

   UtCallbackHolder mCallbacks;

   const int cXIO;
   bool      mXioEnabled;
};

} // namespace xio
} // namespace wsf

#endif
