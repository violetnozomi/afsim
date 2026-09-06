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

#ifndef WSFCYBEREVENTPIPE_HPP
#define WSFCYBEREVENTPIPE_HPP

#include "WsfEventPipe.hpp"
class WsfEventPipeInterface;

namespace wsf
{
namespace cyber
{

class Engagement;

class EventPipe : public WsfEventPipeLogger
{
public:
   static void RegisterEvents(WsfEventPipeExtension& aEventPipeExtension);
   static void RegisterMessages(UtPackSerializer* aSerializer);
   EventPipe(WsfEventPipeInterface* aEventPipeInterfacePtr);


   void UpdateSubscriptions(const WsfEventPipeOptions& aNewCombinedOptions,
                            const WsfEventPipeOptions& aPreviousCombinedOptions) override;

private:
   void CyberAttackInitiated(double aSimTime, const Engagement& aEngagement);
   void CyberAttackSucceeded(double aSimTime, const Engagement& aEngagement);
   void CyberAttackFailed(double aSimTime, const Engagement& aEngagement);
   void CyberAttackDetected(double aSimTime, const Engagement& aEngagement);
   void CyberAttackAttributed(double aSimTime, const Engagement& aEngagement);
   void CyberAttackRecovery(double aSimTime, const Engagement& aEngagement);
   void CyberScanInitiated(double aSimTime, const Engagement& aEngagement);
   void CyberScanSucceeded(double aSimTime, const Engagement& aEngagement);
   void CyberScanFailed(double aSimTime, const Engagement& aEngagement);
   void CyberScanDetected(double aSimTime, const Engagement& aEngagement);
   void CyberScanAttributed(double aSimTime, const Engagement& aEngagement);

   void Send(double aSimTime, const WsfPlatform& aPlatform, std::unique_ptr<WsfEventPipe::MsgBase> aMessagePtr)
   {
      mEventPipePtr->SendDependent(aSimTime, aPlatform, std::move(aMessagePtr));
   }

   WsfSimulation*         mSimulationPtr;
   WsfEventPipeInterface* mEventPipePtr;

   UtCallbackHolder mCyberCallback;

   int mCyberEventId;
};
} // namespace cyber
} // namespace wsf
#endif
