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

#ifndef WSFTSPI_MOVER_HPP
#define WSFTSPI_MOVER_HPP

#include "wsf_export.h"

#include <iosfwd>
#include <string>

class UtInput;
#include "UtOptional.hpp"
#include "UtVec3dX.hpp"

class WsfDraw;
#include "WsfEvent.hpp"
#include "WsfMover.hpp"
#include "WsfTSPI.hpp"

//! WsfTSPI_Mover is a specialization of WsfMover that updates position based on
//! data from a file (see WsfTSPI for details.)

class WSF_EXPORT WsfTSPI_Mover : public WsfMover
{
public:
   //! End of-path option
   enum EndPath
   {
      cEP_EXTRAPOLATE, //!< Extrapolate at end-of-path
      cEP_STOP,        //!< Stop at end-of-path
      cEP_REMOVE       //!< Remove the platform at end-of-path
   };

   WsfTSPI_Mover() = delete;
   WsfTSPI_Mover(WsfScenario& aScenario);
   ~WsfTSPI_Mover() override;

   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;
   void      Update(double aSimTime) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   bool IsStopped() const override;
   bool IsPaused() const override;
   bool IsExtrapolating() const override;

   class OpenError
   {
   };

   bool InitializeMover(double aSimTime);
   void CloseFile();
   void OpenFile(const std::string& aFileName);

   double      SimStartTime() const { return mStartTime; }
   double      TSPI_StartTime() const { return mTSPI_StartTime; }
   std::string FileName() const { return mFileName; }

   bool SetFileName(const std::string& aName);
   void SetStartAtInitialTime(bool aValue) { mStartAtInitialTime = aValue; }
   void SetDeleteOnDestruct(bool aValue) { mDeleteOnDestruct = aValue; }

   void TriggerExtrapolation() override;

   struct Point
   {
      Point(const WsfTSPI& aPoint);

      WsfTSPI  mOrigTSPI;
      UtVec3dX mOrigLocWCS_SL; // at sea level, no alt
      UtVec3dX mOrigFRDinXYZ;  // Original Body Pointing Angles

      WsfTSPI  mMovedTSPI;
      UtVec3dX mMovedLocWCS_SL; // at sea level, no alt
      UtVec3dX mMovedFRDinXYZ;  // Translated Body Pointing Angles
   };

   struct PathMovement
   {
      bool mTransformIsNeeded  = false;
      bool mTranslateIsNeeded  = false;
      bool mStartPointIsAnchor = true; // else Anchor is End Point.

      ut::optional<double> mGC_AnchorHeadingRad; // may be input or calculated from user inputs
      ut::optional<double> mGreatCircleLat;      // if both mGreatCircleLat and mGreatCircleLon
      ut::optional<double> mGreatCircleLon;      // are zero, then use the mNewHeadingRad value.
   };

   // Modified to return true if successful, false if EOF.
   // Modified to return true if successful, false if EOF.
   bool Fetch_TSPI();

   void TranslateAndRotateToNewFile();

   void NormalizeLatLonDeg(double& aLatitudeDeg, double& aLongitudeDeg);

   class InitializeMoverEvent : public WsfEvent
   {
   public:
      InitializeMoverEvent();
      InitializeMoverEvent(double aSimTime, WsfTSPI_Mover* aMoverPtr);

      EventDisposition Execute() override;

   private:
      WsfTSPI_Mover* mMoverPtr;
   };


   //! A class encapsulating a TSPI kinematic trajectory model.
   //! It may be used to predict ahead the locations, times or apogee of entities.
   class TSPI_Model : public WsfMover::KinematicModel
   {
   public:
      TSPI_Model(double               aSimTime,
                 double               aStartTime,
                 double               aTSPI_StartTime,
                 WsfStringId          aModelTypeId,
                 const WsfTSPI_Mover* aTSPI_MoverPtr,
                 WsfDraw*             aDrawPtr = nullptr); // If we want to draw the trajectory.

      bool GetLocationXYZ(double aTime, double aLocXYZ[3]) const override;

      bool GetVelocityXYZ(double aTime, double aVelXYZ[3]) const override;

   protected:
      struct Point
      {
         Point(double aTimeStamp, const double aLocXYZ[3]);
         double mTimeStamp;
         double mLocXYZ[3]; // Spherical
         double mVelXYZ[3]; // Spherical
      };

      void Add(const Point& aPoint);

      double             mTSPI_StartTime;
      std::vector<Point> mPoints;
   };

   WsfMover::KinematicModel* CreateKinematicModel(double       aSimTime,
                                                  WsfStringId  aModelTypeId,
                                                  const double aLocationWCS[3],
                                                  const double aVelocityWCS[3],
                                                  WsfMover*    aMoverPtr,
                                                  WsfDraw*     aDrawPtr) const override;

protected:
   void UpdateMover(double aSimTime);

private:
   void ReadTSPI();
   void TranslateLocation();

   bool ProcessInputRelocateAndRotate(UtInput& aInput);
   //! Computes the rotation matrix for moving aAnchor to aTarget.
   static UtDCM ComputeTranslationTransform(const UtVec3dX& aAnchor, double aTargetLat, double aTargetLon);
   //! Computes the rotation matrix for rotating about aAnchor.
   static UtDCM ComputeRotationTransform(const UtVec3dX& aAnchor, const UtVec3dX& aNonAnchor, const PathMovement& aMovement);
   //! Applies aTranform to each point in aPoints.
   static void TransformPoints(std::vector<Point>& aPoints, const UtDCM& aTransform);

   std::ifstream* mInputStreamPtr;
   std::string    mFileName;
   bool           mIsOpen;
   bool           mEOF;
   bool           mIsInitialized;
   bool           mTranslate;          //! Whether to translate location to a user-defined reference.
   bool           mStartAtInitialTime; //! Whether to begin at the first logged TSPI timestamp.
   bool           mDeleteOnDestruct;
   bool           mIsExtrapolating; //! true is after a scripted burnout occurred.
   EndPath        mAtEndOfPath;
   double         mStartTime;
   double         mTSPI_StartTime;
   double         mCurrentUpdateTime;
   double         mDeltaLat;
   double         mDeltaLon;
   double         mRefLat = 0;
   double         mRefLon = 0;

   WsfTSPI mLastTSPI;
   WsfTSPI mCurrentTSPI;
   WsfTSPI mReadTSPI;

   PathMovement mMovement;
};

#endif
