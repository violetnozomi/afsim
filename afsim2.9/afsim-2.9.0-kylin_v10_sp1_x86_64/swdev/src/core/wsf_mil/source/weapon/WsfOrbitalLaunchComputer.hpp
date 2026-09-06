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

#ifndef WSFORBITALLAUNCHCOMPUTER_HPP
#define WSFORBITALLAUNCHCOMPUTER_HPP

#include "wsf_mil_export.h"

#include <list>

class UtCalendar;
namespace ut
{
class CentralBody;
}
class UtInput;
#include "UtLog.hpp"
#include "WsfLaunchComputer.hpp"

//! A launch computer implementation for launch-to-orbit.
class WSF_MIL_EXPORT WsfOrbitalLaunchComputer : public WsfLaunchComputer
{
public:
   //! The basic type name (needed by weapon_tools and launch computer processor)
   static std::string BaseTypeName() { return "WSF_ORBITAL_LAUNCH_COMPUTER"; }

   WsfOrbitalLaunchComputer();
   WsfOrbitalLaunchComputer(const WsfOrbitalLaunchComputer& aSrc) = default;
   ~WsfOrbitalLaunchComputer() override                           = default;
   WsfOrbitalLaunchComputer& operator=(const WsfOrbitalLaunchComputer&) = delete;

   //! @name Common framework methods
   //@{
   WsfLaunchComputer* Clone() const override;
   const char*        GetScriptClassName() const override;
   bool               ProcessInput(UtInput& aInput) override;
   bool               Initialize(double aSimTime, WsfWeapon* aWeaponPtr) override;
   //@}

   //! @name WsfLaunchComputer methods
   //@{
   double EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime) override;
   //@}

   //! @name Main methods for this class
   //@{
   bool ComputeLEO_Insertion(double aSimTime, double aAltitude, double aInclination);
   //@}

   //! @name Accessors for last results
   //! These are valid only if the last call to ComputeLEO_Insertion returned 'true'
   //@{
   double GetLaunchHeading() const { return mLastResult.mLaunchHeading; }
   // GetLoftAngle is in the base class
   double      GetInclination() const { return mLastResult.mInclination; }
   double      GetInsertionTimeOfFlight() const { return mLastResult.mTimeOfFlight; }
   WsfGeoPoint GetInsertionLocation() const
   {
      return WsfGeoPoint(mLastResult.mLat, mLastResult.mLon, mLastResult.mAlt);
   }
   double GetInsertionSpeed() const { return mLastResult.mSpeedECI; }
   double GetInsertionSpeedFixed() const { return mLastResult.mSpeedFixed; }
   double GetInsertionHeading() const { return mLastResult.mHeading; }
   double GetInsertionPitch() const { return mLastResult.mFlightPathAngle; }
   double GetInsertionResidualDeltaV() const { return mLastResult.mResidualDeltaV; }
   double GetInsertionFuelRemaining() const { return mLastResult.mFuelRemaining; }
   double GetInsertionTimeRemaining() const { return mLastResult.mTimeRemaining; }
   void   GetOrbitalElements(double aResults[6]) const;
   //@}

   //! Define the data for a particular orbit.
   class WSF_MIL_EXPORT Orbit
   {
   public:
      Orbit();

      //! @name Launch conditions needed to achieve the orbit
      //@{
      double mLaunchHeading; // radians
      double mLoftAngle;     // radians
      //@}
      //! @name Resulting orbit data
      //@{
      double mInclination; // radians
      //@}
      //! @name Conditions at orbit insertion.
      //@{
      double mTimeOfFlight;    // seconds
      double mLat;             // decimal degrees
      double mLon;             // decimal degrees
      double mAlt;             // meters
      double mSpeedECI;        // meters/second
      double mSpeedFixed;      // meters/second
      double mHeading;         // radians
      double mFlightPathAngle; // radians
      double mResidualDeltaV;  // meters/second
      double mFuelRemaining;   // kilograms
      double mTimeRemaining;   // seconds
      //@}

      void Print(ut::log::MessageStream& aOut) const;
   };

   using OrbitList     = std::list<Orbit>;
   using OrbitListIter = std::list<Orbit>::const_iterator;

   static bool ReadOrbits(const std::string& aFileName, std::list<Orbit>& aOrbits);

   static bool WriteOrbits(WsfArticulatedPart* aSystemPtr, const std::string& aFileName, const std::list<Orbit>& aOrbits);

   static void ComputeOrbitalElements(const ut::CentralBody& aCentralBody,
                                      double                 aResults[6],
                                      const Orbit&           aOrbit,
                                      UtCalendar&            aCalendar);

   static void ComputeOrbitalElements(const ut::CentralBody& aCentralBody,
                                      double                 aResults[6],
                                      const double           aLocECI[3],
                                      const double           aVelECI[3],
                                      UtCalendar&            aCalendar);

protected:
   // Because the base class tries to do too much...
   bool InitializeTTIData() override { return true; }

   enum Variable
   {
      cALTITUDE,
      cINCLINATION
   };

   void InterpolateOrbits(Orbit& aResult, const Orbit& aOrbit1, const Orbit& aOrbit2, Variable aFunction, double aValue) const;

   bool InterpolateAlongLaunchHeading(Orbit& aResult, OrbitListIter aBestIter, double aAltitude) const;

private:
   std::string      mLEO_DataFileName;
   std::list<Orbit> mLEO_Data;

   Orbit mLastResult;
};

#endif
