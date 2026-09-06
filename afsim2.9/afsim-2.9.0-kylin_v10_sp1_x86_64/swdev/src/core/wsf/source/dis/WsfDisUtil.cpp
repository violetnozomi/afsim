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

#include "dis/WsfDisUtil.hpp"

#include "DisClockTime.hpp"
#include "DisPdu.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"

bool WsfDisUtil::DebugEnabled(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->DebugEnabled();
}

WsfDisPlatform* WsfDisUtil::FindDisPlatform(WsfDisInterface* aInterfacePtr, const DisEntityId& aEntityId)
{
   return aInterfacePtr->FindDisPlatform(aEntityId);
}

WsfDisPlatform* WsfDisUtil::GetDisPlatform(WsfDisInterface* aInterfacePtr, size_t aPlatformIndex)
{
   return aInterfacePtr->GetDisPlatform(aPlatformIndex);
}

void WsfDisUtil::GetEntityIdFromIndex(WsfDisInterface* aInterfacePtr, size_t aPlatformIndex, DisEntityId& aEntityId)
{
   return aInterfacePtr->GetEntityIdFromIndex(aPlatformIndex, aEntityId);
}

WsfExtInterface* WsfDisUtil::GetExtInterface(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetExtInterface();
}

DisUint32 WsfDisUtil::GetNextRequestNumber(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetNextRequestNumber();
}

DisUint8 WsfDisUtil::GetReliabilityService(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetReliabilityService();
}

const WsfScenario& WsfDisUtil::GetScenario(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetScenario();
}

WsfSimulation& WsfDisUtil::GetSimulation(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetSimulation();
}

double WsfDisUtil::GetSimTime(WsfDisInterface* aInterfacePtr)
{
   return aInterfacePtr->GetSimulation().GetSimTime();
}

double WsfDisUtil::GetTimeFromPdu(WsfDisInterface* aInterfacePtr, DisPdu& aPdu)
{
   return aInterfacePtr->GetTimeFromPdu(aPdu);
}

void WsfDisUtil::PutPdu(WsfDisInterface* aInterfacePtr, double aSimTime, std::unique_ptr<DisPdu> aPduPtr)
{
   aInterfacePtr->PutPdu(aSimTime, std::move(aPduPtr));
}

void WsfDisUtil::SimTimeToDisClockTime(double aSimTime, DisClockTime& aDisClockTime)
{
   WsfDisInterface::SimTimeToDisClockTime(aSimTime, aDisClockTime);
}

// ============================================================================
// I needed this helper function to keep things straight:
// Return true = success.  Then false = PROBLEM.
bool WsfDisUtil::WsfToDisPlayers(WsfDisInterface* aInterfacePtr,
                                 size_t           aOriginatingIndex,
                                 size_t           aRequestingIndex,
                                 size_t           aRespondingIndex,
                                 bool&            aSenderIsRequestor,
                                 bool&            aSenderIsResponder,
                                 bool&            aRequestorIsLocal,
                                 bool&            aResponderIsLocal,
                                 WsfDisPlatform*& aRequesterPtr,
                                 WsfDisPlatform*& aResponderPtr,
                                 DisEntityId&     aOriginatingId,
                                 DisEntityId&     aRequestorId,
                                 DisEntityId&     aResponderId)
{
   // Two players have to exist.
   bool success = true;

   if (aRequestingIndex == 0 || aRespondingIndex == 0)
   {
      success = false;
   }

   aSenderIsRequestor = aRequestingIndex == aOriginatingIndex;
   aSenderIsResponder = aRespondingIndex == aOriginatingIndex;

   aRequesterPtr = aInterfacePtr->GetDisPlatform(aRequestingIndex);
   if (aRequesterPtr == nullptr)
   {
      success = false;
   }
   else
   {
      aRequestorIsLocal = !aRequesterPtr->GetPlatform()->IsExternallyControlled();
      aRequestorId      = aRequesterPtr->GetEntityId();
      if (aSenderIsRequestor)
      {
         aOriginatingId = aRequestorId;
      }
   }

   aResponderPtr = aInterfacePtr->GetDisPlatform(aRespondingIndex);
   if (aResponderPtr == nullptr)
   {
      success = false;
   }
   else
   {
      aResponderIsLocal = !aResponderPtr->GetPlatform()->IsExternallyControlled();
      aResponderId      = aResponderPtr->GetEntityId();
      if (aSenderIsResponder)
      {
         aOriginatingId = aResponderId;
      }
   }
   return success;
}

// ============================================================================
// I needed this helper function to keep things straight:
// Return true = success.  Then false = PROBLEM.
bool WsfDisUtil::DisToWsfPlayers(WsfDisInterface*   aInterfacePtr,
                                 const DisEntityId& aOriginatingId,
                                 const DisEntityId& aRequestorId,
                                 const DisEntityId& aResponderId,
                                 bool&              aSenderIsRequestor,
                                 bool&              aSenderIsResponder,
                                 bool&              aRequestorIsLocal,
                                 bool&              aResponderIsLocal,
                                 WsfDisPlatform*&   aRequesterPtr,
                                 WsfDisPlatform*&   aResponderPtr,
                                 size_t&            aRequestingIndex,
                                 size_t&            aRespondingIndex)
{
   // Two players have to exist.
   WsfDisPlatform* origDisPlatPtr = aInterfacePtr->FindDisPlatform(aOriginatingId);

   aRequesterPtr = aInterfacePtr->FindDisPlatform(aRequestorId);
   aResponderPtr = aInterfacePtr->FindDisPlatform(aResponderId);

   if (!((aRequesterPtr != nullptr) && (aResponderPtr != nullptr)))
   {
      return false;
   }

   aSenderIsRequestor = aRequesterPtr == origDisPlatPtr;
   aSenderIsResponder = aResponderPtr == origDisPlatPtr;

   aRequestorIsLocal = !aRequesterPtr->GetPlatform()->IsExternallyControlled();
   aResponderIsLocal = !aResponderPtr->GetPlatform()->IsExternallyControlled();

   aRequestingIndex = aRequesterPtr->GetPlatform()->GetIndex();
   aRespondingIndex = aResponderPtr->GetPlatform()->GetIndex();

   return true;
}
