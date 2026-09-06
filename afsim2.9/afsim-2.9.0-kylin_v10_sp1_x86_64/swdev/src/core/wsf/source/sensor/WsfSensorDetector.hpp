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

#ifndef WSFSENSORDETECTOR_HPP
#define WSFSENSORDETECTOR_HPP

#include "wsf_export.h"

#include <list>
#include <memory>
#include <string>

class UtInput;

class UtInput;
#include "UtMath.hpp"
class WsfSensorMode;

//! An abstract base class that defines a detector for a sensor.
//!
//! Not all sensors support this feature, but for those that do it provides access to an extendable
//! library that implement detection techniques. When a sensor makes use of this interface
//! it typically uses the processed signal and result to provide for signal detection criteria.
namespace wsf
{

class WSF_EXPORT SensorDetector
{
public:
   SensorDetector();
   virtual ~SensorDetector() = default;

   virtual SensorDetector* Clone() const = 0;

   virtual double ComputeProbabilityOfDetection(double aSignalToNoise,
                                                double aDetectionThreshold = UtMath::LinearToDB(3.0)) = 0;

   virtual bool Initialize(double aSimTime, WsfSensorMode* aModePtr, size_t aBeamIndex = 0);

   virtual bool ProcessInput(UtInput& aInput);

   //! Set the debug flag for the detector.
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }
   //! Returns 'true' if debugging is enabled for the detector.
   bool DebugEnabled() const { return mDebugEnabled; }


protected:
   //! 'true' if 'debug_detector' was specified.
   bool mDebugEnabled;
};

class WSF_EXPORT SensorDetectorTypes
{
public:
   typedef SensorDetector* (*FactoryPtr)(const std::string&);
   typedef std::list<FactoryPtr> ObjectFactoryList;

   SensorDetectorTypes(const SensorDetectorTypes& aSrc) = default;
   SensorDetectorTypes& operator=(const SensorDetectorTypes& aRhs) = default;
   virtual ~SensorDetectorTypes()                                  = default;

   static bool LoadInstance(UtInput& aInput, std::unique_ptr<SensorDetector>& aSensorDetectorPtr);
   static void AddObjectFactory(FactoryPtr aFactoryPtr);

private:
   static SensorDetector* CreateInstance(const std::string& aTypeName);

   //! The list of object factories
   static ObjectFactoryList mObjectFactoryList;
};

} // namespace wsf
#endif
