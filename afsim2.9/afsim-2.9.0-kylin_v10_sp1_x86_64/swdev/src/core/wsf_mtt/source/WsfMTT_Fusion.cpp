// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMTT_Fusion.hpp"

#include "MTT.hpp"
#include "MTT_PerceivedCluster.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMTT_Correlation.hpp"
#include "WsfMTT_Interface.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackManager.hpp"

WsfMTT_Fusion::WsfMTT_Fusion(const WsfScenario& aScenario)
   : WsfDefaultFusion(aScenario)
   , mMTT_InterfacePtr(nullptr)
   , mMTT_Ptr(nullptr)
   , mInitialized(false)
{
}

WsfMTT_Fusion::WsfMTT_Fusion(const WsfMTT_Fusion& aSrc)
   : WsfDefaultFusion(aSrc)
   , mMTT_InterfacePtr(nullptr)
   , mMTT_Ptr(nullptr)
   , // must get this from MTT_Interface.
   mInitialized(false)
{
}

// virtual
WsfMTT_Fusion::~WsfMTT_Fusion() {}

//! Fuse a non-local track with an existing local track.
//! @returns true if the local track was updated with non-local track data or false if the
//!          non-local track data was out-of-date.
// virtual
bool WsfMTT_Fusion::UpdateLocalTrackFromNonLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack)
{
   bool updated = false;
   if (!mInitialized)
   {
      InitializeP();
   }

   // Do we have saved data?
   int activeTrackId =
      static_cast<WsfMTT_Correlation&>(GetTrackManager().GetCorrelationStrategy()).GetActiveTrackId(aLocalTrack.GetTrackId());
   if (activeTrackId != -1)
   {
      MTT_PerceivedCluster*  nonLocalTrackPtr;
      MTT_ActiveTrack::Data* tempDataPtr;
      MTT_ActiveTrack*       localTrackPtr;
      mMTT_Ptr->RemoveSavedData(activeTrackId, nonLocalTrackPtr, tempDataPtr, localTrackPtr);
      mMTT_Ptr->MTT_Fuse(aSimTime, *nonLocalTrackPtr, *localTrackPtr, *tempDataPtr);

      // Unpack active track to local track
      localTrackPtr->CopyTo(aSimTime, mMTT_InterfacePtr->GetReferencePoint(), aLocalTrack);

      // If this is a newly created track, Notify observers now
      if (aLocalTrack.GetAuxDataConst().GetBool("IsNewActiveTrack"))
      {
         aLocalTrack.GetAuxData().Assign("IsNewActiveTrack", false);
         GetTrackManager().NotifyOfLocalTrackInitiated(aSimTime, &aLocalTrack, &aNonLocalTrack);
      }

      // Need to update track quality, as it is not handled in the MTT:
      if (aNonLocalTrack.GetTrackQuality() > aLocalTrack.GetTrackQuality())
      {
         aLocalTrack.SetTrackQuality(aNonLocalTrack.GetTrackQuality());
         if (GetTrackManager().DebugEnabled())
         {
            auto logger = ut::log::debug() << "Track quality updated.";
            logger.AddNote() << "T = " << aSimTime;
            logger.AddNote() << "Platform: " << GetTrackManager().GetPlatform()->GetName();
            logger.AddNote() << "LT: " << aLocalTrack.GetTrackId();
            logger.AddNote() << "Set Track Quality: " << aLocalTrack.GetTrackQuality();
         }
      }

      aLocalTrack.Update(aSimTime);

      // done with the non-const saved data, so delete.
      delete tempDataPtr;

      updated = true;
   }
   else // No existing active track id.
   {
      aLocalTrack.UpdateFromMeasurement(aSimTime, aNonLocalTrack);
   }

   return updated;
}

void WsfMTT_Fusion::InitializeP()
{
   mMTT_Ptr = mMTT_InterfacePtr->GetMTT(GetTrackManager().GetPlatform()->GetNameId());
   if (mMTT_Ptr == nullptr)
   {
      std::string msg = GetTrackManager().GetPlatform()->GetName() +
                        ": WsfMTT_Fusion: Unable to initialize (did you set the correlation_type to 'MTT' ?";
      throw UtException(msg);
   }
   mInitialized = true;
}

bool WsfMTT_Fusion::Initialize(WsfTrackManager* aTrackManagerPtr)
{
   if (aTrackManagerPtr != nullptr)
   {
      mMTT_InterfacePtr = static_cast<WsfMTT_Interface*>(aTrackManagerPtr->GetSimulation()->FindExtension("wsf_mtt"));
   }
   return WsfDefaultFusion::Initialize(aTrackManagerPtr);
}
