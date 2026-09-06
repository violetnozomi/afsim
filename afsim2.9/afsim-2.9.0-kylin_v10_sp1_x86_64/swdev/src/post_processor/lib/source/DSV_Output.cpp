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

#include "DSV_Output.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "UtCast.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLatPos.hpp"
#include "UtLexicalCast.hpp"
#include "UtLonPos.hpp"
#include "UtPath.hpp"
#include "UtStringUtil.hpp"
#include "UtWallClock.hpp"

DSV_Output::DSV_Output(const Options& aOptions)
   : Report(Report::Type::cDSV, aOptions)
{
   AddEvent("BTREE_NODE_CHILDREN");
   AddEvent("BTREE_NODE_EXEC");
   AddEvent("COMM_BROKEN", "COMM_STATUS");
   AddEvent("COMM_NON_OPERATIONAL", "COMM_STATUS");
   AddEvent("COMM_OPERATIONAL", "COMM_STATUS");
   AddEvent("COMM_TURNED_OFF", "COMM_STATUS");
   AddEvent("COMM_TURNED_ON", "COMM_STATUS");
   AddEvent("COMM_FREQUENCY_CHANGED");
   AddEvent("COMMENT");
   AddEvent("CRASHED_INTO_GROUND");
   AddEvent("EXCHANGE_COMPLETED", "EXCHANGE_EVENT");
   AddEvent("EXCHANGE_NEGOTIATED", "EXCHANGE_EVENT");
   AddEvent("EXCHANGE_QUERIED", "EXCHANGE_EVENT");
   AddEvent("EXCHANGE_REQUEST_FAILED", "EXCHANGE_EVENT");
   AddEvent("EXECUTE_CALLBACK");
   AddEvent("FUEL_EVENT");
   AddEvent("LOCAL_TRACK_CORRELATION");
   AddEvent("LOCAL_TRACK_DECORRELATION", "LOCAL_TRACK_CORRELATION");
   AddEvent("LOCAL_TRACK_DROPPED", "LOCAL_TRACK_INITDROPUPDATE");
   AddEvent("LOCAL_TRACK_INITIATED", "LOCAL_TRACK_INITDROPUPDATE");
   AddEvent("LOCAL_TRACK_UPDATED", "LOCAL_TRACK_INITDROPUPDATE");
   AddEvent("MESSAGE_DELIVERY_ATTEMPT");
   AddEvent("MESSAGE_DISCARDED", "MESSAGE_XMTD_RCVD_QUEUED_DISCARDED");
   AddEvent("MESSAGE_QUEUED", "MESSAGE_XMTD_RCVD_QUEUED_DISCARDED");
   AddEvent("MESSAGE_RECEIVED", "MESSAGE_XMTD_RCVD_QUEUED_DISCARDED");
   AddEvent("MESSAGE_TRANSMITTED", "MESSAGE_XMTD_RCVD_QUEUED_DISCARDED");
   AddEvent("MESSAGE_TRANSMIT_ENDED");
   AddEvent("MESSAGE_TRANSMITTED_HEARTBEAT");
   AddEvent("MESSAGE_UPDATED");
   AddEvent("MOVER_BROKEN", "MOVER_STATUS");
   AddEvent("MOVER_BURNED_OUT", "MOVER_STATUS");
   AddEvent("MOVER_NON_OPERATIONAL", "MOVER_STATUS");
   AddEvent("MOVER_OPERATIONAL", "MOVER_STATUS");
   AddEvent("MOVER_STAGED", "MOVER_STATUS");
   AddEvent("MOVER_TURNED_OFF", "MOVER_STATUS");
   AddEvent("MOVER_TURNED_ON", "MOVER_STATUS");
   AddEvent("NAVIGATION_STATUS_CHANGED");
   AddEvent("OPERATING_LEVEL_CHANGED");
   AddEvent("PLATFORM_ADDED", "PLATFORM_STATUS");
   AddEvent("PLATFORM_APPEARANCE_CHANGED");
   AddEvent("PLATFORM_BROKEN", "PLATFORM_STATUS");
   AddEvent("PLATFORM_CAPABILITY_CHANGED");
   AddEvent("PLATFORM_DELETED", "PLATFORM_STATUS");
   AddEvent("PLATFORM_INITIALIZED", "PLATFORM_STATUS");
   AddEvent("PLATFORM_OMITTED", "PLATFORM_STATUS");
   AddEvent("PROCESSOR_BROKEN", "PROCESSOR_STATUS");
   AddEvent("PROCESSOR_NON_OPERATIONAL", "PROCESSOR_STATUS");
   AddEvent("PROCESSOR_OPERATIONAL", "PROCESSOR_STATUS");
   AddEvent("PROCESSOR_TURNED_OFF", "PROCESSOR_STATUS");
   AddEvent("PROCESSOR_TURNED_ON", "PROCESSOR_STATUS");
   AddEvent("SENSOR_BROKEN", "SENSOR_STATUS");
   AddEvent("SENSOR_DETECTION_ATTEMPT", "SENSOR_DETECTION");
   AddEvent("SENSOR_DETECTION_CHANGED", "SENSOR_DETECTION");
   AddEvent("SENSOR_FREQUENCY_CHANGED");
   AddEvent("SENSOR_MODE_ACTIVATED", "SENSOR_MODE_STATUS");
   AddEvent("SENSOR_MODE_DEACTIVATED", "SENSOR_MODE_STATUS");
   AddEvent("SENSOR_NON_OPERATIONAL", "SENSOR_STATUS");
   AddEvent("SENSOR_OPERATIONAL", "SENSOR_STATUS");
   AddEvent("SENSOR_REQUEST_CANCELED", "SENSOR_REQUEST_STATUS");
   AddEvent("SENSOR_REQUEST_INITIATED", "SENSOR_REQUEST_STATUS");
   AddEvent("SENSOR_REQUEST_UPDATED", "SENSOR_REQUEST_STATUS");
   AddEvent("SENSOR_TRACK_COASTED", "SENSOR_TRACK_STATUS");
   AddEvent("SENSOR_TRACK_DROPPED", "SENSOR_TRACK_STATUS");
   AddEvent("SENSOR_TRACK_INITIATED", "SENSOR_TRACK_STATUS");
   AddEvent("SENSOR_TRACK_UPDATED", "SENSOR_TRACK_STATUS");
   AddEvent("SENSOR_TURNED_OFF", "SENSOR_STATUS");
   AddEvent("SENSOR_TURNED_ON", "SENSOR_STATUS");
   AddEvent("STATE_ENTRY", "STATE_STATUS");
   AddEvent("STATE_EXIT", "STATE_STATUS");
   AddEvent("TANKING_EVENT");
   AddEvent("TASK_ASSIGNED", "TASK_STATUS");
   AddEvent("TASK_CANCELED", "TASK_STATUS");
   AddEvent("TASK_COMPLETED", "TASK_STATUS");
   AddEvent("TEAM_NAME_DEFINITION");

   // Weapon Events
   AddEvent("DIRECTED_ENERGY_WEAPON_ABORT_SHOT", "DIRECTED_ENERGY_WEAPON_SHOT");
   AddEvent("DIRECTED_ENERGY_WEAPON_BEGIN_SHOT", "DIRECTED_ENERGY_WEAPON_SHOT");
   AddEvent("DIRECTED_ENERGY_WEAPON_COOLDOWN_COMPLETE");
   AddEvent("DIRECTED_ENERGY_WEAPON_UPDATE_SHOT", "DIRECTED_ENERGY_WEAPON_SHOT");
   AddEvent("DIRECTED_ENERGY_WEAPON_END_SHOT", "DIRECTED_ENERGY_WEAPON_SHOT");
   AddEvent("IMAGE_CREATED");
   AddEvent("IMPLICIT_WEAPON_BEGIN_ENGAGEMENT", "IMPLICIT_WEAPON_ENGAGEMENT");
   AddEvent("IMPLICIT_WEAPON_END_ENGAGEMENT", "IMPLICIT_WEAPON_ENGAGEMENT");
   AddEvent("JAMMING_ATTEMPT");
   AddEvent("JAMMING_REQUEST_CANCELED", "JAMMING_REQUEST");
   AddEvent("JAMMING_REQUEST_INITIATED", "JAMMING_REQUEST");
   AddEvent("JAMMING_REQUEST_UPDATED", "JAMMING_REQUEST");
   AddEvent("MOVER_GUIDANCE_PHASE_CHANGED");
   AddEvent("PLATFORM_KILLED");
   AddEvent("WEAPON_FIRE_ABORTED", "WEAPON_FIRE_STATUS");
   AddEvent("WEAPON_FIRE_REQUESTED", "WEAPON_FIRE_STATUS");
   AddEvent("WEAPON_FIRED");
   AddEvent("WEAPON_HIT", "WEAPON_HITMISSKILL");
   AddEvent("WEAPON_KILLED", "WEAPON_HITMISSKILL");
   AddEvent("WEAPON_MISSED", "WEAPON_HITMISSKILL");
   AddEvent("WEAPON_MODE_ACTIVATED", "WEAPON_MODE_STATUS");
   AddEvent("WEAPON_MODE_DEACTIVATED", "WEAPON_MODE_STATUS");
   AddEvent("WEAPON_NON_OPERATIONAL", "WEAPON_STATUS");
   AddEvent("WEAPON_OPERATIONAL", "WEAPON_STATUS");
   AddEvent("WEAPON_RELOAD_ENDED", "WEAPON_RELOAD_STATUS");
   AddEvent("WEAPON_RELOAD_STARTED", "WEAPON_RELOAD_STATUS");
   AddEvent("WEAPON_SELECTED");
   AddEvent("WEAPON_TERMINATED", "WEAPON_TERMINATED");
   AddEvent("WEAPON_TURNED_OFF", "WEAPON_STATUS");
   AddEvent("WEAPON_TURNED_ON", "WEAPON_STATUS");

   // Cyber Events
   AddEvent("CYBER_ATTACK_DETECTED");
   AddEvent("CYBER_ATTACK_FAILED");
   AddEvent("CYBER_ATTACK_INITIATED");
   AddEvent("CYBER_ATTACK_RECOVERY");
   AddEvent("CYBER_ATTACK_SUCCEEDED");
   AddEvent("CYBER_SCAN_DETECTED");
   AddEvent("CYBER_SCAN_FAILED");
   AddEvent("CYBER_SCAN_INITIATED");
   AddEvent("CYBER_SCAN_SUCCEEDED");

   // Battle Manager Events
   AddEvent("BM_AI_ADD_BEHAVIOR");
   AddEvent("BM_AI_EGRESS");
   AddEvent("BM_AI_TAKE_ACTION");
   AddEvent("BM_ALLOCATE");
   AddEvent("BM_ASSIGNMENT_CANCEL");
   AddEvent("BM_ASSIGNMENT_CREATION");
   AddEvent("BM_ASSIGNMENT_OVERRIDE");
   AddEvent("BM_ASSIGNMENT_DOCTRINE_OVERRIDE");
   AddEvent("BM_ASSIGNMENT_STATUS");
   AddEvent("BM_CANTCO_ASSIGNMENT");
   AddEvent("BM_SENSOR_CUE");
   AddEvent("BM_SENSOR_DROP_CUE");
   AddEvent("BM_SENSOR_TRACKING");
}

void DSV_Output::AddEvent(const std::string& aEventName,
                          const std::string& aBaseFilename/* = ""*/)
{
   EventData& data = mEventMap[aEventName];
   data.mBaseFileName = aEventName;
   if (!aBaseFilename.empty())
   {
      data.mBaseFileName = aBaseFilename;
   }
}

bool DSV_Output::CreateRequiredData(std::string& aOutputDir)
{
   mFileNames.clear();
   mFileStreams.clear();
   mEventTagsByIndex.clear();
   for (auto& ed : mEventMap)
   {
      ed.second.mSetIndex = false;
      ed.second.mIndex = 0;
   }

   bool ok = true;
   int index = 0;
   for (const auto& evt : mEvents)
   {
      auto it = mEventMap.find(evt);

      // If it doesn't exist go ahead and create a file for it and issue a warning
      if (it == mEventMap.end())
      {
         std::cout << "***WARNING: Event " << evt << " is unknown event type." << std::endl;
         AddEvent(evt, evt);

         it = mEventMap.find(evt);
         if (it == mEventMap.end())
         {
            std::cout << "***WARNING: Event " << evt << " not automatically added." << std::endl;
            continue;
         }
      }

      // if the event ostream index has not been set
      if (!it->second.mSetIndex)
      {
         it->second.mIndex = ut::safe_cast<size_t>(index);
         it->second.mSetIndex = true;
         for (auto& i : mEventMap)
         {
            if (i.second.mBaseFileName == it->second.mBaseFileName)
            {
               i.second.mIndex = ut::safe_cast<size_t>(index);
               i.second.mSetIndex = true;
            }
         }
         mFileStreams.emplace_back(ut::make_unique<std::ofstream>());
         mFileNames.emplace_back("");
         ++index;
      }

      if (!StreamIsOpen(it->second.mIndex))
      {
         std::string file = aOutputDir + "/" + it->second.mBaseFileName + ".csv";
         ok &= OpenFile(file, it->second.mIndex);
      }

      // sorts headers by index instead of header name
      auto indexIt = mEventTagsByIndex.find(evt);
      if (indexIt == mEventTagsByIndex.end())
      {
         for (const auto& hdr : mEventToTagData[evt])
         {
            mEventTagsByIndex[evt][hdr.second.mIndex] = hdr.first;
         }
      }
   }
   return ok;
}

bool DSV_Output::Filter(std::vector<std::string>& aLine)
{
   if (aLine.size() < 2)
   {
      std::cout << "***WARNING: Cannot find event time and/or type in the line '" << UtStringUtil::Join("", aLine) << "'" << std::endl;
      return true;
   }

   auto evtTime = ut::lexical_cast<double>(aLine[0]);
   if (evtTime < mStartTime || evtTime > mEndTime)
   {
      return true;
   }

   // Update the max count
   auto it = mCountHeader.find(aLine[1]); // evtType = aLine[1]
   if (it != mCountHeader.end())
   {
      UpdateMaxCount(aLine);
   }
   return false;
}

bool DSV_Output::OpenFile(const std::string& aFileName,
                          size_t             aIndex)
{
   bool ok = true;
   if (mFileNames.size() <= aIndex)
   {
      mFileStreams.emplace_back(ut::make_unique<std::ofstream>());
      mFileNames.emplace_back("");
   }

   mFileNames[aIndex] = aFileName;
   if (StreamIsOpen(aIndex))
   {
      mFileStreams[aIndex]->close();
   }

   if (!mFileNames[aIndex].empty())
   {
      mFileStreams[aIndex]->open(aFileName);
      if (mFileStreams[aIndex]->fail())
      {
         std::cout << "***ERROR: Unable to open event output file '" << aFileName << "' aIndex: " << aIndex << std::endl;
         ok = false;
      }
   }
   return ok;
}

void DSV_Output::PrintReport(std::istream& aInStream,
                             std::ostream& aOutStream)
{
   std::string line;
   while (getline(aInStream, line))
   {
      if (!mFilteredDataLoc.empty())
      {
         if (aInStream.tellg() == mFilteredDataLoc.front())
         {
            mFilteredDataLoc.pop();
            continue;
         }
      }
      std::vector<std::string> dataStrVec;
      Report::Tokenize(line, dataStrVec);
      std::string evtType = dataStrVec[1];
      PrintData(StreamRef(mEventMap[evtType].mIndex), dataStrVec);
   }
}

void DSV_Output::PrintData(std::ostream&            aStream,
                           std::vector<std::string> aEventVec)
{
   std::string evtType = aEventVec[1];
   auto cntIt = mCountHeader.find(evtType);
   for (const auto& hdr : mEventTagsByIndex[evtType])
   {
      if (cntIt != mCountHeader.end())
      {
         Header::DataTag tag = ParseHeaderTag(hdr.second);
         if (tag.countType == Header::CountType::cINDEX)
         {
            continue;
         }
      }

      size_t dataIndex = hdr.first;

      if (dataIndex > 0)
      {
         aStream << PrintDelimiter();
      }

      if (dataIndex < aEventVec.size())
      {
         double eventData;
         if (ut::conversion::try_lexical_convert(aEventVec[dataIndex], eventData))
         {
            switch (mEventToTagData[evtType][hdr.second].mType)
            {
            case Header::DataType::Double:
               aStream << std::setprecision(mPrecision)
                       << eventData;
               break;
            case Header::DataType::Time:
               aStream << FormatTime(eventData);
               break;
            case Header::DataType::Lat:
               aStream << UtLatPos(eventData, mLatLonFormat);
               break;
            case Header::DataType::Lon:
               aStream << UtLonPos(eventData, mLatLonFormat);
               break;
            case Header::DataType::Angle:
               aStream << FormatAngle(eventData);
               break;
            default:
               aStream << aEventVec[dataIndex];
               break;
            }
         }
         else
         {
            aStream << aEventVec[dataIndex];
         }
      }

      if (cntIt != mCountHeader.end())
      {
         if (cntIt->second.find(dataIndex) != cntIt->second.end())
         {
            PrintCount(aStream, aEventVec, dataIndex);
         }
      }
   }
   aStream << std::endl;
   aStream.flush();
}

void DSV_Output::PrintHeaderData(std::ostream&            aStream,
                                 std::vector<std::string> aEventVec)
{
   std::string evtType = aEventVec[1];
   auto it = mCountHeader.find(evtType);
   for (const auto& hdr : mEventTagsByIndex[evtType])
   {
      if (it != mCountHeader.end())
      {
         Header::DataTag tag = ParseHeaderTag(hdr.second);
         if (tag.countType == Header::CountType::cINDEX)
         {
            continue;
         }
      }

      size_t dataIndex = hdr.first;

      if (dataIndex > 0)
      {
         aStream << PrintDelimiter();
      }

      aStream << hdr.second;

      if (it != mCountHeader.end())
      {
         auto indexIt = it->second.find(dataIndex);
         if (indexIt != it->second.end())
         {
            PrintHeaderCount(aStream, aEventVec, dataIndex);
         }
      }
   }
   aStream << std::endl;
   aStream.flush();
}

void DSV_Output::PrintReportHeader(std::ostream& aStream)
{
   mHeaderPrinted.clear();

   for (const auto& evt : mEvents)
   {
      std::string baseName = mEventMap[evt].mBaseFileName;
      auto it = mHeaderPrinted.find(baseName);
      if (it == mHeaderPrinted.end())
      {
         size_t index = mEventMap[evt].mIndex;
         std::vector<std::string> eventVec = { "", evt };
         PrintHeaderData(StreamRef(index), eventVec);
         mHeaderPrinted[baseName] = true;
      }
   }
}

void DSV_Output::ProcessHeaders(std::istream& aStream)
{
   // Additional data clears from base class implementations
   mCountHeaderMax.clear();
   mCountHeader.clear();

   mEvents.clear();
   mEventToTagData.clear();

   // Extract the headers
   std::vector<std::string> headerStrings = ExtractHeaders(aStream);
   mEndOfHeaders = aStream.tellg();

   // Process the header lines
   std::vector<Header> headers;
   for (const auto& h : headerStrings)
   {
      auto eventHeader = ProcessHeaderLine(h);
      const std::string& evtName = eventHeader.GetName();
      headers.emplace_back(eventHeader);
      mEvents.emplace_back(evtName);

      size_t index = 0;
      size_t prevCountIndex = 0; // Addition from base class implementation
      for (const auto& tag : eventHeader.GetDataTags())
      {
         if (mEventToTagData[evtName].find(tag.name) == std::end(mEventToTagData[evtName]))
         {
            TagData td;
            td.mIndex = index;
            td.mType = tag.dataType;
            mEventToTagData[evtName][tag.name] = td;
         }

         // Additional functionality from base class implementation
         if (tag.countType == Header::CountType::cCOUNT)
         {
            mCountHeaderMax[evtName][index] = 1;
            prevCountIndex = index;
         }
         else if (tag.countType == Header::CountType::cINDEX) // is a string to be output for the previous count
         {
            size_t loc = tag.name.find_last_of('_');
            std::string header = tag.name.substr(0, loc + 1);
            mCountHeader[evtName][prevCountIndex].emplace_back(header);
         }

         ++index;
      }
   }
   // If there are no headers, throw an exception to catch upstream
   if (mEvents.empty())
   {
      throw UtException{ "Cannot generate DSV Report without CSV headers." };
   }
}

bool DSV_Output::StreamIsOpen(size_t aIndex) const
{
   return (aIndex < mFileStreams.size()) && (mFileStreams[aIndex]->is_open());
}

void DSV_Output::UpdateMaxCount(const std::vector<std::string>& aEvent)
{
   std::string evt = aEvent[1];
   auto it = mCountHeader.find(evt);
   if (it != mCountHeader.end())
   {
      for (const auto& index : it->second)
      {
         size_t ind = index.first;
         if (ind < aEvent.size())
         {
            size_t data;
            if (ut::conversion::try_lexical_convert(aEvent[ind], data) && data > mCountHeaderMax[evt][ind])
            {
               mCountHeaderMax[evt][ind] = data;
            }
         }
      }
   }
}

void DSV_Output::PrintHeaderCount(std::ostream&             aStream,
                                  std::vector<std::string>& aEvent,
                                  size_t&                   aIndex)
{
   std::string evt = aEvent[1];
   auto it = mCountHeader.find(evt);
   if (it != mCountHeader.end())
   {
      for (size_t obj = 0; obj < mCountHeaderMax[evt][aIndex]; ++obj)
      {
         for (const auto& header : it->second[aIndex])
         {
            aStream << PrintDelimiter() << header << (obj + 1);
         }
      }
   }
}

void DSV_Output::PrintCount(std::ostream&             aStream,
                            std::vector<std::string>& aEvent,
                            size_t&                   aIndex)
{
   std::string evt = aEvent[1];
   auto it = mCountHeader.find(evt);
   if (aIndex < aEvent.size() &&
       it != mCountHeader.end())
   {
      size_t existingObj;
      if (!ut::conversion::try_lexical_convert(aEvent[aIndex], existingObj))
      {
         existingObj = 0;
      }
      size_t neededObj = mCountHeaderMax[evt][aIndex] - existingObj;
      size_t numTags = it->second[aIndex].size();
      for (size_t obj = 0; obj < existingObj; ++obj)
      {
         for (size_t tagNum = 1; tagNum <= numTags; ++tagNum)
         {
            size_t dataIndx = aIndex + (obj * numTags) + tagNum;
            aStream << PrintDelimiter() << aEvent[dataIndx];
         }
      }

      for (size_t obj = 0; obj < neededObj; ++obj)
      {
         for (size_t i = 0; i < numTags; ++i)
         {
            aStream << PrintDelimiter();
         }
      }
   }
}
