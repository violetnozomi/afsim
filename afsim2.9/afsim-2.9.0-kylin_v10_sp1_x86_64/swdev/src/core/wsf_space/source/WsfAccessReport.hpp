// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFACCESSREPORT_HPP
#define WSFACCESSREPORT_HPP

#include "wsf_space_export.h"

#include <list>
#include <map>
#include <string>

#include "UtCalendar.hpp"
#include "UtCallbackHolder.hpp"
class WsfPlatform;
#include "WsfScenarioExtension.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"

class UtInput;
class WsfDateTime;
class WsfSensor;
class WsfSensorResult;
class WsfSimuluation;

//! The data portion of the Access Report class.  It is configured in the scenario extension and
//! then passed into the simulation extension's constructor and populated during runtime.
class WsfAccessReportData
{
public:
   WsfAccessReportData();

   struct DetectionInterval
   {
      bool   mComplete;
      double mStart;
      double mEnd;
      double mDuration;
   };

   using DetectionIntervalList = std::list<DetectionInterval>;

   struct DetectorDetecteeKey
   {
      DetectorDetecteeKey(const std::string& aDetector, const std::string& aDetectee, const std::string& aSensorName);

      WsfStringId mDetector;
      WsfStringId mDetectee;
      WsfStringId mSensorName;

      bool operator<(const DetectorDetecteeKey& aRhs) const;
   };

   using DetectorDetecteeMap = std::map<DetectorDetecteeKey, DetectionIntervalList>;
   using KeyInputs           = std::list<DetectorDetecteeKey>;

   KeyInputs           mKeyInputs;
   DetectorDetecteeMap mDetectorDetecteeMap;
   std::string         mDefaultSensor;
   std::string         mFileName;
   std::string         mFormat;
};

//! @brief A utility that produces access reports for which platforms have visibility with each other.
//! Access Reports provide start time, end time, and the duration for platforms that have access (visibility) with each
//! other. Access is computed by detecting platform(s) with sensors, using SensorDetectionChanged callbacks. A variety
//! of options are avaialbable for specifying detecting platform(s), detected platform(s), and detecting sensor(s).
class WSF_SPACE_EXPORT WsfAccessReport : public WsfSimulationExtension, protected WsfAccessReportData
{
public:
   explicit WsfAccessReport(const WsfAccessReportData& aData);

   bool Initialize() override;

protected:
   void SensorDetectionChangedHandler(double aSimTime, WsfSensor* aSensorPtr, size_t aTargetIndex, WsfSensorResult& aResult);
   void SimulationCompleteHandler(double aSimTime);

private:
   std::string FormatDateTime(const double& aSimTime);

   UtCallbackHolder mCallbacks;
   UtCalendar       mSimulationCalendar;
};

//! The scenario extension used to configure the access reports object.
class WsfConfigureAccessReport : public WsfScenarioExtension, public WsfAccessReportData
{
public:
   bool Complete() override;
   bool ProcessInput(UtInput& aInput) override;
   void SimulationCreated(WsfSimulation& aSimulation) override;

private:
   void AddDefaultSensor(WsfPlatform& aPlatform);
};
#endif
