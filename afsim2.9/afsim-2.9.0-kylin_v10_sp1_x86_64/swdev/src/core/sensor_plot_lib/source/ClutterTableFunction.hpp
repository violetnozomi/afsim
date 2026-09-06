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

#ifndef CLUTTERTABLEFUNCTION_HPP
#define CLUTTERTABLEFUNCTION_HPP

#include <string>
#include <vector>

#include "Function.hpp"
#include "Sensor.hpp"
#include "Target.hpp"

class ClutterTableFunction : public Function
{
public:
   explicit ClutterTableFunction(WsfScenario& aScenario);

   bool Execute(WsfSimulation& aSimulation) override;
   bool ProcessInput(UtInput& aInput) override;

   class Point
   {
   public:
      Point()
         : range(0.0)
         , bearing(0.0)
         , clutter(0.0)
      {
      }
      bool   operator<(const Point& aRHS) const { return range < aRHS.range; }
      double range;
      double bearing;
      double clutter;
   };

   class PointArray
   {
   public:
      PointArray()
         : altitude(0.0)
         , data()
      {
      }
      bool               operator<(const PointArray& aRHS) const { return altitude < aRHS.altitude; }
      double             altitude;
      std::vector<Point> data;
   };

   typedef std::vector<PointArray> Envelope;

   //! return the altitude at an index within the envelope
   double Altitude(Envelope& aEnvelope, size_t aAltIndex) const { return aEnvelope[aAltIndex].altitude; }

   //! return the range at an altitude/range index within the envelope
   double GroundRange(Envelope& aEnvelope, size_t aAltIndex, size_t aRangeIndex) const
   {
      return mEnvelope[aAltIndex].data[aRangeIndex].range;
   }

protected:
   void PrintCluttertable(Envelope& aEnvelope);

private:
   void ProcessRangeBearingInput(UtInput& aInput, const std::string& aBlockTerminator, PointArray& aRanges);

   WsfSimulation* mSimulationPtr;
   Envelope       mEnvelope;
   Sensor         mSensor;
   Target         mTarget;

   double              mSensorPlatformYaw;
   double              mSensorPlatformPitch;
   double              mSensorPlatformRoll;
   double              mSensorPlatformLat;
   double              mSensorPlatformLon;
   double              mSensorPlatformAlt;
   bool                mSensorPlatformAltSet;
   std::string         mOutputObjectName;
   std::string         mOutputFileName;
   std::string         mAltUnitsStr;
   std::string         mBearingUnitsStr;
   double              mAltUnitsScale;
   std::string         mRangeUnitsStr;
   double              mRangeUnitsScale;
   double              mBearingUnitsScale;
   std::vector<double> mBearings;
};

#endif
