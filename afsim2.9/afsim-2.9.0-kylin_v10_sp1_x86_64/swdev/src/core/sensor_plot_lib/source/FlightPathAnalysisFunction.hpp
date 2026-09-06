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

#ifndef FLIGHTPATHANALYSISFUNCTION_HPP
#define FLIGHTPATHANALYSISFUNCTION_HPP

#include <string>
#include <vector>

#include "MapPlotFunction.hpp"
#include "SupTMProjection.hpp"
class WsfPlatform;
class WsfSensor;
#include "mover/WsfTSPI.hpp"

class FlightPathAnalysisFunction : public MapPlotFunction
{
public:
   enum Variable
   {
      cVAR_REQUIRED_JAMMER_POWER
   };

   //! The list of values at a specified sample point
   using Point = std::vector<double>;

   static constexpr double cUNDEFINED = 1.0E+30;

   explicit FlightPathAnalysisFunction(const WsfScenario& aScenario);
   ~FlightPathAnalysisFunction() override = default;

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

private:
   class PathPoint
   {
   public:
      PathPoint()
         : mLat(0.0)
         , mLon(0.0)
         , mAlt(0.0)
         , mSpeed(0.0)
         , mHeading(0.0)
         , mPitch(0.0)
         , mRoll(0.0)
         , mAltIsAGL(false)
      {
      }

      double mLat;
      double mLon;
      double mAlt;
      double mSpeed;
      double mHeading;
      double mPitch;
      double mRoll;
      bool   mAltIsAGL;
   };
   class Selector
   {
   public:
      enum SelectorType
      {
         cSENSOR_TYPE,
         cSENSOR_CATEGORY,
         cPLATFORM_TYPE,
         cPLATFORM_CATEGORY
      };
      SelectorType mType;
      WsfStringId  mValue;
   };

   using SelectorList = std::vector<Selector>;

   bool ConvertALARM_LatLon(const std::string& aString, bool aIsLat, double& aOutput);

   void ConvertCartesianToSpherical(double aDownRange, double aCrossRange, double& aLatitude, double& aLongitude);

   void ConvertSphericalToCartesian(double aLatitude, double aLongitude, double& aDownRange, double& aCrossRange);

   void EvaluatePoint(double aSimTime, WsfPlatform* aTargetPtr, std::vector<double>& aDataValues);

   void EvaluateSensor(double aSimTime, WsfSensor* aSensorPtr, WsfPlatform* aTargetPtr, std::vector<double>& aDataValues);

   void InitializeSensorPlatforms();

   bool InSelectorList(WsfSensor* aSensorPtr, const SelectorList& aSelectorList);

   void ReadALARM_FPA_File(const std::string& aFileName);

   void ReadTSPI_File(const std::string& aFileName);

   void ReadSelector(UtInput& aInput, SelectorList& aSelectorList);

   void ResetPathPoints();

   void WriteGnuPlotFile(const std::vector<Point>& aVarValues);

   WsfSimulation*           mSimulationPtr;
   std::vector<Variable>    mVarList;
   std::vector<std::string> mVarLabels;
   std::vector<double>      mVarMinValues;

   std::vector<PathPoint> mPathPoints;
   std::string            mALARM_FPA_FileName;
   std::string            mTSPI_FileName;
   WsfTSPI                mTSPI_Point;
   //! Sample interval when using a route.
   double mSampleInterval;

   double      mSensorPlatformYaw;
   double      mSensorPlatformPitch;
   double      mSensorPlatformRoll;
   std::string mModeName;
   size_t      mModeIndex;
   bool        mAutomaticTargetCueing;
   bool        mReverseEvaluations;

   WsfStringId mTargetPlatformId;

   std::string mRangeUnits;

   //! Used for converting between X/Y and Lat/Lon.
   SupTMProjection mProjection;

   std::string mGnuPlotFile;
   std::string mHeaderLine1;
   std::string mHeaderLine2;
   std::string mHeaderLine3;

   SelectorList mExclusionList;
   SelectorList mInclusionList;

   std::vector<WsfSensor*> mSensors;

   double mOriginToENU_Transform[3][3];
};

#endif
