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

#ifndef WSFMTT_FUSION_HPP
#define WSFMTT_FUSION_HPP

class MTT;
#include "WsfDefaultFusion.hpp"
class WsfTrackManager;
class WsfMTT_Interface;

//! A fusion strategy incorporating the algorithms of the Suppressor 7 MTT
//! (Multi-Target Tracker).
//! This class inherits from default fusion because it uses the same non-kinematic rules.
class WsfMTT_Fusion : public WsfDefaultFusion
{
public:
   explicit WsfMTT_Fusion(const WsfScenario& aScenario);
   WsfMTT_Fusion(const WsfMTT_Fusion& aSrc);
   WsfMTT_Fusion& operator=(const WsfMTT_Fusion&) = delete;

   ~WsfMTT_Fusion() override;

   //! Create a clone of "this" object.
   WsfMTT_Fusion* Clone() const override { return new WsfMTT_Fusion(*this); }

   bool Initialize(WsfTrackManager* aTrackManagerPtr) override;

   bool UpdateLocalTrackFromNonLocalTrack(double aSimTime, WsfLocalTrack& aLocalTrack, const WsfTrack& aNonLocalTrack) override;

protected:
private:
   WsfMTT_Interface* mMTT_InterfacePtr;
   MTT*              mMTT_Ptr;
   bool              mInitialized;

   void InitializeP();
};

#endif
