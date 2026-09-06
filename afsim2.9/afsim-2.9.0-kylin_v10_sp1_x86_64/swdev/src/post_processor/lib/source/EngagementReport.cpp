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

#include "EngagementReport.hpp"

#include <iomanip>
#include <string>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputFile.hpp"
#include "UtInputBlock.hpp"
#include "UtPath.hpp"

EngagementReport::EngagementReport(const Options& aOptions)
   : Report(Type::cENGAGEMENT, aOptions)
{
}

bool EngagementReport::CreateRequiredData(std::string& aOutputDir)
{
   if (mSubReportType == SubReport::GAH)
   {
      if (mReportOptions.find("FullActionText") == mReportOptions.end())
      {
         //TODO
      }
   }
   else if (mSubReportType == SubReport::WEX)
   {
      mRequiredData = { "time", "platform", "event", "Against Platforms",
                        "# of Wpns for Action", "Weapon Type", "# Available on Platform"
                      };
   }
   else if (mSubReportType == SubReport::TEH)
   {
      mRequiredData = { "time", "platform", "event", "target_platform", "sensor",
                        "track_id", "side", "track_type"
                      };
   }
   return true;
}

bool EngagementReport::Filter(std::vector<std::string>& aLine)
{
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

   if (mSubReportType == SubReport::WEX && (evtType != "WEAPON_FIRED" &&
                                            evtType != "WEAPON_FIRE_ABORTED" &&
                                            evtType != "WEAPON_RELOAD_STARTED" &&
                                            evtType != "PLATFORM_ADDED" &&
                                            evtType != "WEAPON_RELOAD_ENDED"))
   {
      return true;
   }
   else if (mSubReportType == SubReport::TEH && (evtType != "LOCAL_TRACK_DROPPED" &&
                                                 evtType != "LOCAL_TRACK_INITIATED" &&
                                                 evtType != "LOCAL_TRACK_UPDATED" &&
                                                 evtType != "SENSOR_REQUEST_CANCELED" &&
                                                 evtType != "SENSOR_REQUEST_INITIATED" &&
                                                 evtType != "SENSOR_REQUEST_UPDATED" &&
                                                 evtType != "SENSOR_TRACK_COASTED" &&
                                                 evtType != "SENSOR_TRACK_DROPPED" &&
                                                 evtType != "SENSOR_TRACK_INITIATED" &&
                                                 evtType != "PLATFORM_ADDED"))
   {
      return true;
   }

   if (evtType != "PLATFORM_ADDED" && (evtTime < mStartTime || evtTime > mEndTime))
   {
      return true;
   }

   if (!mTakenBy.empty() && evtType != "PLATFORM_ADDED")
   {
      size_t index = mEventToTagData[evtType]["platform"].mIndex;
      auto it = mTakenBy.find(aLine[index]);
      if (it == mTakenBy.end())
      {
         return true;
      }
   }

   if (!mTakenAgainst.empty() && evtType != "PLATFORM_ADDED")
   {
      auto it = mEventToTagData[evtType].find("target_platform");
      if (it != mEventToTagData[evtType].end())
      {
         size_t index = it->second.mIndex;
         auto taken = mTakenAgainst.find(aLine[index]);
         if (taken == mTakenAgainst.end())
         {
            return true;
         }
      }
   }
   return false;
}

void EngagementReport::PrintReportHeader(std::ostream& aStream)
{
   std::vector<std::string> header;
   std::vector<size_t> numDashes;
   aStream << "Report: " << Report::GetTypeStr(mReportType) << std::endl
           << "Report Name: " << mReportName << std::endl
           << "Report Type: " << GetSubreportStr() << std::endl
           << "Begin Report Time: " << mStartTime << "(" << mStartTimeUnits << ")" << std::endl
           << "End Report Time: " << mEndTime << "(" << mEndTimeUnits << ")" << std::endl << std::endl;

   if (mSubReportType == SubReport::GAH)
   {
      for (const auto& x : mReportOptions)
      {
         if (x == "PlatformName")
         {
            header.emplace_back("Platform Name");

         }
         else if (x == "ActingPlatformsIFF")
         {
            header.emplace_back("Acting IFF");
         }
         else if (x == "AgainstPlatformsIFF")
         {
            header.emplace_back("Against IFF");
         }
         else if (x == "ActingPlatformsPosition")
         {
            header.emplace_back("Acting Lat");
            header.emplace_back("Acting Lon");
            header.emplace_back("Acting Alt");
         }
         else if (x == "AgainsPlatformsPosition")
         {
            header.emplace_back("Against Lat");
            header.emplace_back("Against Lon");
            header.emplace_back("Against Alt");
         }
         else if (x == "MissileNumber")
         {
            header.emplace_back("Missile Number");
         }
         else if (x == "MissilePosition")
         {
            header.emplace_back("Missile Lat");
            header.emplace_back("Missile Lon");
            header.emplace_back("Missile Alt");
         }
         else if (x == "SensorName")
         {
            header.emplace_back("Sensor Name");
         }
         else if (x == "SlantRange")
         {
            header.emplace_back("Slant Range");
         }
         else if (x == "GroundRange")
         {
            header.emplace_back("Ground Range");
         }
         else if (x == "GroundRangeToDefendedAsset")
         {
            header.emplace_back("Ground Range To Defended Asset");
         }
         else if (x == "ElevationAngle")
         {
            header.emplace_back("El Angle");
         }
         else if (x == "AzimuthAngle")
         {
            header.emplace_back("Az Angle");
         }
         else if (x == "TargetBodyFrameElevation")
         {
            header.emplace_back("Tgt Frame El");
         }
         else if (x == "TargetBodyFrameAzimuth")
         {
            header.emplace_back("Tgt Frame Az");
         }
         else if (x == "WeaponName")
         {
            header.emplace_back("Weapon Name");
         }
         else if (x == "ShotNumber")
         {
            header.emplace_back("Shot Number");
         }
         else if (x == "TrackNumber")
         {
            header.emplace_back("Track Number");
         }
         else if (x == "TrackingSensorName")
         {
            header.emplace_back("Tracking Sensor Name");
         }
         else if (x == "TrackSupporterName")
         {
            header.emplace_back("Track Supporter Name");
         }
         else if (x == "ClosingRate")
         {
            header.emplace_back("Closing Rate");
         }
         else if (x == "TimeToGoforMissile")
         {
            header.emplace_back("Time To Go for Missile");
         }
         else if (x == "SlantRangeToGo")
         {
            header.emplace_back("Slant Range To Go");
         }
      }
   }
   else if (mSubReportType == SubReport::DFH)
   {
      header = { "MC#", "Time", "Acting Platform", "Against Platform" };
   }
   else if (mSubReportType == SubReport::WEX)
   {
      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         aStream << std::setfill(' ') << std::setw(5) << "MC#"
                 << std::setw(13) << "Time"
                 << std::setw(31) << "Acting Platforms"
                 << std::setw(15) << "Action"
                 << std::setw(31) << "Against Platforms"
                 << std::setw(21) << "# of Weapons for Action"
                 << std::setw(20) << "Weapon Type"
                 << std::setw(24) << "# Available on Platform";
         numDashes = { 6, 13, 31, 15, 31, 21, 20, 24 };
      }
      else
      {
         header = { "MC#", "Time", "Acting Platforms", "Action", "Against Platforms",
                    "# of Weapons for Action", "Weapon Type", "# Available on Platform"
                  };
      }
   }
   else if (mSubReportType == SubReport::TEH)
   {
      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         aStream << std::setfill(' ') << std::setw(5) << "MC#"
                 << std::setw(13) << "Time"
                 << std::setw(31) << "Acting Platforms"
                 << std::setw(24) << "Action"
                 << std::setw(31) << "Against Platforms"
                 << std::setw(20) << "Sensor Name"
                 << std::setw(31) << "Track Number"
                 << std::setw(9) << "Side"
                 << std::setw(20) << "Track Class";
         numDashes = { 6, 13, 31, 24, 31, 20, 31, 9, 20 };
      }
      else
      {
         header = { "MC#", "Time", "Acting Platforms", "Action", "Against Platforms",
                    "Sensor Name", "Track Number", "Side", "Track Class"
                  };
      }
   }

   if (mDelimiterType != DelimiterType::cCOLUMN)
   {
      bool isFirst = true;
      for (auto& hdr : header)
      {
         if (isFirst)
         {
            isFirst = false;
         }
         else
         {
            aStream << PrintDelimiter();
         }

         aStream << hdr;
      }
      aStream << std::endl << std::setfill('-') << std::setw(107) << "" << std::endl;
      aStream << std::setfill(' ');
   }
   else
   {
      aStream << std::endl;
      for (auto& dashes : numDashes)
      {
         aStream << std::setfill('-') << std::setw(dashes) << " ";
      }
      aStream << std::endl;
   }
}

void EngagementReport::PrintReport(std::istream& aInStream,
                                   std::ostream& aOutStream)
{
   std::map<std::string, std::string> platformType;

   std::string line;
   while (getline(aInStream, line))
   {
      if (!mFilteredDataLoc.empty())
      {
         std::iostream::pos_type filteredLoc = mFilteredDataLoc.front();
         if (aInStream.tellg() == filteredLoc)
         {
            mFilteredDataLoc.pop();
            continue;
         }
      }
      std::vector<std::string> dataStrVec;
      Report::Tokenize(line, dataStrVec);
      double evtTime = atof(dataStrVec[0].c_str());
      std::string evtType = dataStrVec[1];
      auto eventIt = mEventToTagData.find(evtType);
      if (eventIt == mEventToTagData.end())
      {
         continue;
      }
      if (evtType == "PLATFORM_ADDED")
      {
         size_t index = eventIt->second["platform"].mIndex;
         size_t typeIndex = eventIt->second["type"].mIndex;
         platformType.emplace(dataStrVec[index], dataStrVec[typeIndex]);
      }
      else if (mSubReportType == SubReport::WEX)
      {
         if (mDelimiterType == DelimiterType::cCOLUMN)
         {
            if (evtType == "WEAPON_FIRED")
            {
               if (mUseMC)
               {
                  aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << "  ";
               }
               aOutStream << std::setw(8) << FormatTime(evtTime)
                          << std::setw(31) << dataStrVec[eventIt->second["platform"].mIndex]
                          << std::setw(15) << "LAUNCHED";
               FormatStringColumn(aOutStream, dataStrVec[eventIt->second["target_platform"].mIndex], 31);
               aOutStream << std::setw(21) << 1;

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << std::setw(20) << it->second;
               }
               else
               {
                  aOutStream << std::setw(20) << "UNKNOWN";
               }
               aOutStream << std::setw(25) << "UNKNOWN" << std::endl;
            }
            else if (evtType == "WEAPON_RELOAD_STARTED" ||
                     evtType == "WEAPON_RELOAD_ENDED")
            {
               if (mUseMC)
               {
                  aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << "  ";
               }
               aOutStream << std::setw(8) << FormatTime(evtTime) << PrintDelimiter()
                          << std::setw(31) << dataStrVec[eventIt->second["platform"].mIndex]
                          << std::setw(15) << "RELOAD_" << (evtType == "WEAPON_RELOAD_STARTED" ? "STARTED" : "ENDED")
                          << std::setw(31) << "N/A"
                          << std::setw(21) << dataStrVec[eventIt->second["weapon_quantity"].mIndex];

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << std::setw(20) << it->second;
               }
               else
               {
                  aOutStream << std::setw(20) << "UNKNOWN";
               }

               index = eventIt->second["weapon_quantity"].mIndex;
               if (dataStrVec[index].empty())
               {
                  aOutStream << std::setw(25) << "UNKNOWN";
               }
               else
               {
                  aOutStream << std::setw(25) << dataStrVec[index];
               }
               aOutStream << std::endl;
            }
            else if (evtType == "WEAPON_FIRE_ABORTED")
            {
               if (mUseMC)
               {
                  aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << "  ";
               }
               aOutStream << std::setw(8) << FormatTime(evtTime) << PrintDelimiter()
                          << std::setw(31) << dataStrVec[eventIt->second["platform"].mIndex] << PrintDelimiter()
                          << std::setw(15) << "LNCH_REL" << PrintDelimiter()
                          << std::setw(31) << "N/A" << PrintDelimiter()
                          << std::setw(21) << dataStrVec[eventIt->second["weapon_qty"].mIndex] << PrintDelimiter();

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << std::setw(20) << it->second;
               }
               else
               {
                  aOutStream << std::setw(20) << "UNKNOWN";
               }
               aOutStream << std::setw(25) << "UNKNOWN" << std::endl;
            }
         }
         else
         {
            if (evtType == "WEAPON_FIRED")
            {
               if (mUseMC)
               {
                  aOutStream << mCurrentMC << PrintDelimiter();
               }
               aOutStream << FormatTime(evtTime) << PrintDelimiter()
                          << dataStrVec[eventIt->second["platform"].mIndex] << PrintDelimiter()
                          << "LAUNCHED" << PrintDelimiter()
                          << dataStrVec[eventIt->second["target_platform"].mIndex] << PrintDelimiter()
                          << 1 << PrintDelimiter();

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << it->second;
               }
               else
               {
                  aOutStream << "UNKNOWN";
               }
               aOutStream << PrintDelimiter() <<  "UNKNOWN" << std::endl;
            }
            else if (evtType == "WEAPON_RELOAD_STARTED" ||
                     evtType == "WEAPON_RELOAD_ENDED")
            {
               if (mUseMC)
               {
                  aOutStream << mCurrentMC << PrintDelimiter();
               }
               aOutStream << FormatTime(evtTime) << PrintDelimiter()
                          << dataStrVec[eventIt->second["platform"].mIndex] << PrintDelimiter()
                          << "RELOAD_" << (evtType == "WEAPON_RELOAD_STARTED" ? "STARTED" : "ENDED") << PrintDelimiter()
                          << "N/A" << PrintDelimiter()
                          << dataStrVec[eventIt->second["weapon_quantity"].mIndex] << PrintDelimiter();

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << it->second << PrintDelimiter();
               }
               else
               {
                  aOutStream << "UNKNOWN" << PrintDelimiter();
               }

               index = eventIt->second["weapon_quantity"].mIndex;
               if (dataStrVec[index].empty())
               {
                  aOutStream << "UNKNOWN";
               }
               else
               {
                  aOutStream << dataStrVec[index];
               }
               aOutStream << std::endl;
            }
            else if (evtType == "WEAPON_FIRE_ABORTED")
            {
               if (mUseMC)
               {
                  aOutStream << mCurrentMC << PrintDelimiter();
               }
               aOutStream << FormatTime(evtTime) << PrintDelimiter()
                          << dataStrVec[eventIt->second["platform"].mIndex] << PrintDelimiter()
                          << "LNCH_REL" << PrintDelimiter()
                          << "N/A" << PrintDelimiter()
                          << dataStrVec[eventIt->second["weapon_qty"].mIndex] << PrintDelimiter();

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << it->second;
               }
               else
               {
                  aOutStream << "UNKNOWN";
               }
               aOutStream << PrintDelimiter() << "UNKNOWN" << std::endl;
            }
         }
      }
      else if (mSubReportType == SubReport::TEH)
      {
         if (mDelimiterType == DelimiterType::cCOLUMN)
         {
            if (mUseMC)
            {
               aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
            }
            aOutStream << "  " << std::setw(8) << FormatTime(evtTime);
            FormatStringColumn(aOutStream, dataStrVec[eventIt->second["platform"].mIndex], 31);
            FormatStringColumn(aOutStream, evtType, 24);
            FormatStringColumn(aOutStream, dataStrVec[eventIt->second["target_platform"].mIndex], 31);
            if (evtType == "LOCAL_TRACK_DROPPED")
            {
               aOutStream << std::setw(20) << "N/A"
                          << std::setw(31) << "N/A";
            }
            else
            {
               FormatStringColumn(aOutStream, dataStrVec[eventIt->second["sensor"].mIndex], 20);
               FormatStringColumn(aOutStream, dataStrVec[eventIt->second["track_id"].mIndex], 31);
            }

            FormatStringColumn(aOutStream, dataStrVec[eventIt->second["side"].mIndex], 9);
            if (evtType == "SENSOR_TRACK_DROPPED" ||
                evtType == "SENSOR_REQUEST_CANCELED" ||
                evtType == "LOCAL_TRACK_DROPPED")
            {
               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << std::setw(20) << it->second;
               }
               else
               {
                  aOutStream << std::setw(20) << "UNKNOWN";
               }
            }
            else
            {
               FormatStringColumn(aOutStream, dataStrVec[eventIt->second["track_type"].mIndex], 20);
            }

            aOutStream << std::endl;

         }
         else
         {
            if (evtType == "LOCAL_TRACK_DROPPED")
            {
               if (mUseMC)
               {
                  aOutStream << mCurrentMC << PrintDelimiter();
               }
               aOutStream << FormatTime(evtTime) << PrintDelimiter()
                          << dataStrVec[eventIt->second["platform"].mIndex] << PrintDelimiter()
                          << evtType << PrintDelimiter()
                          << dataStrVec[eventIt->second["target_platform"].mIndex] << PrintDelimiter()
                          << "N/A" << PrintDelimiter()
                          << "N/A" << PrintDelimiter()
                          << dataStrVec[eventIt->second["side"].mIndex] << PrintDelimiter();

               size_t index = eventIt->second["platform"].mIndex;
               auto it = platformType.find(dataStrVec[index]);
               if (it != platformType.end())
               {
                  aOutStream << it->second << std::endl;
               }
               else
               {
                  aOutStream << "UNKNOWN" << std::endl;
               }
            }
            else
            {
               bool isFirst = true;
               if (mUseMC)
               {
                  aOutStream << mCurrentMC << PrintDelimiter();
               }
               for (const auto& x : mRequiredData)
               {
                  if (isFirst)
                  {
                     isFirst = false;
                  }
                  else
                  {
                     aOutStream << PrintDelimiter();
                  }
                  if ((evtType == "SENSOR_TRACK_DROPPED" ||
                       evtType == "SENSOR_REQUEST_CANCELED") && x == "track_type")
                  {
                     size_t index = eventIt->second["platform"].mIndex;
                     auto it = platformType.find(dataStrVec[index]);
                     if (it != platformType.end())
                     {
                        aOutStream << it->second;
                     }
                     else
                     {
                        aOutStream << "UNKNOWN";
                     }
                  }
                  else
                  {
                     if (dataStrVec[eventIt->second[x].mIndex].empty())
                     {
                        aOutStream << "UNKNOWN";
                     }
                     else if (x == "time")
                     {
                        aOutStream << FormatTime(evtTime);
                     }
                     else
                     {
                        aOutStream << dataStrVec[eventIt->second[x].mIndex];
                     }
                  }
               }
               aOutStream << std::endl;
            }
         }
      }
      else if (mSubReportType == SubReport::DFH)
      {
         //TODO
      }
      else if (mSubReportType == SubReport::GAH)
      {
         //TODO
      }
   }
}

bool EngagementReport::ProcessInputConfiguration(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "report_type")
   {
      aInput.ReadCommand(command);
      SetSubreport(command);
   }
   else if (command == "options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReportOptions.insert(command);
      }
   }
   else if (command == "acting_platforms")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mTakenBy.insert(command);
      }
   }
   else if (command == "against_platforms")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mTakenAgainst.insert(command);
      }
   }
   else if (command == "types_of_actions")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mActionTypes.insert(command);
      }
   }
   else if (command == "types_of_weapons")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mWeaponTypes.insert(command);
      }
   }
   else if (command == "areas_of_interest")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mAOI.insert(command);
      }
   }
   else if (command == "directed_energy_options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mDEOptions.insert(command);
      }
   }
   else if (command == "track_options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReportOptions.insert(command);
      }
   }
   else if (command == "measurement_options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReportOptions.insert(command);
      }
   }
   else if (command == "reference_set")
   {
      aInput.ReadValue(mReferenceSet);
   }
   else if (command == "originators")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReportOptions.insert(command);
      }
   }
   else
   {
      myCommand = Report::ProcessInputConfiguration(aInput);
   }
   return myCommand;
}

void EngagementReport::SetSubreport(const std::string& aSubReport)
{
   if (aSubReport == "geometric_action_history")
   {
      mSubReportType = SubReport::GAH;
   }
   else if (aSubReport == "damage_factor_history")
   {
      mSubReportType = SubReport::DFH;
   }
   else if (aSubReport == "track_event_history")
   {
      mSubReportType = SubReport::TEH;
   }
   else if (aSubReport == "weapon_expenditures")
   {
      mSubReportType = SubReport::WEX;
   }
   else
   {
      throw UtException("***ERROR: '" + aSubReport + "' is an invalid report_type.");
   }
}

std::string EngagementReport::GetSubreportStr(SubReport & aSubReport)
{
   switch (aSubReport)
   {
      case SubReport::DFH:
         return "Damage Factor History";
      case SubReport::GAH:
         return "Geometric Action History";
      case SubReport::TEH:
         return "Track Event History";
      case SubReport::WEX:
         return "Weapon Expenditures";
      default:
         return "UNKNOWN";
   }
}
