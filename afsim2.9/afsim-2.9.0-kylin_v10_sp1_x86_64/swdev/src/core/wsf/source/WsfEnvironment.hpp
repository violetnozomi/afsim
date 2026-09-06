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

#ifndef WSFENVIRONMENT_HPP
#define WSFENVIRONMENT_HPP

#include "wsf_export.h"

#include <functional>

#include "TblLookup.hpp"
namespace ut
{
class CentralBodyEllipsoid;
}
#include "UtCentralPoint.hpp"
#include "UtCloneablePtr.hpp"
#include "UtInput.hpp"
class WsfNoiseCloudTypes;
class WsfScenario;

//! WsfEnvironment is a singleton that is responsible for
//! maintaining the data that describes the overall environment.
//! In the future, this singleton will be removed an objects
//! created on a regional basis to describe the environment for
//! that region

class WSF_EXPORT WsfEnvironment
{
public:
   //! Land Cover.
   //! @note The values are important and should not be changed (they are used as array indices).
   enum LandCover
   {
      cGENERAL              = 0,
      cURBAN                = 1,
      cAGRICULTURAL         = 2,
      cRANGELAND_HERBACEOUS = 3,
      cRANGELAND_SHRUB      = 4,
      cFOREST_DECIDUOUS     = 5,
      cFOREST_CONIFEROUS    = 6,
      cFOREST_MIXED         = 7,
      cFOREST_CLEARCUT      = 8,
      cFOREST_BLOCKCUT      = 9,
      cWETLAND_FORESTED     = 10,
      cWETLAND_NONFORESTED  = 11,
      cBARREN               = 12,
      cWATER                = 13,
      cNUM_LAND_COVERS      = 14
   };

   //! Land Formation
   //! @note The values are important and should not be changed (they are used as array indices).
   enum LandFormation
   {
      cINVALID          = 0, // Land Formation Enumeration Starts at 1.
      cLEVEL            = 1,
      cINCLINED         = 2,
      cUNDULATING       = 3,
      cROLLING          = 4,
      cHUMMOCKY         = 5,
      cRIDGED           = 6,
      cMODERATELY_STEEP = 7,
      cSTEEP            = 8,
      cBROKEN           = 9,
      cNUM_LAND_FORMS   = 10
   };

   //! Sea state
   //! @note The values are important and should not be changed (they are used as array indices).
   enum SeaState
   {
      cCALM_GLASSY    = 0,
      cCALM_RIPPLED   = 1,
      cSMOOTH         = 2,
      cSLIGHT         = 3,
      cMODERATE       = 4,
      cROUGH          = 5,
      cVERY_ROUGH     = 6,
      cNUM_SEA_STATES = 7
   };

   //! A table to get wind direction and speed as a function of altitude above MSL.
   class WindAltitudeTable
   {
   public:
      WindAltitudeTable();

      TblIndVarU<double>  mAltitude;
      TblDepVar1<double>  mWindDirection;
      TblDepVar1<double>  mWindSpeed;
      TblLookupLU<double> mAltLookup;
   };


   WsfEnvironment(WsfScenario& aScenario);
   ~WsfEnvironment();
   WsfEnvironment(const WsfEnvironment&) = delete;
   WsfEnvironment& operator=(const WsfEnvironment&) = delete;

   bool ProcessInput(UtInput& aInput);

   //! Global Land Cover / Land Form methods.
   LandCover GetLandCover() const { return mLandCover; }

   LandFormation GetLandFormation() const { return mLandFormation; }

   LandCover GetLandCover(double aLat, double aLon) const;

   LandFormation GetLandFormation(double aLat, double aLon) const;

   typedef std::function<LandCover(double, double)> GetLandCoverFunction;
   void                                             SetLandCoverStrategy(const GetLandCoverFunction& aFunction);

   SeaState GetSeaState() const { return mSeaState; }

   double GetWindSpeed() const { return mWindSpeed; }

   void SetWindSpeed(double aSpeed) { mWindSpeed = aSpeed; }

   double GetWindDirection() const { return mWindDirection; }

   void SetWindDirection(double aDirection) { mWindDirection = aDirection; }

   void GetWind(double aLat, double aLon, double aAlt, double& aWindHeading, double& aWindSpeed);

   //! Get the lower and upper altitudes of the clouds.
   //! @param aLowerAlt The altitude at which the clouds begin (m).
   //! @param aUpperAlt The altitude at which the clouds end (m).
   void GetCloudLevel(double& aLowerAlt, double& aUpperAlt) const
   {
      aLowerAlt = mCloudLowerAlt;
      aUpperAlt = mCloudUpperAlt;
   }

   bool LOS_ClearOfClouds(const double aEmissionPointWCS[3], const double aTargetPointWCS[3]) const;

   bool LOS_AdjustedClearOfClouds(const double aEmissionPointWCS[3], double aTargetPointWCS[3]) const;

   //! Get the density of water in the clouds.
   //! @returns the density of water in the clouds (kg/m^3)
   double GetCloudWaterDensity() const { return mCloudWaterDensity; }

   //! Get the altitude above which it is no longer raining.
   //! @returns The altitude above which it is is no longer raining (m).
   double GetRainUpperLevel() const { return mRainUpperAlt; }

   //! Get the rate of rain fall.
   //! @returns The rate of rain fall (m/s).
   double GetRainRate() const { return mRainRate; }

   double GetDustStormVisibility() const { return mDustStormVisibility; }

   void CreateWindAltitudeTable();

   WindAltitudeTable* GetWindAltitudeTablePtr() { return mWindAltitudeTablePtr; }

   WsfNoiseCloudTypes& GetNoiseCloudTypes() const { return *mNoiseCloudTypes; }

   bool UsingPolarOffsetAngles() const;

   const ut::CentralPoint& GetCentralPoint() const { return *mCentralPointPtr; }

   const ut::CentralBody& GetCentralBody() const { return mCentralPointPtr->GetAsCentralBody(); }

   const ut::CentralBodyEllipsoid& GetEllipsoid() const;

   void GetPolarOffsetAngles(double& aPolarOffsetAngleX, double& aPolarOffsetAngleY) const;

private:
   ut::CentralBody& GetCentralBodyP() { return mCentralPointPtr->GetAsCentralBody(); }

   LandCover     mLandCover;
   LandFormation mLandFormation;
   SeaState      mSeaState;
   double        mWindSpeed;
   double        mWindDirection;

   double mCloudLowerAlt;
   double mCloudUpperAlt;
   double mCloudWaterDensity;
   double mRainRate;
   double mRainUpperAlt;
   double mDustStormVisibility;
   double mPolarOffsetAngleX;
   double mPolarOffsetAngleY;

   WindAltitudeTable*   mWindAltitudeTablePtr;
   GetLandCoverFunction mGetLandCoverFunction;

   WsfNoiseCloudTypes*                mNoiseCloudTypes;
   ut::CloneablePtr<ut::CentralPoint> mCentralPointPtr;
};

#endif
