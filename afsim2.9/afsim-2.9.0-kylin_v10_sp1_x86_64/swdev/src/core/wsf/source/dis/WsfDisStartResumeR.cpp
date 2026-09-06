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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "dis/WsfDisStartResumeR.hpp"

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisControlEnums.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisAcknowledgeR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisStartResumeR::WsfDisStartResumeR(WsfDisInterface* aInterfacePtr)
   : DisStartResumeR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisStartResumeR::WsfDisStartResumeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisStartResumeR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisStartResumeR::~WsfDisStartResumeR() {}

DisPdu* WsfDisStartResumeR::ProduceResponse(bool aIsDone)
{
   WsfDisAcknowledgeR* resultPtr = new WsfDisAcknowledgeR(mInterfacePtr);
   resultPtr->SetReceivingEntity(GetOriginatingEntity());
   resultPtr->SetRequestId(GetRequestId());
   resultPtr->SetAcknowledgeFlag(DisEnum::Control::ResponseToCmdFlag::START_RESUME);
   DisEnum16 compliance = DisEnum::Control::ResponseCompliance::UNABLE_TO_COMPLY;
   if (aIsDone)
   {
      compliance = DisEnum::Control::ResponseCompliance::ABLE_TO_COMPLY;
   }
   resultPtr->SetResponseFlag(compliance);
   return resultPtr;
}

bool WsfDisStartResumeR::ProcessStartCommand()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      auto out = ut::log::debug() << "WsfDisStartResumeR processed, and local Resume command will issued.";

      DisClockTime realWorldTime  = GetRealWorldTime();
      DisClockTime simulationTime = GetSimulationTime();

      if ((realWorldTime.GetHour() != 0 || realWorldTime.GetTimePastHour() != 0) ||
          (simulationTime.GetHour() != 0 || simulationTime.GetTimePastHour() != 0))
      {
         double secPastHour = DisTime::GetAbsoluteTime(realWorldTime.GetTimePastHour());
         int    minutes     = static_cast<int>(secPastHour / 60.0);
         double seconds     = secPastHour - (minutes * 60.0);

         out.AddNote() << "WsfDisStartResumeR does not yet use parameters for anything:";
         out.AddNote() << "Real World Time: " << static_cast<int>(realWorldTime.GetHour()) << ':' << minutes << ':'
                       << seconds;
         secPastHour = DisTime::GetAbsoluteTime(simulationTime.GetTimePastHour());
         minutes     = static_cast<int>(secPastHour / 60.0);
         seconds     = secPastHour - (minutes * 60.0);
         out.AddNote() << "Simulation Time: " << static_cast<int>(simulationTime.GetHour()) << ':' << minutes << ':'
                       << seconds;
      }
      else
      {
         out.AddNote() << "WsfDisStartResumeR did not contain any simulation time parameters.";
      }
   }
   WsfDisUtil::GetSimulation(mInterfacePtr).Resume();

   return true;
}

bool WsfDisStartResumeR::ActionIsForMe() const
{
   // See if I should process this PDU.

   const DisEntityId& destId = GetReceivingEntity();
   DisUint16          site   = destId.GetSite();
   DisUint16          appl   = destId.GetApplication();
   DisUint16          entity = destId.GetEntity();

   if ((site == Dis::GetSite()) && (appl == Dis::GetApplication()) &&
       (entity == 0)) // The System Manager is assumed (?) to have entityId zero.
   {
      return true;
   }

   // The check for 0xFF is due to a mis-interpretation by Seattle.
   // These are supposed to be 16 bit fields, not 8 bit values.

   if (((site == DisEntityId::NO_SITES) || (site == DisEntityId::ALL_SITES) || (site == 0xFF) || (site == Dis::GetSite())) &&
       ((appl == DisEntityId::NO_APPLIC) || (appl == DisEntityId::ALL_APPLIC) || (appl == 0xFF) ||
        (appl == Dis::GetApplication())))
   {
      // For now we only process requests for non-specific entities

      if ((entity == DisEntityId::NO_ENTITY) || (entity == DisEntityId::ALL_ENTITIES) || (entity == 0xFF))
      {
         return true;
      }
   }

   return false;
}

// virtual
void WsfDisStartResumeR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisStartResumeR: Unhandled virtual TimedOut() call.";
   }
}
