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

#include "EclipseReport.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLexicalCast.hpp"
#include "UtStringUtil.hpp"

//Constructor for Eclipse Report
EclipseReport::EclipseReport(const Options& aOptions)
   : Report(Type::cECLIPSE, aOptions),
     mPlatforms()
{
}

bool EclipseReport::CreateRequiredData(std::string& aOutputDir)
{
   //Need the Simulation start date and time found in SIMULATION_STARTING event
   auto it = mEventToTagData.find("SIMULATION_STARTING");
   if (it == mEventToTagData.end())
   {
      throw UtException("*****ERROR: Could not find the simulation start time. Enable the \"SIMULATION_STARTING\" event in the csv event output file.");
   }
   return true;
}

//Filters out unneeded data
bool EclipseReport::Filter(std::vector<std::string>& aLine)
{
   //Each event needs to have at least a time (aLine[0]), and event type (aLine[1])
   if (aLine.size() < 2)
   {
      std::string warning;
      for (const auto& line : aLine)
      {
         warning += line;
      }
      std::cout << "***WARNING: Cannot find event time or type in the line '" << warning << "'" << std::endl;
      return true;
   }

   std::string evtType = aLine[1];
   double evtTime = atof(aLine[0].c_str());

   //Only need ECLIPSE_ENTRY and ECLIPSE_EXIT events
   if (!(evtType == "ECLIPSE_ENTRY" ||
         evtType == "ECLIPSE_EXIT"))
   {
      return true;
   }

   //Filters out any event that does not have a header for information
   bool hasHeader = false;
   for (const auto& evT : mEvents)
   {
      if (evT == evtType)
      {
         hasHeader = true;
         break;
      }
   }

   if (!hasHeader)
   {
      return true;
   }

   //Filters out any event that does not occur between the given start and end time.
   if (evtTime < mStartTime ||
       evtTime > mEndTime)
   {
      return true;
   }

   //If no platform block is given, all platforms are accepted.
   if (!mPlatforms.empty())
   {
      //Filter out platforms that are not desired.
      size_t index = mEventToTagData[evtType]["platform"].mIndex;
      if (mPlatforms.find(aLine[index]) == mPlatforms.end())
      {
         return true;
      }
   }

   //Update Eclipse Report priority queue
   return UpdateEclipseReport(aLine, evtTime);
}

void EclipseReport::PrintReportHeader(std::ostream& aStream)
{
   aStream << "Report:             " << Report::GetTypeStr(mReportType) << std::endl
           << "Report Name:        " << mReportName << std::endl
           << "Begin Report Time:  " << mStartTime << " " << mStartTimeUnits << std::endl
           << "End Report Time:    " << mEndTime << " " << mEndTimeUnits << std::endl
           << "Sim Date/Time:      " << FormatDateTime() << std::endl;

   aStream << "Platforms:";

   if (mPlatforms.empty())
   {
      aStream << "          All Platforms" << std::endl;
   }
   else
   {
      size_t wdth = 1;
      for (auto& dtd : mPlatforms)
      {
         aStream << std::setw(dtd.size() + wdth) << dtd << std::endl;
         wdth = 20;
      }
   }

   aStream << std::endl;
   if (mDelimiterType == DelimiterType::cCOLUMN)
   {
      if (mUseMC)
      {
         aStream << std::setfill(' ') << std::setw(5) << "MC#";
      }
      aStream << std::setfill(' ') << std::setw(mMaxPlatformNameSize) << "Platform"
              << std::setw(21) << "Start Time"
              << std::setw(21) << "End Time"
              << std::setw(mMaxTimeSize) << "Rel Start Time"
              << std::setw(mMaxTimeSize) << "Rel End Time"
              << std::setw(mMaxTimeSize) << "Duration" << std::endl;
   }
   else
   {
      if (mUseMC)
      {
         aStream << "MC#" << PrintDelimiter();
      }
      aStream << "Platform" << PrintDelimiter()
              << "Start Time" << PrintDelimiter()
              << "End Time" << PrintDelimiter()
              << "Rel Start Time" << PrintDelimiter()
              << "Rel End Time" << PrintDelimiter()
              << "Duration" << std::endl;
   }
}

bool EclipseReport::ProcessInputConfiguration(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "platforms")
   {
      UtInputBlock block(aInput);
      std::string pltfrmName;
      while (block.ReadCommand(pltfrmName))
      {
         mPlatforms.emplace(pltfrmName);
      }
   }
   else
   {
      myCommand = Report::ProcessInputConfiguration(aInput);
   }
   return myCommand;
}

//Update the Eclipse Report priority queue and map.
bool EclipseReport::UpdateEclipseReport(std::vector<std::string>& aDataStrVec, double aEvtTime)
{
   std::string evt = aDataStrVec[1];
   const auto& evtIt = mEventToTagData.find(evt);
   std::string platform = aDataStrVec[evtIt->second["platform"].mIndex];

   auto eclipseIter = mEclipseMap.find(platform);

   //The platform currently has an event attached to it.
   if (eclipseIter != mEclipseMap.end())
   {
      if (evt == "ECLIPSE_ENTRY")
      {
         return true; //A platform cannot have two ECLIPSE_ENTRY events without a corresponding ECLIPSE_EXIT event between them.
      }
      else  //ECLIPSE_EXIT event
      {
         EclipseInterval& eclipseInterval = eclipseIter->second;

         eclipseInterval.mEnd = aEvtTime;
         eclipseInterval.mDuration = eclipseInterval.mEnd - eclipseInterval.mStart;

         EclipseEvent evt(platform, eclipseInterval);
         mEclipseQueue.push(evt);
         mEclipseMap.erase(eclipseIter);
      }
   }
   else
   {
      if (evt == "ECLIPSE_ENTRY")
      {
         EclipseInterval eclipseInterval(aEvtTime, mEndTime);
         mEclipseMap[platform] = eclipseInterval;
      }
      else //ECLIPSE_EXIT event
      {
         EclipseInterval eclipseInterval(mStartTime, aEvtTime);
         EclipseEvent evt(platform, eclipseInterval);
         mEclipseQueue.push(evt);
      }
   }

   mMaxPlatformNameSize = std::max(platform.length(), mMaxPlatformNameSize);
   return false;
}

void EclipseReport::PrintReport(std::istream& aInStream,
                                std::ostream& aOutStream)
{
   //Push all platforms that have ECLIPSE_ENTRY events without corresponding ECLIPSE_EXIT events to the queue.
   for (const auto& platform : mEclipseMap)
   {
      mEclipseQueue.push(EclipseEvent(platform.first, platform.second));
   }

   //Print all Eclipse events in the queue.
   while (!mEclipseQueue.empty())
   {
      EclipseEvent evt = mEclipseQueue.top();
      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         if (mUseMC)
         {
            aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
         }
         aOutStream << std::setfill(' ') << std::setw(mMaxPlatformNameSize) << evt.mPlatform
                    << std::setw(21) << FormatDateTime(evt.mInterval.mStart)
                    << std::setw(21) << FormatDateTime(evt.mInterval.mEnd)
                    << std::setw(mMaxTimeSize) << UtTime::ToString(evt.mInterval.mStart, mTimeFormat)
                    << std::setw(mMaxTimeSize) << UtTime::ToString(evt.mInterval.mEnd, mTimeFormat)
                    << std::setw(mMaxTimeSize) << UtTime::ToString(evt.mInterval.mDuration, mTimeFormat) << std::endl;
      }
      else
      {
         if (mUseMC)
         {
            aOutStream << mCurrentMC << PrintDelimiter();
         }
         aOutStream << evt.mPlatform << PrintDelimiter()
                    << FormatDateTime(evt.mInterval.mStart) << PrintDelimiter()
                    << FormatDateTime(evt.mInterval.mEnd) << PrintDelimiter()
                    << UtTime::ToString(evt.mInterval.mStart, mTimeFormat) << PrintDelimiter()
                    << UtTime::ToString(evt.mInterval.mEnd, mTimeFormat) << PrintDelimiter()
                    << UtTime::ToString(evt.mInterval.mDuration, mTimeFormat) << std::endl;
      }

      mEclipseQueue.pop();
   }
}

//Constructor for Eclipse Events
EclipseReport::EclipseEvent::EclipseEvent(const std::string& aPlatform,
                                          const EclipseInterval& aEclipseInterval)
   : mPlatform(aPlatform),
     mInterval(aEclipseInterval)
{
}

//Less than operator overload for Eclipse Events
bool EclipseReport::EclipseEvent::operator<(const EclipseEvent& aRhs) const
{
   return mInterval < aRhs.mInterval;
}

//Less than operator overload for Eclipse Intervals
bool EclipseReport::EclipseInterval::operator<(const EclipseInterval& aRhs) const
{
   if (aRhs.mStart != mStart)
   {
      return (aRhs.mStart < mStart);
   }

   return (aRhs.mEnd < mEnd);
}

//Constructor for Eclipse Interval
EclipseReport::EclipseInterval::EclipseInterval(double aStartTime,
                                                double aEndTime)
   : mStart(aStartTime),
     mEnd(aEndTime)
{
   mDuration = mEnd - mStart;
}

//Copy constructor for Eclipse Interval
EclipseReport::EclipseInterval::EclipseInterval(const EclipseInterval& aEclipseInterval)
   : mStart(aEclipseInterval.mStart),
     mEnd(aEclipseInterval.mEnd),
     mDuration(aEclipseInterval.mDuration)
{
}
