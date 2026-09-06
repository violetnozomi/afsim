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
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#ifndef WSFMTT_INTERFACE_HPP
#define WSFMTT_INTERFACE_HPP

#include "wsf_mtt_export.h"

#include <map>
#include <string>

class MTT;
class MTT_ActiveTrack;
class MTT_CandidateTrack;
class MTT_EmbryonicTrack;
#include "MTT_Parameters.hpp"
#include "UtCallback.hpp"
#include "UtCallbackHolder.hpp"
class UtInput;

#include "WsfMTT_ReferencePoint.hpp"
class WsfPlatform;
class WsfTrack;
#include "WsfSimulationExtension.hpp"
#include "WsfTrackId.hpp"
class WsfTrackManager;
class WsfEventOutput;

class WSF_MTT_EXPORT WsfMTT_InterfaceData
{
public:
   WsfMTT_InterfaceData(WsfScenario& aScenario);
   virtual ~WsfMTT_InterfaceData() = default;

   bool ProcessInput(UtInput& aInput);

   WsfScenario*          mScenarioPtr;
   bool                  mUserReferencePointDefined;
   bool                  mReferencePointDefined;
   WsfMTT_ReferencePoint mReferencePoint;
};

class WSF_MTT_EXPORT WsfMTT_Interface : public WsfSimulationExtension, public WsfMTT_InterfaceData
{
public:
   WsfMTT_Interface(const WsfMTT_InterfaceData& aData);

   ~WsfMTT_Interface() override;

   void AddedToSimulation() override;

   static WsfMTT_Interface* Find(const WsfSimulation& aSimulation);

   bool Initialize() override;

   MTT* GetMTT(WsfStringId aPlatformId); // For use at runtime.

   WsfMTT_ReferencePoint& GetReferencePoint() { return mReferencePoint; }

   void Register(std::shared_ptr<MTT> aMTT_Ptr, WsfPlatform* aPlatformPtr);

private:
   UtCallbackHolder mCallbacks;

   // Instance maintenance
   using PlatformToMTT_Map = std::map<WsfStringId, std::shared_ptr<MTT>>;
   struct Trackers
   {
   public:
      Trackers()  = default;
      ~Trackers() = default;
      PlatformToMTT_Map mPlatformToMTT_Map;
   };
   Trackers mTrackers;
};

#endif
