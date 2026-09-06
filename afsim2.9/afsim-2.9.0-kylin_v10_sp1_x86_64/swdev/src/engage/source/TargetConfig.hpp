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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef TARGETCONFIG_HPP
#define TARGETCONFIG_HPP

#include <string>
#include <vector>

class UtInput;


namespace engage
{

class Simulation;
class Task;

class TargetConfig
{
public:
   struct PathPoint
   {
      double mTime;
      double mPosXYZ[3]; // X, Y, Z position
      double mVelXYZ[3]; // X, Y, Z velocity
      double mPitch;
      double mRoll;
      double mYaw;
   };
   using FlightPathVec = std::vector<PathPoint>;

   TargetConfig();

   bool Complete(bool aTargetGridUsed);

   bool CreatePlatform(Simulation& aSimulation, const Task& aTask) const;

   bool ProcessInput(UtInput& aInput);
   bool ProcessSiteGridInput(UtInput& aInput);

   int GetPointCount() const { return mPointCount; }
   int GetGridDrCount() const { return mGridDownRangeCount; }
   int GetGridCrCount() const { return mGridCrossRangeCount; }
   int GetGridAltCount() const { return mGridAltitudeCount; }

   double      GetGridDrMin() const { return mGridDownRangeMin; }
   double      GetGridDrMax() const { return mGridDownRangeMax; }
   double      GetGridDrStep() const { return mGridDownRangeStep; }
   std::string GetGridDrUnits() const { return mGridCrossRangeUnits; }

   double      GetGridCrMin() const { return mGridCrossRangeMin; }
   double      GetGridCrMax() const { return mGridCrossRangeMax; }
   double      GetGridCrStep() const { return mGridCrossRangeStep; }
   std::string GetGridCrUnits() const { return mGridCrossRangeUnits; }

   double      GetGridAltMin() const { return mGridAltitudeMin; }
   double      GetGridAltMax() const { return mGridAltitudeMax; }
   double      GetGridAltStep() const { return mGridAltitudeStep; }
   std::string GetGridAltUnits() const { return mGridAltitudeUnits; }

   double      GetGridSpeed() const { return mGridSpeed; }
   double      GetGridMach() const { return mGridMach; }
   std::string GetGridSpeedUnits() const { return mGridSpeedUnits; }

   void GetRunPosition(int aTargetIndex, double& aX, double& aY, double& aZ) const;

private:
   std::string mTargetType;

   //! @name Target grid
   //@{
   double      mGridDownRangeMin;
   double      mGridDownRangeMax;
   double      mGridDownRangeStep;
   std::string mGridDownRangeUnits;
   double      mGridCrossRangeMin;
   double      mGridCrossRangeMax;
   double      mGridCrossRangeStep;
   std::string mGridCrossRangeUnits;
   double      mGridAltitudeMin;
   double      mGridAltitudeMax;
   double      mGridAltitudeStep;
   std::string mGridAltitudeUnits;
   double      mGridSpeed;
   double      mGridMach;
   std::string mGridSpeedUnits;

   //! Computed values.
   int mGridPointCount;
   int mGridDownRangeCount;
   int mGridCrossRangeCount;
   int mGridAltitudeCount;
   //@}

   //! @name Simple flight path (single starting point, constant heading and speed).
   //@{
   double mSimpleX;       // SXT
   double mSimpleY;       // SYT
   double mSimpleZ;       // SZT
   double mSimpleSpeed;   // SVT
   double mSimpleHeading; // SPSIT
   double mSimplePitch;   // Not in ESAMS
   double mSimpleRoll;    // Not in ESAMS
   //@}

   //! @name Multiple point flight path.
   //@{
   FlightPathVec mFlightPath;
   //@}

   //! @name Multiple point flight route.
   //@{
   struct RoutePoint
   {
      double mPosXYZ[3]; // X, Y, Z position
      double mSpeed;
   };

   std::vector<RoutePoint> mFlightRoute;
   //@}

   int mPointCount;

   bool mUseTargetPath; // Use the target input for position and path
};

} // namespace engage

#endif
