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

#ifndef WSFBEAMDIRECTOR_HPP
#define WSFBEAMDIRECTOR_HPP

#include "wsf_mil_export.h"

#include <set>

#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEvent.hpp"
#include "WsfImage.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSensorModeList.hpp"
#include "WsfStringId.hpp"
#include "WsfWeapon.hpp"

/** A sensor meant to be used as a beam director (target tracker) for a
   High-Energy Laser (HEL).

   This sensor has three modes: scanning, locked with imaging, and cued to image.
   During the scan phase the laser sensor is cued to a location or track.
   It uses this location as the center of its field-of-view (fov).
   It will then scan for targets within its scan fov.  If it detects one,
   it will "lock on" to this target and commence its imaging sequence.
   Once an image is acquired, if it subtends a given critical
   size (total # pixels above threshold), the sensor will "cue to the image", meaning
   it simulates a lock onto a sub-portion of the image.  At this point, a co-pointed
   HEL can fire.

   Breaklock can occur for a number of reasons.  One of these is that the target moves
   too quickly in a radial direction for the gimbals to catch up.  A target can also
   be out of range, or obscured.

   Many status messages are produced by this sensor; they are as follows:

<ul>
<li>   LOCKED_ON:            The lidar scan has successfully acquired a target.
<li>   BREAKLOCK:            The lidar lock has broken lock.
<li>   IN_CONSTRAINTS:       The scan center is within the sensor's field of regard.
<li>   OUT_OF_CONSTRAINTS:   The scan center is outside the sensor's field of view: the
      parent platform or target must maneuver to be IN_CONSTRAINTS.
<li>   CUED_TO_IMAGE:        We have sufficient resolution in our image, and we are locking onto
      a sub-portion of the image.
<li>   LOST_IMAGE_CUE:       We have lost our ability to cue to a sub-portion of the image and
      drop back into locked mode.
<li>   SLEW_RATE_EXCEEDED:   We have lost target lock due to the relative angular movement of the
      target exceeding the slew rate capability of the gimbals.
<li>   NO_DETECTIONS:        We have turned the beam director off due to a failure to detect any
      targets within the given scan volume.
</ul>
*/
class WSF_MIL_EXPORT WsfBeamDirector : public WsfSensor
{
public:
   WsfBeamDirector(WsfScenario& aScenario);
   WsfBeamDirector(const WsfBeamDirector& aSrc) = default;
   WsfBeamDirector& operator=(const WsfBeamDirector&) = delete;
   ~WsfBeamDirector() override                        = default;

   WsfSensor* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   void Update(double aSimTime) override;

   void TurnOff(double aSimTime) override;
   void TurnOn(double aSimTime) override;

   void PointAtTarget(double aSimTime, const WsfWeapon::FireTarget& aTarget);

   //! Get the current cue.
   const WsfTrack& GetCue() const { return mCue; }

   //! Get the current target offset
   WsfStringId GetTargetOffset() const { return mTargetOffset; }

   //! Set the cue; this is a method of passing the track from one mode to the next.
   void SetCue(const WsfTrack& aTrack) { mCue = aTrack; }

   size_t      GetEM_XmtrCount() const override { return 1; }
   WsfEM_Xmtr& GetEM_Xmtr() const { return GetCurrentMode()->GetEM_Xmtr(); }
   WsfEM_Xmtr& GetEM_Xmtr(size_t aIndex) const override { return GetCurrentMode()->GetEM_Xmtr(); }

   size_t      GetEM_RcvrCount() const override { return 1; };
   WsfEM_Rcvr& GetEM_Rcvr() const { return GetCurrentMode()->GetEM_Rcvr(); }
   WsfEM_Rcvr& GetEM_Rcvr(size_t aIndex) const override { return GetCurrentMode()->GetEM_Rcvr(); };

   double GetMaxRange() const { return GetCurrentMode()->GetMaxRange(); }

   void GetAzFOV(double& aMinAzFOV, double& aMaxAzFOV) const
   {
      return GetEM_Xmtr().GetAntenna()->GetAzimuthFieldOfView(aMinAzFOV, aMaxAzFOV);
   }

   void GetElFOV(double& aMinElFOV, double& aMaxElFOV) const
   {
      return GetEM_Xmtr().GetAntenna()->GetElevationFieldOfView(aMinElFOV, aMaxElFOV);
   }

   // void UpdateSlewRatesAndGimbalAngles(double aDeltaTime,
   //                                     double aTargetVecWCS[3]);

   void PointAtWCS(double aSimTime, double aPointAtWCS[3]);

   //! Return the tracking state of the current mode;
   //! This method is used by the API.
   int GetTrackingState() const { return GetCurrentMode()->GetType(); }

   bool IsTracking() const { return GetCurrentMode()->IsTracking(); }

   bool IsImaging() const { return GetCurrentMode()->IsImaging(); }

   //! Return the targets currently in the FOV of the sensor
   //! This method is used by the API.
   const std::set<WsfStringId>& GetTargetsInFOV() const { return GetCurrentMode()->GetTargetsInFOV(); }

   bool IsInFOR(WsfPlatform* aTargetPtr);

   class WsfBeamDirectorMode : public WsfSensorMode
   {
   public:
      //! The possible mode types of the beam director (previously in LaserState).
      //! Do not change the current enumerations; they are being used by the API.
      enum Type
      {
         cSCANNING               = 0,
         cLOCKED                 = 1,
         cLOCKED_WAITING_FOR_CUE = 2,
         cCUED                   = 3,
         cBREAKLOCK              = 4,
         cNUM_STATES             = 5,
         cUNDEFINED              = 999
      };

      WsfBeamDirectorMode() = default;
      WsfBeamDirectorMode(const WsfBeamDirectorMode& aSrc);
      WsfBeamDirectorMode& operator=(const WsfBeamDirectorMode&) = delete;
      ~WsfBeamDirectorMode() override;

      WsfSensorMode* Clone() const override { return new WsfBeamDirectorMode(*this); }
      bool           ProcessInput(UtInput& aInput) override;
      bool           Initialize(double aSimTime) override;

      void SetTrack(const WsfTrack& aTrack) { mTrack = aTrack; }

      bool IsImaging() const { return ((mType == cLOCKED) || (mType == cLOCKED_WAITING_FOR_CUE) || (mType == cCUED)); }
      bool IsTracking() const { return ((mType != cSCANNING) && (mType != cBREAKLOCK)); }

      WsfTrack& GetTrack() { return mTrack; }

      int GetType() const { return mType; }

      void PointAtTargetUpdate(double aSimTime);

      const std::set<WsfStringId>& GetTargetsInFOV() const { return mTargetsInFOV; }

      bool AttemptToDetect(double               aSimTime,
                           WsfPlatform*         aTargetPtr,
                           WsfSensor::Settings& aSettings,
                           WsfSensorResult&     aResult) override;

      void Deselect(double aSimTime) override;
      void Select(double aSimTime) override;

      const WsfImage& GetImage();

      const WsfImage::Object* GetImageObject() const
      {
         const WsfImage::Object* objectPtr = nullptr;
         if (!mImagePtr->GetObjects().empty())
         {
            objectPtr = &mImagePtr->GetObjects()[0];
         }
         return objectPtr;
      }

      const WsfTrack& GetTrack() const { return mTrack; }

      void Update(double aSimTime);

      /** Get the maximum range for this sensor */
      double GetMaxRange() const { return mXcvrAntenna.GetMaximumRange(); }

      WsfEM_Xmtr& GetEM_Xmtr() { return mTransmitter; }
      WsfEM_Xmtr& GetEM_Xmtr(unsigned int aIndex) { return mTransmitter; }

      WsfEM_Rcvr& GetEM_Rcvr() { return mReceiver; }
      WsfEM_Rcvr& GetEM_Rcvr(unsigned int aIndex) { return mReceiver; }

   protected:
      void PointAtWCS(double aSimTime, double aPointAtWCS[3]);

      void CreateTrackFromResult(double                 aSimTime,
                                 const WsfSensorResult& aResult, // in
                                 WsfPlatform*           aTargetPtr);

      void CreateImageFromResult(double                 aSimTime,
                                 const WsfSensorResult& aResult, // in
                                 WsfPlatform*           aTargetPtr);

      WsfEM_Xmtr    mTransmitter = WsfEM_Xmtr(WsfEM_Xmtr::cXF_SENSOR);
      WsfEM_Antenna mXcvrAntenna = WsfEM_Antenna();
      WsfEM_Rcvr    mReceiver    = WsfEM_Rcvr(WsfEM_Rcvr::cRF_SENSOR);

   private:
      Type                  mType               = cUNDEFINED;
      double                mLastUpdateTime     = 0.0;
      double                mAngularResolution  = 3.0e-5;
      double                mScanFOV_Az[2]      = {-UtMath::cPI, UtMath::cPI};
      double                mScanFOV_El[2]      = {-UtMath::cPI_OVER_2, UtMath::cPI_OVER_2};
      double                mTargetRadialExtent = 10.0;
      WsfTrack              mTrack              = WsfTrack();
      WsfImage*             mImagePtr           = nullptr;
      int                   mMinPixelsForCue    = 0;
      bool                  mPerfectCorrelation = false;
      std::set<WsfStringId> mTargetsInFOV       = std::set<WsfStringId>(); // Used in API

   }; // WsfBeamDirectorMode

private:
   void AcquireTarget(double aSimTime, const WsfTrack& aTrack);

   WsfBeamDirectorMode* GetCurrentMode() const override
   {
      return mWsfBeamDirectorModeList[mModeListPtr->GetCurrentMode()];
   }

   // Mode-independent variables
   WsfTrack                  mCue;
   WsfStringId               mTargetOffset;
   bool                      mCommentsEnabled;
   WsfBeamDirectorMode::Type mLastModeType;

   //! The sensor-specific list of modes (not valid until Initialize is called)
   std::vector<WsfBeamDirectorMode*> mWsfBeamDirectorModeList;
}; // WsfBeamDirector

#endif
