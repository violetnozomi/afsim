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

#include "dis/WsfDisLaserDesignations.hpp"

#include <cassert>

#include "UtCallbackHolder.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfDisObserver.hpp"
#include "WsfMil.hpp"
#include "WsfMilDisInterface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "dis/WsfDisDesignator.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"

// ============================================================================
WsfDisLaserDesignations::WsfDisLaserDesignations(WsfMilDisInterface* aInterfacePtr)
   : mDebug(false)
   , mSendCount(0)
   , mReceiveCount(0)
   , mCallbacks()
   , mItarPtr(aInterfacePtr)
   , mDisInterfacePtr(mItarPtr->GetDis())
   , mSimulation(WsfDisUtil::GetSimulation(mDisInterfacePtr))
{
   // ITAR - below needs fixed
   mLaserDesignationsPtr = static_cast<WsfLaserDesignations*>(&mSimulation.GetExtension("laser_designations"));

   mCallbacks.Add(WsfObserver::Initialize(&mSimulation).Connect(&WsfDisLaserDesignations::Initialize, this));
}

// ============================================================================
//! Function to Initialize the laser designations.  (Function is internally
//! callback-queued, so does not need explicitly called by the user.)
//! @returns true if successful.
bool WsfDisLaserDesignations::Initialize()
{
   mCallbacks.Add(WsfObserver::DisDesignatorReceived(&mSimulation).Connect(&WsfDisLaserDesignations::ProcessPDU, this));
   mCallbacks.Add(mLaserDesignationsPtr->SpotUpdated.Connect(&WsfDisLaserDesignations::SpotUpdated, this));
   return true;
}

// ============================================================================
bool WsfDisLaserDesignations::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "dis_laser_designations")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand())
      {
         UtInput& input = block.GetInput();
         if (!ProcessInput(input))
         {
            throw UtInput::UnknownCommand(input);
         }
      }
   }
   else if (command == "debug_dis_laser_designations")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ========================================================================
void WsfDisLaserDesignations::ProcessPDU(WsfDisInterface*, const WsfDisDesignator& aPdu)
{
   // This simulation just received an external Designation PDU.
   // Recreate or update the spot locally in WSF.

   if (mDisInterfacePtr == nullptr)
   {
      return;
   }

   WsfDisPlatform* disDesignatingPlatPtr = WsfDisUtil::FindDisPlatform(mDisInterfacePtr, aPdu.GetDesignatingEntity());
   WsfDisPlatform* disDesignatedPlatPtr  = WsfDisUtil::FindDisPlatform(mDisInterfacePtr, aPdu.GetDesignatedEntity());

   if ((disDesignatingPlatPtr != nullptr) && (disDesignatedPlatPtr != nullptr))
   {
      WsfPlatform* designatingPlatformPtr = disDesignatingPlatPtr->GetPlatform();
      WsfPlatform* designatedPlatformPtr  = disDesignatedPlatPtr->GetPlatform();

      if ((designatedPlatformPtr != nullptr) && (designatingPlatformPtr != nullptr))
      {
         size_t designatedIndex  = designatedPlatformPtr->GetIndex();
         size_t designatingIndex = designatingPlatformPtr->GetIndex();

         // Create a blank laser spot, then populate all attributes into it from the PDU.
         WsfLaserDesignations::Spot spot;
         SetSpotFromPduP(spot, designatingIndex, designatedIndex, aPdu);

         // Let the WSF repository worry about whether this is a new or updated laser spot.
         double emitterLocWCS[3];
         designatingPlatformPtr->GetLocationWCS(emitterLocWCS);
         mLaserDesignationsPtr->AddOrUpdate(spot, emitterLocWCS);

         ++mReceiveCount;

         if (mDebug && mReceiveCount % 367 == 0)
         {
            ut::log::debug() << "WsfDisLaserDesignations receiving Designator PDUs. Forwarding to WSF.";
         }
      }
   }
}

// ========================================================================
void WsfDisLaserDesignations::SpotUpdated(const WsfLaserDesignations::Spot& aSpot)
{
   // This simulation just received notification of an WSF laser spot update.
   // Send the spot update externally in a PDU, unless it actually originated externally.
   if (aSpot.IsExternal())
   {
      return;
   }

   if (mDisInterfacePtr == nullptr)
   {
      return;
   }

   WsfDisPlatform* designatingPlatPtr = WsfDisUtil::GetDisPlatform(mDisInterfacePtr, aSpot.GetDesignatingIndex());
   WsfDisPlatform* designatedPlatPtr  = WsfDisUtil::GetDisPlatform(mDisInterfacePtr, aSpot.GetDesignatedIndex());

   DisEntityId designatingId(DisEntityId::ENTITY_ID_UNKNOWN);
   if (designatingPlatPtr != nullptr)
   {
      designatingId = designatingPlatPtr->GetEntityId();
   }

   DisEntityId designatedId(DisEntityId::D_SPOT_NO_ENTITY);
   if (designatedPlatPtr != nullptr)
   {
      designatedId = designatedPlatPtr->GetEntityId();
   }

   // Create a blank designation PDU, then populate all attributes into it from the spot.
   auto pduPtr = ut::make_unique<DisDesignator>();
   SetPduFromSpotP(aSpot, designatingId, designatedId, *pduPtr);

   double simTime = WsfDisUtil::GetSimTime(mDisInterfacePtr);
   WsfDisUtil::PutPdu(mDisInterfacePtr, simTime, std::move(pduPtr));

   ++mSendCount;

   if (mDebug && mSendCount % 367 == 0)
   {
      ut::log::debug() << "WsfDisLaserDesignations receiving WSF Spots. Forwarding to DIS.";
   }
}

// =======================================================================
// private
void WsfDisLaserDesignations::SetSpotFromPduP(WsfLaserDesignations::Spot& aSpot,
                                              size_t                      aDesignatingIndex,
                                              size_t                      aDesignatedIndex,
                                              const WsfDisDesignator&     aDesignation)
{
   // Fill in the data from a PDU into a local Spot object.

   // --- Start with 3-D vector translation as needed from float to double ---
   float accelF[3];
   aDesignation.GetAcceleration(accelF[0], accelF[1], accelF[2]);
   double accelD[3];
   accelD[0] = (double)accelF[0];
   accelD[1] = (double)accelF[1];
   accelD[2] = (double)accelF[2];

   float offsetF[3];
   aDesignation.GetOffset(offsetF[0], offsetF[1], offsetF[2]);
   double offsetD[3];
   offsetD[0] = (double)offsetF[0];
   offsetD[1] = (double)offsetF[1];
   offsetD[2] = (double)offsetF[2];

   // These are both doubles already...
   double locationWCS[3];
   aDesignation.GetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);

   // Set the vector attributes:
   aSpot.SetAcceleration(accelD);
   aSpot.SetOffsetECS(offsetD);
   aSpot.SetLocationWCS(locationWCS);

   // Set the unitary attributes
   aSpot.SetCode(aDesignation.GetCode());
   aSpot.SetCodeName(aDesignation.GetCodeName());
   aSpot.SetDesignatedIndex(mSimulation, aDesignatedIndex);
   aSpot.SetDesignatingIndex(aDesignatingIndex);
   aSpot.SetPower((double)aDesignation.GetPower());
   aSpot.SetWavelength((double)aDesignation.GetWavelength()); // Also infers Frequency.

   aSpot.SetIsExternal(true); // since it comes from a PDU...

   // aSpot.SetSimTimeStamp(0.0); // The WSF receiver should time-tag this at time of receipt!
}

// =======================================================================
// private
void WsfDisLaserDesignations::SetPduFromSpotP(const WsfLaserDesignations::Spot& aSpot,
                                              const DisEntityId&                aDesignatingId,
                                              const DisEntityId&                aDesignatedId,
                                              DisDesignator&                    aDesignation)
{
   // Fill in the data from a Spot into a PDU object.

   // Start with translating the vectors.
   double accelD[3];
   aSpot.GetAcceleration(accelD);

   float accelF[3];
   accelF[0] = (float)accelD[0];
   accelF[1] = (float)accelD[1];
   accelF[2] = (float)accelD[2];

   double offsetD[3];
   aSpot.GetOffsetECS(offsetD);

   float offsetF[3];
   offsetF[0] = (float)offsetD[0];
   offsetF[1] = (float)offsetD[1];
   offsetF[2] = (float)offsetD[2];

   double locationWCS[3];
   aSpot.GetLocationWCS(locationWCS);

   // Now set the vector attributes.
   aDesignation.SetAcceleration(accelF[0], accelF[1], accelF[2]);
   aDesignation.SetLocation(locationWCS[0], locationWCS[1], locationWCS[2]);
   aDesignation.SetOffset(offsetF[0], offsetF[1], offsetF[2]);

   // Now set the unitary attributes.
   aDesignation.SetCode(aSpot.GetCode());
   aDesignation.SetCodeName(aSpot.GetCodeName());

   aDesignation.SetDesignatedEntity(aDesignatedId);
   aDesignation.SetDesignatingEntity(aDesignatingId);

   aDesignation.SetPower((float)aSpot.GetPower());
   aDesignation.SetWavelength((float)aSpot.GetWavelength()); // Also infers Frequency.
}
