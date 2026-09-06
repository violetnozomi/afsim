// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFOPTICALBACKGROUND_HPP
#define WSFOPTICALBACKGROUND_HPP

#include "wsf_mil_export.h"

#include <mutex>
#include <vector>

#include "UtCalendar.hpp"
#include "UtLog.hpp"
class UtInput;
#include "UtVec3.hpp"
class WsfPlatform;
#include "WsfEM_Types.hpp"
#include "WsfSensor.hpp"

//! This class provides a common optical environment for WSF_EOIR_SENSOR, WSF_IRST_SENSOR and WSF_OPTICAL_SENSOR.
class WSF_MIL_EXPORT WsfOpticalEnvironment
{
public:
   WsfOpticalEnvironment();
   WsfOpticalEnvironment(const WsfOpticalEnvironment& aSrc);
   WsfOpticalEnvironment& operator=(const WsfOpticalEnvironment&) = delete;

   using BandList = std::vector<WsfEM_Types::OpticalBand>;

   bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);

   bool ProcessInput(UtInput& aInput);

   double ComputeBackgroundRadiance(double aSimTime, const BandList& aBands, WsfSensorResult& aResult, double& aSolarNoise);

   double ComputePathRadiance(double aSimTime, const BandList& aBand, WsfSensorResult& aResult);

   //! @name The following a public only to facilitate testing...
   //@{
   double ComputeDynamicBackgroundRadiance(const BandList& aBands,
                                           const UtVec3d&  aViewerLoc,
                                           const UtVec3d&  aViewerDir,
                                           const UtVec3d&  aSunLoc,
                                           const UtVec3d&  aMoonLoc,
                                           double&         aSolarNoise);
   //@}

   static double GetBlackbodyRadiance(const BandList& aBands, double aTemperature);

   static double GetSolarRadiantIntensity(const BandList& aBands);

   static double GetSolarRadiantIntensity(WsfEM_Types::OpticalBand aBand);

   static void GetWavelengthLimits(WsfEM_Types::OpticalBand aBand, double& aLowerWavelength, double& aUpperWavelength);

   static void PrintComputedData();

private:
   void UpdateSunMoonData(double aSimTime);

   double ReadRadianceValue(UtInput& aInput);

   static void ComputeAndPrintBandData(const char*             aName,
                                       double                  aLowerWavelength,
                                       double                  aUpperWavelength,
                                       double&                 aL,
                                       double&                 aPhi,
                                       ut::log::MessageStream& aStream);

   WsfPlatform* mPlatformPtr;

   //! Background radiance when looking at a target above the horizon (W/m^2/sr)
   double mBackgroundRadianceAboveHorizon;

   //! Background radiance when looking at a target below the horizon (W/m^2/sr)
   double mBackgroundRadianceBelowHorizon;

   //! The lower angular limit of the background transition range (relative to the horizon)
   double mLowerTransitionAngle;

   //! The upper angular limit of the background transition range (relative to the horizon)
   double mUpperTransitionAngle;

   //! Fixed path radiance (W/m^2/sr)
   double mPathRadiance;

   //! If true then the simple dynamic background radiance model will be used.
   bool mUseDynamicModel;

   //! @name The following are used only for the dynamic background model.
   //! These will eventually get replaced when the Sun and Moon location are cached in the simulation.
   //@{
   std::recursive_mutex mSunMoonUpdateMutex;    //!< Mutex for performing the update
   double               mLastSunMoonUpdateTime; //!< Last update time for the positions...
   UtCalendar           mCurrentTime;
   UtVec3d              mSunLoc;
   UtVec3d              mMoonLoc;
   //@}
};

#endif
