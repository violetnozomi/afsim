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

#ifndef WSF_MTT_CORRELATION_HPP
#define WSF_MTT_CORRELATION_HPP

#include <map>
#include <memory>

class MTT;
#include "MTT_PerceivedCluster.hpp"
#include "WsfCorrelationStrategy.hpp"
class WsfMTT_Interface;
#include "UtCallbackHolder.hpp"
class UtInput;

//! An implementation of WsfCorrelationStrategy implementing the correlation algorithms of the
//! Suppressor 7 MTT (Multi-Target tracker).  This is a nearest-neighbor strategy.
class WsfMTT_Correlation : public WsfCorrelationStrategy
{
public:
   WsfMTT_Correlation();
   WsfMTT_Correlation(const WsfMTT_Correlation& aSrc);
   WsfMTT_Correlation& operator=(const WsfMTT_Correlation&) = delete;
   ~WsfMTT_Correlation() override                           = default;

   WsfCorrelationStrategy* Clone() const override { return new WsfMTT_Correlation(*this); }

   bool ProcessInput(UtInput& aInput) override; // for general configuration

   bool Initialize(WsfTrackManager* aTrackManagerPtr) override;

   int GetActiveTrackId(const WsfTrackId& aLocalTrackId) const;

   void DecorrelateNonlocalTrack(const WsfTrack& aNonLocalTrack);

   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;

private:
   void LocalTrackCorrelated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aRawTrackId);
   void LocalTrackDecorrelated(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aRawTrackId);
   void LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);
   void RawTrackDropped(double aSimTime, const WsfTrack* aRawTrackPtr);

   WsfMTT_Interface*    mMTT_InterfacePtr;
   std::shared_ptr<MTT> mMTT_Ptr;

   std::map<WsfTrackId, int>                            mNLT_IdToAT_IdMap;
   std::map<WsfTrackId, int>                            mLT_IdToAT_IdMap;
   std::map<int, std::unique_ptr<MTT_PerceivedCluster>> mPerceivedClusterMap;

   UtCallbackHolder mCallbacks;
};

#endif
