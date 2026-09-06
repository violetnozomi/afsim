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

#include "WsfEclipseReport.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtStringUtil.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceObserver.hpp"

WsfEclipseReportData::WsfEclipseReportData()
   : mReportAll(false)
   , mEclipseMap()
   , mFileName("")
   , mFormat("csv")
{
}

WsfEclipseReport::WsfEclipseReport(const WsfEclipseReportData& aData)
   : WsfSimulationExtension()
   , WsfEclipseReportData(aData)
   , mCallbacks()
   , mSimulationCalendar()
   , mPlatformToCurrentEventIdMap()
{
}

bool WsfConfigureEclipseReport::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "eclipse_report")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "file_prefix")
         {
            aInput.ReadValueQuoted(mFileName);
         }
         else if (command == "file_format")
         {
            aInput.ReadValue(mFormat);
            if ((mFormat != "csv") && (mFormat != "tsv"))
            {
               throw UtInput::BadValue(aInput);
            }
         }
         else if (command == "report")
         {
            std::string platformName;
            aInput.ReadValue(platformName);
            if (UtStringUtil::CompareNoCase(platformName, "all"))
            {
               mReportAll = true;
            }
            else
            {
               mEclipseMap[platformName] = EclipseIntervalList();
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

bool WsfEclipseReport::Initialize()
{
   if (mReportAll || (mEclipseMap.size() > 0))
   {
      mSimulationCalendar = GetSimulation().GetDateTime().GetStartDateAndTime();

      // Register for callbacks that allow us to update detections and write to the file at the end of the simulation.
      mCallbacks.Add(
         WsfObserver::PlatformInitialized(&GetSimulation()).Connect(&WsfEclipseReport::PlatformInitializedHandler, this));
      mCallbacks.Add(
         WsfObserver::SimulationComplete(&GetSimulation()).Connect(&WsfEclipseReport::SimulationCompleteHandler, this));
      mCallbacks.Add(
         WsfObserver::OrbitalManeuverCompleted(&GetSimulation()).Connect(&WsfEclipseReport::OrbitalManeuverCompleted, this));
   }
   return true;
}

void WsfEclipseReport::PlatformInitializedHandler(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Set up initial events.
   if (mReportAll || (mEclipseMap.find(aPlatformPtr->GetName()) != mEclipseMap.end()))
   {
      WsfSpaceMoverBase* spaceMoverPtr = dynamic_cast<WsfSpaceMoverBase*>(aPlatformPtr->GetMover());
      if (spaceMoverPtr != nullptr)
      {
         InitiateEclipseEvent(aSimTime, 0, *spaceMoverPtr);
      }
   }
}

//! Use the SimulationCompleted callaback to write detection inteval output to the specified file.
void WsfEclipseReport::SimulationCompleteHandler(double aSimTime)
{
   std::ofstream EclipseReportFile;
   EclipseReportFile.open(mFileName + "." + mFormat);

   // Header

   std::vector<std::string> headerStringVector =
      {"Eclipse", "Platform", "Start Time", "End Time", "Rel. Start Time (s)", "Rel. End Time (s)", "Duration (s)"};
   if (mFormat == "csv")
   {
      EclipseReportFile << UtStringUtil::Join(",", headerStringVector) << std::endl;
   }
   else if (mFormat == "tsv")
   {
      EclipseReportFile << UtStringUtil::Join("\t", headerStringVector) << std::endl;
   }

   // Body

   int EclipseIndex = 1;
   for (auto& eclipses : mEclipseMap)
   {
      std::string platformName = eclipses.first;

      for (auto& eclipseInterval : eclipses.second)
      {
         if (eclipseInterval.mEnd == 0.0)
         {
            eclipseInterval.mEnd = aSimTime;
         }
         eclipseInterval.mDuration = eclipseInterval.mEnd - eclipseInterval.mStart;

         std::vector<std::string> rowStringVector = {std::to_string(EclipseIndex++),
                                                     platformName,
                                                     FormatDateTime(eclipseInterval.mStart),
                                                     FormatDateTime(eclipseInterval.mEnd),
                                                     std::to_string(eclipseInterval.mStart),
                                                     std::to_string(eclipseInterval.mEnd),
                                                     std::to_string(eclipseInterval.mDuration)};

         if (mFormat == "csv")
         {
            EclipseReportFile << UtStringUtil::Join(",", rowStringVector) << std::endl;
         }
         else if (mFormat == "tsv")
         {
            EclipseReportFile << UtStringUtil::Join("\t", rowStringVector) << std::endl;
         }
      }
   }

   EclipseReportFile.close();
}

std::string WsfEclipseReport::FormatDateTime(const double& aSimTime)
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

//! Set up the eclipse event (called when the platform is initialized or the satellite maneuvers).
//! @param aSimTime The current simulation time.
//! @param aId The id to be associated with this event.  The most recently initiated event for a given platform is
//! considered the current one.
//! @param aSpaceMover The space mover object associated with this eclipse.
void WsfEclipseReport::InitiateEclipseEvent(double aSimTime, size_t aId, WsfSpaceMoverBase& aSpaceMover)
{
   double                        timeToEntry, timeToExit;
   bool                          solutionExists = aSpaceMover.GetPropagator().GetEclipseTimes(timeToEntry, timeToExit);
   std::unique_ptr<EclipseEvent> eventPtr{nullptr};
   mPlatformToCurrentEventIdMap[aSpaceMover.GetPlatform()->GetNameId()] = aId;

   if (solutionExists)
   {
      if (timeToEntry > timeToExit)
      {
         // Set up the initial interval with the current time as the entry time, and evaluate on exit.
         EclipseInterval interval;
         interval.mStart = aSimTime;
         mEclipseMap[aSpaceMover.GetPlatform()->GetNameId()].push_back(interval);
         eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cEXIT, aId, *this, &aSpaceMover);
         eventPtr->SetTime(aSimTime + timeToExit);
      }
      else
      {
         eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cENTRY, aId, *this, &aSpaceMover);
         eventPtr->SetTime(aSimTime + timeToEntry);
      }
   }
   else
   {
      // re-evaluate after one quarter orbit.
      double timeToEval =
         0.25 / (aSpaceMover.GetPropagator().GetOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cTWO_PI);
      eventPtr = ut::make_unique<EclipseEvent>(EclipseEvent::cEVALUATE, aId, *this, &aSpaceMover);
      eventPtr->SetTime(aSimTime + timeToEval);
   }

   GetSimulation().AddEvent(std::move(eventPtr));
}

void WsfEclipseReport::OrbitalManeuverCompleted(double                 aSimTime,
                                                WsfSpaceMoverBase*     aSpaceMoverPtr,
                                                const WsfOrbitalEvent& aManeuver)
{
   InitiateEclipseEvent(aSimTime, aManeuver.GetUniqueId(), *aSpaceMoverPtr);
}

void WsfConfigureEclipseReport::SimulationCreated(WsfSimulation& aSimulation)
{
   // Only need to create the simulation extension if the scenario extension was configured.
   if (mReportAll || (mEclipseMap.size() > 0))
   {
      aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfEclipseReport>(*this));
   }
}

WsfEclipseReport::EclipseEvent::EclipseEvent(Type aType, size_t aId, WsfEclipseReport& aReport, WsfSpaceMoverBase* aSpaceMoverPtr)
   : WsfEvent()
   , mType(aType)
   , mId(aId)
   , mReport(aReport)
   , mSpaceMoverPtr(aSpaceMoverPtr)
   , mPlatformIndex(aSpaceMoverPtr->GetPlatform()->GetIndex())
{
}

WsfEvent::EventDisposition WsfEclipseReport::EclipseEvent::Execute()
{
   // If we have maneuvered, there can be an old event scheduling the eclipses from the previous orbit.
   if (GetSimulation()->PlatformExists(mPlatformIndex) &&
       (mId == mReport.mPlatformToCurrentEventIdMap[mSpaceMoverPtr->GetPlatform()->GetNameId()]))
   {
      mSpaceMoverPtr->Update(GetTime()); // In case the mover was not up-to-date.
      double timeToEntry    = 0.0;
      double timeToExit     = 0.0;
      bool   solutionExists = mSpaceMoverPtr->GetPropagator().GetEclipseTimes(timeToEntry, timeToExit);
      if (solutionExists)
      {
         if (mType == cENTRY)
         {
            // Set up a new interval with the start time being the entry time.
            EclipseInterval interval;
            interval.mStart = GetTime();
            mReport.mEclipseMap[mSpaceMoverPtr->GetPlatform()->GetName()].push_back(interval);
            // Switch to evaluate exit type after timeToExit.
            mType = cEXIT;
            SetTime(GetTime() + timeToExit);
         }
         else if (mType == cEXIT)
         {
            // Set the end time time to be the exit time.
            auto interval  = mReport.mEclipseMap[mSpaceMoverPtr->GetPlatform()->GetName()].rbegin();
            interval->mEnd = GetTime();
            // Switch to evaluate entry type after timeToEntry.
            mType = cENTRY;
            SetTime(GetTime() + timeToEntry);
         }
         else // cEVALUATE
         {
            // Switch to evaluate entry type after timeToEntry.
            // todo logic to handle case where we are already in shadow (rare).
            mType = cENTRY;
            SetTime(GetTime() + timeToEntry);
         }
      }
      else
      {
         mType = cEVALUATE;

         // Evaluate one quarter orbit later
         double timeToEval =
            0.25 /
            (mSpaceMoverPtr->GetPropagator().GetOrbitalState().GetOrbitalElements().GetMeanMotion() * UtMath::cTWO_PI);
         SetTime(GetTime() + timeToEval);
      }
      return WsfEvent::cRESCHEDULE;
   }
   else
   {
      return WsfEvent::cDELETE;
   }
}
