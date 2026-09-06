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
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef WSFWEAPONTRACKPROCESSOR_HPP
#define WSFWEAPONTRACKPROCESSOR_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

class WsfLocalTrack;
#include "WsfProcessor.hpp"
class WsfScriptContext;
class WsfScriptMessageHandler;
class WsfSensor;
#include "WsfStringId.hpp"
class WsfTrack;
class WsfTrackId;

//! A processor that is responsible for maintaining the current perception of the target.
//!
//! This processor is designed to be used on weapons (like missiles or smart bombs)
//! to maintain the 'Current Target' track object on the platform to which the
//! processor is attached (The 'Current Target' is used by WsfGuidanceProcessor
//! to determine the guidance commands which are then fed to the mover or by some
//! missile movers which have their own embedded guidance computer).
//!
//! The processor accepts track input from multiple source and forms the current
//! target track.  For command guidance this is simply taking a track from datalink
//! and making it the current target.  For active guidance this is simply taking
//! the track from the seeker and making it the current target.  For track-via-missile
//! this may mean taking a combination of the two in order to make the current target.

class WSF_MIL_EXPORT WsfWeaponTrackProcessor : public WsfProcessor
{
public:
   enum EngagementPhase
   {
      cMID_COURSE,
      cTERMINAL
   };

   WsfWeaponTrackProcessor(WsfScenario& aScenario);
   ~WsfWeaponTrackProcessor() override;

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   void          Update(double aSimTime) override;

   EngagementPhase GetEngagementPhase() const { return mEngagementPhase; }

   void SetPlatform(WsfPlatform* aPlatformPtr) override;

   void SetCurrentTarget(double aSimTime, WsfTrack* aTrackPtr);

   struct Sensor
   {
      WsfStringId mSensorName;
      WsfSensor*  mSensorPtr;
      //! If less    than zero, the turn-on time is time-since-launch.
      //! If greater than zero, the turn-on time is time-until-intercept.
      double mTurnOnTime;
      double mTurnOnRange;
      bool   mUseTargetLocation;
   };

protected:
   WsfWeaponTrackProcessor(const WsfWeaponTrackProcessor& aSrc);
   WsfWeaponTrackProcessor& operator=(const WsfWeaponTrackProcessor&) = delete;

   static bool FindIntercept(const double aWpnLocWCS[3],
                             double       aWpnSpeed,
                             const double aTgtLocWCS[3],
                             const double aTgtVelWCS[3],
                             double       aIntLocWCS[3]);

   virtual void GetInterceptData(double aSimTime, double& aTimeToGo, double& aRangeToGo, double& aRangeToTgt);

   virtual void ProcessTrackDrop(double aSimTime, const WsfTrackId& aTrackId);

   virtual void ProcessTrackUpdate(double aSimTime, const WsfTrack& aTrack);

   virtual void UpdateTargetTrack(double aSimTime, bool aAdjustUpdateInterval);

private:
   size_t GetCurrentTargetIndex();

   std::unique_ptr<WsfScriptContext>        mContextPtr;
   std::unique_ptr<WsfScriptMessageHandler> mMessageHandlerPtr;

   std::vector<Sensor> mSensorList;
   EngagementPhase     mEngagementPhase;

   //! True if tracks unrelated to the current target are ignored
   bool mIgnoreUplinkTargetChange;

   //! True if uplink is always required.
   bool mUplinkRequired;

   //! True if intercept calculation is required
   bool mRequireInterceptCalculation;

   //! The maximum amount of time that may elapse without updating the
   //! current target track.
   double mCoastTime;

   //! The projected Time-To-Go at which the switch-over from command
   //! guidance to combined or self-guidance should occur.
   double mSwitchTime;

   //! The projected Range-To-Go at which the switch-over from command
   //! guidance to combined or self-guidance should occur.
   double mSwitchRange;

   WsfTrack* mCmdTrackPtr;
   WsfTrack* mSnrTrackPtr;

   double mTimeToGo;  //!< Time to go to intercept location
   double mRangeToGo; //!< Range to go to intercept location

   double mRangeToTgt; //!< Range to current target location wrt to current weapon location

   //! The time when we last received valid data from either a sensor or a data link.
   double mLastCmdTrackUpdateTime;
   double mLastSnrTrackUpdateTime;
};

#endif
