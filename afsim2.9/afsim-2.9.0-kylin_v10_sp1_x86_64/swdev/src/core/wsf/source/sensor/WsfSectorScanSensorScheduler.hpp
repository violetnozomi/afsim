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

#ifndef WSFSECTORSCANSENSORSCHEDULER_HPP
#define WSFSECTORSCANSENSORSCHEDULER_HPP

#include "wsf_export.h"

#include <list>
#include <queue>
#include <set>

class UtInput;
#include "WsfSensorScheduler.hpp"
#include "WsfTrack.hpp"

//! The Sector Scan Sensor Scheduler provides a sensor scanning function that is more
//! temporally correct for choosing detection targets than the default scheduler.  It also allows for
//! definition of scan "sectors" in azimuth and elevation, such as for multi-bar radar scans or as a
//! scan pattern for an optical sensor.  The sensor is cued to the instantaneous scan location,
//! so script and visualizations have the correct pointing location.
class WSF_EXPORT WsfSectorScanSensorScheduler : public WsfSensorScheduler
{
public:
   WsfSectorScanSensorScheduler();
   ~WsfSectorScanSensorScheduler() override = default;

   static std::unique_ptr<WsfSensorScheduler> ObjectFactory(const std::string& aTypeName);

   WsfSensorScheduler* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime, WsfSensor* aSensorPtr, WsfSensorTracker* aTrackerPtr) override;

   void TransitionSector(double aSimTime);

   void ModeDeselected(double aSimTime, WsfStringId aModeNameId) override;
   void ModeSelected(double aSimTime, WsfStringId aModeNameId) override;

   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr) override;

   void RemoveTarget(double aSimTime, size_t aTargetIndex) override;

   bool SelectTarget(double               aSimTime,
                     double&              aNextSimTime,
                     size_t&              aTargetIndex,
                     WsfTrackId&          aRequestId,
                     WsfSensor::Settings& aSettings) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

private:
   WsfSectorScanSensorScheduler(const WsfSectorScanSensorScheduler& aSrc);
   WsfSectorScanSensorScheduler& operator=(const WsfSectorScanSensorScheduler&) = delete;

   void AdvanceScan(double aSimTime);

   void ResetSearchList();

   //! The pointers to the sensor modes, indexed by mode index.
   std::vector<WsfSensorMode*> mModeList;

   //! The last mode explicitly selected (i.e.: via WsfSensor::SelectMode).
   size_t mLastExplicitModeIndex;

   //! The mode index to be used for searching.
   //! If greater than the mode count then searching is disabled.
   std::vector<size_t> mSearchModeIndex;

   //! 'true' if search chances can be processed.
   bool mSearchAllowed;

   struct SensorTarget
   {
      SensorTarget() = default;
      SensorTarget(size_t aTargetIndex, size_t aModeIndex, double aDetectTime)
         : mTargetIndex(aTargetIndex)
         , mModeIndex(aModeIndex)
         , mDetectTime(aDetectTime)
      {
      }

      // Define operator< so that the priority queue provides results with the smallest time first.
      bool operator<(const SensorTarget& rhs) const { return mDetectTime > rhs.mDetectTime; }

      size_t mTargetIndex{0};
      size_t mModeIndex{0};
      double mDetectTime{0.0};
   };

   //! A queue of platform indices representing targets in the sensor's current FOV angle.
   using SensorTargetQueue = std::priority_queue<SensorTarget>;
   SensorTargetQueue mSensorTargetQueue;

   //! The value of UpdateInterval defined by user at run-time in the input files.
   double mSensorUpdateInterval;

   //! The value of Sim Time during the last Update interval.  This is used
   //! by SelectTarget to know when to search through the list of
   //! platforms and create a list of targets to return.
   double mLastUpdateTime;

   //! A single sector through which the sensor is to scan.  The sector is of a
   //! defined type of azimuth, elevation, or a combination of azimuth and elevation.
   class Sector
   {
   public:
      Sector();
      ~Sector() = default;

      enum Type
      {
         cUNDEFINED,
         cAZ,
         cEL,
         cAZEL,
      };

      bool ProcessInput(UtInput& aInput);
      bool Initialize(WsfSensor& aSensor);

      bool CheckTransition(double aAz, double aEl);

      Type                              GetType() const { return mType; }
      void                              SetType(Type aType) { mType = aType; }
      double                            GetStartAz() const { return mStartAz; }
      double                            GetEndAz() const { return mEndAz; }
      void                              SetEndAz(double aEndAz) { mEndAz = aEndAz; }
      void                              SetStartAz(double aStartAz) { mStartAz = aStartAz; }
      double                            GetStartEl() const { return mStartEl; }
      double                            GetEndEl() const { return mEndEl; }
      void                              SetStartEl(double aStartEl) { mStartEl = aStartEl; }
      void                              SetEndEl(double aEndEl) { mEndEl = aEndEl; }
      double                            GetAzScanRate() const { return mAzScanRate; }
      double                            GetElScanRate() const { return mElScanRate; }
      void                              SetAzScanRate(double aRate) { mAzScanRate = aRate; }
      void                              SetElScanRate(double aRate) { mElScanRate = aRate; }
      WsfArticulatedPart::SlewDirection GetAzSlewDirection() const { return mAzScanDir; }
      WsfArticulatedPart::SlewDirection GetElSlewDirection() const { return mElScanDir; }
      void   SetAzSlewDirection(WsfArticulatedPart::SlewDirection aAzScanDir) { mAzScanDir = aAzScanDir; }
      double GetScanTime() const { return mScanTime; }
      bool   Validate() const;

   private:
      static const double cUNINITIALIZED;

      Type                              mType;
      WsfArticulatedPart::SlewDirection mAzScanDir;
      WsfArticulatedPart::SlewDirection mElScanDir;
      double                            mStartAz;
      double                            mEndAz;
      double                            mStartEl;
      double                            mEndEl;
      double                            mAzScanRate;
      double                            mElScanRate;
      double                            mScanTime;
   };

   void BeginSlew(double aSimTime, Sector& aSector);

   void SlewToSector(double aSimTime, Sector& aSector);

   double EstimateDetectTime(double aSimTime, WsfPlatform& aTarget);

   void InitializeScan(double aSimTime);

   using Sectors = std::list<Sector>;

   bool              mSlewingToNextSector;
   bool              mIsContinuousAz;
   bool              mFrameBasedScheduling;
   Sector            mSlewTransition;
   Sector*           mCurrentSectorPtr;
   Sectors           mSectors;
   Sectors::iterator mSectorIter;
   Sectors::iterator mNextSectorIter;
   std::set<size_t>  mTargetsSelectedThisFrame;
};

#endif
