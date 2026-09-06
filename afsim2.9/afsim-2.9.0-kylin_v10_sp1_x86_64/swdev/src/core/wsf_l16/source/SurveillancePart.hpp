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
#ifndef L16_SURVEILLANCEPART_HPP
#define L16_SURVEILLANCEPART_HPP

#include <set>

#include "ComputerPart.hpp"
#include "InputEnumeration.hpp"
#include "UtScriptData.hpp"
class WsfLocalTrack;
class WsfTrack;
#include "WsfTrackId.hpp"
class WsfTrackManager;
#include "wsf_l16_export.h"

#include "WsfTypes.hpp"

namespace WsfL16
{
class Interface;

//! SurveillancePart is responsible for sending tracks.
class WSF_L16_EXPORT SurveillancePart : public ComputerPart
{
public:
   SurveillancePart(WsfScenario& aScenario);
   SurveillancePart(const SurveillancePart& aSrc);
   ~SurveillancePart() override = default;

   ComputerPart* Clone() const override;

   bool Initialize(double aSimTime, ComputerProcessor& aComputer) override;

   bool ProcessInput(UtInput& aInput) override;

   //! Handle an incoming message
   void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) override;

   //! This method is called when the a slot is open
   void HandleSend(double aSimTime) override;

   //! Suppresses or allows reporting of sensor tracks from the given sensor name.
   //! Returns whether the sensor was previously being suppressed.
   bool SuppressSensor(WsfStringId aSensorName, bool aSuppress);

   // GetTrackNumber called by an external method.
   int GetTrackNumber(WsfLocalTrack& aLocalTrack);

   // SendAirTrack called by an external method.
   void SendAirTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber);

   WsfSimulation* GetSimulation() const;

protected:
   bool HaveReportingResponsibility(double               aSimTime,
                                    const WsfTrack*      aTrackPtr,
                                    const WsfLocalTrack* aLocalTrackPtr,
                                    int                  aLink16_TrackQuality, // for aTrackPtr
                                    std::string&         aTrackNumberString,
                                    bool&                aIsR2_Switch);

   bool OwningPlatformWantsTrackRefNumber(const WsfTrack& aTrack);

   enum TrackEnvironment
   {
      cAIR,
      cSURFACE,
      cSUBSURFACE,
      cLAND
   };

   void TrackUpdated(const WsfTrack& aTrack);

   void TrackCreated(const WsfTrack& aTrack);

   void TrackDropped(const WsfTrack& aTrack);

   void RawTrackReceived(const WsfTrack& aTrack);

   void RawTrackDropped(const WsfTrack& aTrack);

   void SendLandTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber);

   void SendSubsurfaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber);
   void SendSurfaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber);

   void SendSpaceTrack(double aSimTime, const WsfTrack& aTrack, int aTrackNumber);

   bool IsTrackReportable(const WsfTrack& aTrack);

   bool IsSensorTrackSuppressed(const WsfTrack& aTrack);
   struct TrackInfo;

   using TrackMap  = std::map<WsfTrackId, TrackInfo>;
   using TrackList = std::list<TrackMap::iterator>;

   struct TrackInfo
   {
      double              mLastSendTime;
      TrackList::iterator mIterator;
      bool                mIsUpdated;
   };

   TrackMap::iterator FindTrackToSend();

   bool TrackAvailableToSend(double aSimTime);

   // Map containing all of the current tracks
   TrackMap mTracks;

   // Track lists:
   // Tracks get pushed to back, and pulled from front

   //   A list of unmodified tracks.
   TrackList mUnchangedTracks;

   //   A list of tracks that have been modified
   TrackList             mUpdatedTracks;
   double                mMaxSendInterval{6.0};
   bool                  mSendNonSensorReports{true};
   bool                  mSuppressDeadTargets{true};
   bool                  mIgnoreReportingResponsibility{false};
   WsfTrackManager*      mTrackManager{nullptr};
   std::pair<int, int>   mTrackNumberRange{0, 0};
   int                   mNextTrackNumber;
   int                   mDefaultTrackDomain{WSF_SPATIAL_DOMAIN_AIR};
   std::set<int>         mSuppressDomains;
   std::set<WsfStringId> mSuppressSensors;
   InputEnumeration      mSpatialDomainInput;
   UtScriptData*         mTrackVariablePtr;
   Interface*            mInterfacePtr{nullptr};
};
} // namespace WsfL16

#endif
