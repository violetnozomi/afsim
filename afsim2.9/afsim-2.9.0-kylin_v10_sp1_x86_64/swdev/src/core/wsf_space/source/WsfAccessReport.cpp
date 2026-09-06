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

#include "WsfAccessReport.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfComponentList.hpp"
#include "WsfDateTime.hpp"
#include "WsfGeometricSensor.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorObserver.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSensorTypes.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"

WsfAccessReportData::WsfAccessReportData()
   : mFormat("csv")
{
}

WsfAccessReport::WsfAccessReport(const WsfAccessReportData& aData)
   : WsfSimulationExtension()
   , WsfAccessReportData(aData)
{
}

bool WsfConfigureAccessReport::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "access_report")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "default_sensor")
         {
            aInput.ReadValue(mDefaultSensor);
         }
         else if (command == "file_prefix")
         {
            aInput.ReadValueQuoted(mFileName);
         }
         else if (command == "file_format")
         {
            aInput.ReadValue(mFormat);
            if (mFormat != "csv" && mFormat != "tsv")
            {
               throw UtInput::BadValue(aInput);
            }
         }
         else if (command == "report")
         {
            std::string detectorPlatformName;
            aInput.ReadValue(detectorPlatformName);
            if (UtStringUtil::CompareNoCase(detectorPlatformName, "all"))
            {
               detectorPlatformName = "*";
            }

            std::string toString;
            aInput.ReadValue(toString);
            aInput.StringEqual(toString, "to");

            std::string detecteePlatformName;
            aInput.ReadValue(detecteePlatformName);
            if (UtStringUtil::CompareNoCase(detecteePlatformName, "all"))
            {
               detecteePlatformName = "*";
            }

            std::string usingString;
            aInput.ReadValue(usingString);
            if (usingString == "using")
            {
               std::string sensorName;
               aInput.ReadValue(sensorName);
               if (UtStringUtil::CompareNoCase(sensorName, "all"))
               {
                  sensorName = "*";
               }
               mKeyInputs.push_back(DetectorDetecteeKey(detectorPlatformName, detecteePlatformName, sensorName));
            }
            else
            {
               aInput.PushBack(usingString);
               mKeyInputs.push_back(DetectorDetecteeKey(detectorPlatformName, detecteePlatformName, ""));
               continue;
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }

   return myCommand;
}

bool WsfAccessReport::Initialize()
{
   if (mKeyInputs.size() > 0)
   {
      mSimulationCalendar = GetSimulation().GetDateTime().GetStartDateAndTime();

      // Register for callbacks that allow us to update detections and write to the file at the end of the simulation.
      mCallbacks.Add(
         WsfObserver::SensorDetectionChanged(&GetSimulation()).Connect(&WsfAccessReport::SensorDetectionChangedHandler, this));
      mCallbacks.Add(
         WsfObserver::SimulationComplete(&GetSimulation()).Connect(&WsfAccessReport::SimulationCompleteHandler, this));
   }
   return true;
}

bool WsfConfigureAccessReport::Complete()
{
   // Note: Error / warning handling is simplified with multiple direct return statements,
   // rather than using a single return statement at the end of the method.
   //
   //!!! This is called from WsfScenario::LoadComplete, and does not use the return value.
   //!!! (TODO) need to examine WsfScenario to see how to properly handle LoadComplete errors.
   //!!! For now throwing UtExceptions from here to prevent simulation crash.

   // The following code replaces all wildcards in the input keys with actual values.
   // Detector values.
   if (mKeyInputs.size() > 0)
   {
      KeyInputs           detectorInputs;
      KeyInputs::iterator inputKeyIter = mKeyInputs.begin();
      while (inputKeyIter != mKeyInputs.end())
      {
         DetectorDetecteeKey& inputKey             = *inputKeyIter;
         const std::string&   detectorPlatformName = inputKey.mDetector;

         if (detectorPlatformName == "*")
         {
            // Add entries for all platforms.
            for (unsigned int platformIndex = 0; platformIndex < GetScenario().GetInputPlatformCount(); ++platformIndex)
            {
               detectorInputs.push_back(DetectorDetecteeKey(GetScenario().GetInputPlatformEntry(platformIndex)->GetName(),
                                                            inputKey.mDetectee,
                                                            inputKey.mSensorName));
            }
         }
         else
         {
            if (GetScenario().GetInputPlatformByName(detectorPlatformName) == nullptr)
            {
               std::string errString =
                  "(access_report) Detector platform name " + detectorPlatformName + " does not exist.";
               throw UtException(errString);
               // std::cout << errString << std::endl;
               // return false;
            }
            detectorInputs.push_back(inputKey);
         }
         ++inputKeyIter;
      }

      // Detectee values
      KeyInputs detecteeInputs;
      inputKeyIter = detectorInputs.begin();
      while (inputKeyIter != detectorInputs.end())
      {
         DetectorDetecteeKey& inputKey             = *inputKeyIter;
         const std::string&   detecteePlatformName = inputKey.mDetectee;

         if (detecteePlatformName == "*")
         {
            // Add entries for all platforms.
            for (unsigned int platformIndex = 0; platformIndex < GetScenario().GetInputPlatformCount(); ++platformIndex)
            {
               const std::string detecteePlatformName = GetScenario().GetInputPlatformEntry(platformIndex)->GetName();
               detecteeInputs.push_back(DetectorDetecteeKey(inputKey.mDetector, detecteePlatformName, inputKey.mSensorName));
            }
         }
         else
         {
            if (GetScenario().GetInputPlatformByName(detecteePlatformName) == nullptr)
            {
               std::string errString =
                  "(access_report) Detectee platform name " + detecteePlatformName + " does not exist.";
               throw UtException(errString);
               // std::cout << errString << std::endl;
               // return false;
            }
            detecteeInputs.push_back(inputKey);
         }
         ++inputKeyIter;
      }

      // Sensor inputs.
      KeyInputs detecteeSensorInputs;
      inputKeyIter = detecteeInputs.begin();
      while (inputKeyIter != detecteeInputs.end())
      {
         DetectorDetecteeKey& inputKey             = *inputKeyIter;
         const std::string&   detecteePlatformName = inputKey.mDetectee;
         const std::string    detectorPlatformName = inputKey.mDetector;

         if (detectorPlatformName != detecteePlatformName)
         {
            WsfPlatform* detectorPlatformPtr =
               GetScenario().GetInputPlatformByName(detectorPlatformName); // Name is already verified.
            if (inputKey.mSensorName == "*")
            {
               const WsfPlatformComponentList& componentList = detectorPlatformPtr->GetComponents();

               if (componentList.GetComponentCount<WsfSensor>() == 0)
               {
                  AddDefaultSensor(*detectorPlatformPtr);
               }
               for (WsfComponentList::RoleIterator<WsfSensor> sensorIter(componentList); !sensorIter.AtEnd(); ++sensorIter)
               {
                  detecteeSensorInputs.push_back(
                     DetectorDetecteeKey(detectorPlatformName, detecteePlatformName, sensorIter->GetName()));
               }
            }
            else if (inputKey.mSensorName == "") // No sensor specified; attempt to add the default sensor
            {
               AddDefaultSensor(*detectorPlatformPtr);
               inputKey.mSensorName = mDefaultSensor;
               detecteeInputs.push_back(inputKey);
            }
            else
            {
               if (detectorPlatformPtr->GetComponent<WsfSensor>(inputKey.mSensorName) == nullptr)
               {
                  std::string errString = "(access_report) No sensor definition exists for sensor " +
                                          inputKey.mSensorName + " on platform " + detectorPlatformName;
                  throw UtException(errString);
               }
               detecteeSensorInputs.push_back(inputKey);
            }
         }
         ++inputKeyIter;
      }

      for (auto keyInput : detecteeSensorInputs)
      {
         mDetectorDetecteeMap[keyInput] = DetectionIntervalList();
      }
   }

   return true;
}

//! Use the SensorDetectionChanged callback to store detection interval results.
void WsfAccessReport::SensorDetectionChangedHandler(double           aSimTime,
                                                    WsfSensor*       aSensorPtr,
                                                    size_t           aTargetIndex,
                                                    WsfSensorResult& aResult)
{
   WsfSimulation* simulationPtr = aSensorPtr->GetSimulation();

   std::string                   detectorName = aSensorPtr->GetPlatform()->GetName();
   std::string                   detecteeName = simulationPtr->GetPlatformByIndex(aTargetIndex)->GetName();
   DetectorDetecteeMap::iterator detectionIntervalListIter =
      mDetectorDetecteeMap.find(DetectorDetecteeKey(detectorName, detecteeName, aSensorPtr->GetName()));
   if (detectionIntervalListIter != mDetectorDetecteeMap.end())
   {
      if (aResult.Detected() == true)
      {
         DetectionInterval detectionInterval;

         detectionInterval.mComplete = false;
         detectionInterval.mStart    = aSimTime;
         detectionInterval.mEnd      = simulationPtr->GetEndTime();
         detectionInterval.mDuration = 0;

         detectionIntervalListIter->second.push_back(detectionInterval);
      }
      else
      {
         DetectionInterval& detectionInterval = detectionIntervalListIter->second.back();

         detectionInterval.mComplete = true;
         detectionInterval.mEnd      = aSimTime;
         detectionInterval.mDuration = detectionInterval.mEnd - detectionInterval.mStart;
      }
   }
}

//! Use the SimulationCompleted callaback to write detection inteval output to the specified file.
void WsfAccessReport::SimulationCompleteHandler(double aSimTime)
{
   std::ofstream accessReportFile;
   accessReportFile.open(mFileName + "." + mFormat);

   // Header
   std::vector<std::string> headerStringVector = {"Access",
                                                  "Detector",
                                                  "Detectee",
                                                  "Sensor",
                                                  "Start Time",
                                                  "End Time",
                                                  "Rel. Start Time (s)",
                                                  "Rel. End Time (s)",
                                                  "Duration (s)"};
   if (mFormat == "csv")
   {
      accessReportFile << UtStringUtil::Join(",", headerStringVector) << std::endl;
   }
   else if (mFormat == "tsv")
   {
      accessReportFile << UtStringUtil::Join("\t", headerStringVector) << std::endl;
   }

   // Body

   int accessIndex = 1;
   for (auto& detecteeSensorMap : mDetectorDetecteeMap)
   {
      std::string detectorName = detecteeSensorMap.first.mDetector;
      std::string detecteeName = detecteeSensorMap.first.mDetectee;
      std::string sensorName   = detecteeSensorMap.first.mSensorName;

      for (auto& detectionInterval : detecteeSensorMap.second)
      {
         if (!detectionInterval.mComplete)
         {
            detectionInterval.mComplete = true;
            detectionInterval.mDuration = detectionInterval.mEnd - detectionInterval.mStart;
         }

         std::vector<std::string> rowStringVector = {std::to_string(accessIndex++),
                                                     detectorName,
                                                     detecteeName,
                                                     sensorName,
                                                     FormatDateTime(detectionInterval.mStart),
                                                     FormatDateTime(detectionInterval.mEnd),
                                                     std::to_string(detectionInterval.mStart),
                                                     std::to_string(detectionInterval.mEnd),
                                                     std::to_string(detectionInterval.mDuration)};

         if (mFormat == "csv")
         {
            accessReportFile << UtStringUtil::Join(",", rowStringVector) << std::endl;
         }
         else if (mFormat == "tsv")
         {
            accessReportFile << UtStringUtil::Join("\t", rowStringVector) << std::endl;
         }
      }
   }

   accessReportFile.close();
}

std::string WsfAccessReport::FormatDateTime(const double& aSimTime)
{
   UtCalendar aCalendar = mSimulationCalendar;
   aCalendar.AdvanceTimeBy(aSimTime);

   char dateTime[25];
   std::sprintf(dateTime,
                "%04i-%02i-%02i %02i:%02i:%07.4f",
                aCalendar.GetYear(),
                aCalendar.GetMonth(),
                aCalendar.GetDay(),
                aCalendar.GetHour(),
                aCalendar.GetMinute(),
                aCalendar.GetSecond());

   return std::string(dateTime);
}

// private
//! Attempt to add the default sensor to a platform.  An error and exception are thrown if the default sensor type is
//! specified but not defined in the input.
void WsfConfigureAccessReport::AddDefaultSensor(WsfPlatform& aPlatform)
{
   if (mDefaultSensor == "")
   {
      auto logger = ut::log::warning() << "(access_report) Platform provides no sensor definitions, consider utilizing "
                                          "the \"default_sensor\" command";
      logger.AddNote() << "Platform Name: " << aPlatform.GetName();
   }
   else if (GetScenario().GetSensorTypes().IsType(mDefaultSensor))
   {
      // Clone the default sensor and place it on the platform.
      WsfSensor* sensorPtr = GetScenario().GetSensorTypes().Clone(mDefaultSensor);
      sensorPtr->SetName(mDefaultSensor);
      aPlatform.AddComponent(sensorPtr);
   }
   else
   {
      std::string errString = "(access_report) No sensor definition exists for default_sensor " + mDefaultSensor;
      throw UtException(errString);
   }
}

WsfAccessReportData::DetectorDetecteeKey::DetectorDetecteeKey(const std::string& aDetector,
                                                              const std::string& aDetectee,
                                                              const std::string& aSensorName)
   : mDetector(aDetector)
   , mDetectee(aDetectee)
   , mSensorName(aSensorName)
{
}

bool WsfAccessReportData::DetectorDetecteeKey::operator<(const DetectorDetecteeKey& aRhs) const
{
   if (aRhs.mDetector != mDetector)
   {
      return (mDetector < aRhs.mDetector);
   }
   else if (aRhs.mSensorName != mSensorName)
   {
      return (mSensorName < aRhs.mSensorName);
   }
   else
   {
      return (mDetectee < aRhs.mDetectee);
   }
}

void WsfConfigureAccessReport::SimulationCreated(WsfSimulation& aSimulation)
{
   // Only need to create the simulation extension if the scenario extension was configured.
   if (mKeyInputs.size() > 0)
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfAccessReport>(*this));
   }
}
