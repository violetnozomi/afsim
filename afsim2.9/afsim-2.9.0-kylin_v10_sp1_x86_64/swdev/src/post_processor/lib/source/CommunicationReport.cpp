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

#include "CommunicationReport.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLexicalCast.hpp"

namespace
{

std::string JoinKeyStrings(const std::map<std::string, int>& aMap, const std::string& aDelimiter = "")
{
   std::ostringstream out;
   for(auto it = std::begin(aMap); it != std::end(aMap); ++it)
   {
      if(it != std::begin(aMap))
      {
         out << aDelimiter;
      }
      out << it->first;
   }
   return out.str();
}

}

CommunicationReport::CommunicationReport(const Options& aOptions)
   : Report(Type::cCOMMUNICATION, aOptions)
{}

bool CommunicationReport::Filter(std::vector<std::string>& aLine)
{
   if (aLine.size() < 2)
   {
      std::string warning;
      for (const auto& line : aLine)
      {
         warning += line;
      }
      std::cout <<"***WARNING: Cannot find event time or type in the line '" << warning << "'" << std::endl;
      return true;
   }
   auto evtTime = ut::lexical_cast<double>(aLine[0]);
   std::string evtType = aLine[1];

   if (evtType != "MESSAGE_DISCARDED" &&
       evtType != "MESSAGE_UPDATED" &&
       evtType != "MESSAGE_QUEUED" &&
       evtType != "MESSAGE_TRANSMITTED" &&
       evtType != "MESSAGE_RECEIVED")
   {
      return true;
   }

   // Return true if the event type is not in the list of headers
   if(!report::util::Contains(mEvents, evtType))
   {
      return true;
   }

   if (evtTime < mStartTime || evtTime > mEndTime)
   {
      return true;
   }

   return false;
}

void CommunicationReport::PrintReportHeader(std::ostream& aStream)
{
   aStream << "Report: " << Report::GetTypeStr(mReportType) << std::endl
           << "Report Name: " << mReportName << std::endl
           << "Report Type: " << mSubReportType << std::endl
           << "Begin Report Time: " << mStartTime << " (" << mStartTimeUnits << ")" << std::endl
           << "End Report Time: " << mEndTime << " (" << mEndTimeUnits << ")" << std::endl;

   aStream << "Networks: ";
   if (mNetworks.empty())
   {
      aStream << "All Networks";
   }
   aStream << JoinKeyStrings(mNetworks, ", ") << std::endl;

   aStream << "Transmitting Platforms: ";
   if (mTransmitters.empty())
   {
      aStream << "All Transmitters";
   }
   aStream << JoinKeyStrings(mTransmitters, ", ") << std::endl;

   aStream << "Receiving Platforms: ";
   if (mReceivers.empty())
   {
      aStream << "All Receivers";
   }
   aStream << JoinKeyStrings(mReceivers, ", ") << std::endl;

   aStream << std::endl;

   std::ios init(nullptr);
   init.copyfmt(aStream);
   if (mSubReportType == "traffic_counts")
   {
      std::string headerOption("Network");
      if (mReportOptions.count("platforms") ||
          mReportOptions.count("time_interval"))
      {
         headerOption = "Transmitting Platform";
      }
      mHeader = { "MC#",  headerOption, "Messages Generated", "Messages Transmitted", "Messages Purged", "Messages Completed" };

      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         aStream << std::setw(5) << "MC#"
                 << std::setw(31) << headerOption
                 << std::setw(20)
                 << "Messages Generated "
                 << std::setw(16) << "Messages Transmitted "
                 << std::setw(30) << "Transmissions Completed To Receivers "
                 << std::setw(16) << "Messages Purged\n";

         std::vector<int> dashCount = { 5, 30, 18, 15, 29, 15 };
         for (auto dshCnt : dashCount)
         {
            aStream << std::setfill('-') << std::setw(dshCnt) << "";
            aStream << " ";
         }
      }
      else
      {
         aStream << "MC#" << PrintDelimiter()
                 << headerOption << PrintDelimiter()
                 << "Messages Generated" << PrintDelimiter()
                 << "Messages Transmitted" << PrintDelimiter()
                 << "Messages Completed" << PrintDelimiter()
                 << "Messages Purged" << "\n"
                 << std::setfill('-') << std::setw(101) << "";
      }
   }
   else if (mSubReportType == "connectivity")
   {
      /*if (mReportOption == "StatusChange")
      {
      mHeader = {  "MC#", "Time", "Network", "Transmitting Platform", "Receiving Platform", "Status"};
      isFirst = true;
      for (auto x : mHeader)
      {
      if (isFirst)
      {
      isFirst = false;
      if (mDelimiter == "Column")
      {
      aStream << " ";
      }
      }
      else
      {
      if (mDelimiter == "Column")
      {
      aStream << setw(x.second);
      }
      aStream << PrintDelimiter();
      }
      aStream << x.first;
      }
      if (mDelimiter == "Column")
      {
      aStream << "\n ";
      for (auto x : mHeader)
      {
      for (int i = 0; i < x.second; i++)
      {
      aStream << "-";
      }
      }
      }
      }
      else if (mReportOption == "TimeHistory")
      {
      mHeader = { { "MC#", 5 },{ "Time", 10 },{ "No. Selected Connected Links", 28 },{ "Jammed", 7 },{ "Low Power", 10 },{ "Blacked Out", 12 },{ "Not Active", 11 } ,{ "Not Aligned", 12 } ,{ "No GPS", 7 } };
      isFirst = true;
      for (auto x : mHeader)
      {
      if (isFirst)
      {
      isFirst = false;
      }
      else
      {
      aStream << PrintDelimiter();
      }
      aStream << x.first;
      }
      if (mDelimiter == "Column")
      {
      aStream << "\n ";
      for (auto x : mHeader)
      {
      for (int i = 0; i < x.second; i++)
      {
      aStream << "-";
      }
      }
      }
      }
      else
      {
      //throw error
      }*/
   }
   else
   {
      //throw error
   }
   aStream.copyfmt(init);
   aStream << "\n";
}

bool CommunicationReport::ProcessInputConfiguration(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "report_type")
   {
      aInput.ReadValue(mSubReportType);
   }
   else if (command == "options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (!mReportOptions.count(command))
         {
            mReportOptions[command] = 1;
         }
      }
   }
   else if (command == "transmitters")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mTransmitters[command] = 1;
         if (command == "all_transmitters")
         {
            mTransmitters.clear();
         }
      }
   }
   else if (command == "receivers")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReceivers[command] = 1;
         if (command == "all_receivers")
         {
            mReceivers.clear();
         }
      }
   }
   else if (command == "networks")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mNetworks[command] = 1;
         if (command == "all_networks")
         {
            mNetworks.clear();
         }
      }
   }
   else
   {
      myCommand = Report::ProcessInputConfiguration(aInput);
   }
   return myCommand;
}

void CommunicationReport::PrintReport(std::istream& aInStream,
                                      std::ostream& aOutStream)
{
   int totalGen = 0;
   int totalXmtd = 0;
   int totalComp = 0;
   int totalPurged = 0;
   int intervalGen = 0;
   int intervalXmtd = 0;
   int intervalComp = 0;
   int intervalPurged = 0;
   double endInterval = mStartTime + mInterval;

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

      if (mSubReportType == "traffic_counts")
      {
         size_t index = mEventToTagData[evtType]["platform"].mIndex;
         if (mReportOptions.count("networks"))
         {
            index = mEventToTagData[evtType]["system"].mIndex;
         }
         else if (mReportOptions.count("time_interval"))
         {
            if (evtTime >= endInterval)
            {
               if (mDelimiterType == DelimiterType::cCOLUMN)
               {
                  for (const auto& x : mExists)
                  {
                     if (mUseMC)
                     {
                        aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << PrintDelimiter();
                     }
                     aOutStream << std::setfill(' ') << std::setw(31) << x.first << PrintDelimiter();
                     int tempInt = (mMessagesGen.count(x.first) ? mMessagesGen[x.first] : 0);
                     aOutStream << std::setfill(' ') << std::setw(19) << tempInt;
                     tempInt = (mMessagesXmtd.count(x.first) ? mMessagesXmtd[x.first] : 0);
                     aOutStream << std::setfill(' ') << std::setw(16) << tempInt;
                     tempInt = (mMessagesComp.count(x.first) ? mMessagesComp[x.first] : 0);
                     aOutStream << std::setfill(' ') << std::setw(30) << tempInt;
                     tempInt = (mMessagesPurged.count(x.first) ? mMessagesPurged[x.first] : 0);
                     aOutStream << std::setfill(' ') << std::setw(16) << tempInt << std::endl;
                  }
                  aOutStream << std::setfill('-') << std::setw(118) << std::endl;
                  if (mUseMC)
                  {
                     aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << PrintDelimiter();
                  }
                  aOutStream << " Time " << std::setw(8) << FormatTime(endInterval) << PrintDelimiter()
                             << std::setw(14) << "Totals" << PrintDelimiter()
                             << std::setw(19) << intervalGen << PrintDelimiter()
                             << std::setw(16) << intervalXmtd << PrintDelimiter()
                             << std::setw(30) << intervalComp << PrintDelimiter()
                             << std::setw(16) << intervalPurged << std::endl << std::endl;
               }
               else
               {
                  for (const auto& exist : mExists)
                  {
                     if (mUseMC)
                     {
                        aOutStream << mCurrentMC << PrintDelimiter();
                     }
                     aOutStream << exist.first << PrintDelimiter();

                     if (mMessagesGen.count(exist.first))
                     {
                        aOutStream << mMessagesGen[exist.first];
                     }
                     else
                     {
                        aOutStream << 0;
                     }

                     aOutStream << PrintDelimiter();

                     if (mMessagesXmtd.count(exist.first))
                     {
                        aOutStream << mMessagesXmtd[exist.first];
                     }
                     else
                     {
                        aOutStream << 0;
                     }

                     aOutStream << PrintDelimiter();

                     if (mMessagesComp.count(exist.first))
                     {
                        aOutStream << mMessagesComp[exist.first];
                     }
                     else
                     {
                        aOutStream << 0;
                     }
                     aOutStream << PrintDelimiter();

                     if (mMessagesPurged.count(exist.first))
                     {
                        aOutStream << mMessagesPurged[exist.first];
                     }
                     else
                     {
                        aOutStream << 0;
                     }
                     aOutStream << std::endl;
                  }
                  aOutStream << std::setfill('-') << std::setw(101) << std::endl
                             << "Time " << FormatTime(endInterval)
                             << " Totals" << PrintDelimiter()
                             << intervalGen << PrintDelimiter()
                             << intervalXmtd << PrintDelimiter()
                             << intervalComp << PrintDelimiter()
                             << intervalPurged << std::endl << std::endl;
               }

               intervalGen = 0;
               intervalXmtd = 0;
               intervalComp = 0;
               intervalPurged = 0;

               mMessagesComp.clear();
               mMessagesGen.clear();
               mMessagesPurged.clear();
               mMessagesXmtd.clear();

               endInterval += mInterval;
            }
         }

         if (!mExists.count(dataStrVec[index]))
         {
            mExists[dataStrVec[index]] = 1;
         }

         if (evtType == "MESSAGE_DISCARDED")
         {
            auto it = mMessagesPurged.find(dataStrVec[index]);
            if (it != mMessagesPurged.end())
            {
               it->second++;
            }
            else
            {
               mMessagesPurged[dataStrVec[index]] = 1;
            }
            totalPurged++;
            intervalPurged++;
         }
         else if (evtType == "MESSAGE_QUEUED")
         {
            auto it = mMessagesGen.find(dataStrVec[index]);
            if (it != mMessagesGen.end())
            {
               it->second++;
            }
            else
            {
               mMessagesGen[dataStrVec[index]] = 1;
            }
            totalGen++;
            intervalGen++;
         }
         else if (evtType == "MESSAGE_TRANSMITTED")
         {
            auto it = mMessagesXmtd.find(dataStrVec[index]);
            if (it != mMessagesXmtd.end())
            {
               it->second++;
            }
            else
            {
               mMessagesXmtd[dataStrVec[index]] = 1;
            }
            totalXmtd++;
            intervalXmtd++;
         }
         else if (evtType == "MESSAGE_RECEIVED")
         {
            auto it = mMessagesComp.find(dataStrVec[index]);
            if (it != mMessagesComp.end())
            {
               it->second++;
            }
            else
            {
               mMessagesComp[dataStrVec[index]] = 1;
            }
            totalComp++;
            intervalComp++;
         }
      }
      else if (mSubReportType == "connectivity")
      {
         //TODO
      }
   }

   if (mSubReportType == "traffic_counts")
   {
      int tempInt;
      for (const auto& exist : mExists)
      {
         const auto& genIt = mMessagesGen.find(exist.first);
         const auto& xmitIt = mMessagesXmtd.find(exist.first);
         const auto& compIt = mMessagesComp.find(exist.first);
         const auto& purgedIt = mMessagesPurged.find(exist.first);
         if (mDelimiterType == DelimiterType::cCOLUMN)
         {
            if (mUseMC)
            {
               aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
            }
            aOutStream << std::setw(31) << exist.first;
            tempInt = (genIt != mMessagesGen.end() ? genIt->second : 0);
            aOutStream << std::setw(18) << tempInt;
            tempInt = (xmitIt != mMessagesXmtd.end() ? xmitIt->second : 0);
            aOutStream << std::setw(17) << tempInt;
            tempInt = (compIt != mMessagesComp.end() ? compIt->second : 0);
            aOutStream << std::setw(30) << tempInt;
            tempInt = (purgedIt != mMessagesPurged.end() ? purgedIt->second : 0);
            aOutStream << std::setw(16) << tempInt;
         }
         else
         {
            if (mUseMC)
            {
               aOutStream << mCurrentMC << PrintDelimiter();
            }
            aOutStream << exist.first << PrintDelimiter();

            if (genIt != mMessagesGen.end())
            {
               aOutStream << genIt->second;
            }
            else
            {
               aOutStream << 0;
            }
            aOutStream << PrintDelimiter();
            if (xmitIt != mMessagesXmtd.end())
            {
               aOutStream << xmitIt->second;
            }
            else
            {
               aOutStream << 0;
            }
            aOutStream << PrintDelimiter();

            if (compIt != mMessagesComp.end())
            {
               aOutStream << compIt->second;
            }
            else
            {
               aOutStream << 0;
            }

            aOutStream << PrintDelimiter();

            if (purgedIt != mMessagesPurged.end())
            {
               aOutStream << purgedIt->second;
            }
            else
            {
               aOutStream << 0;
            }
         }
         aOutStream << std::endl;
      }

      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         if (mReportOptions.count("time_interval"))
         {
            aOutStream << std::setfill('-') << std::setw(118) << std::endl;
            if (mUseMC)
            {
               aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC << PrintDelimiter();
            }
            aOutStream << " Time " << std::setw(8) << FormatTime(endInterval) << PrintDelimiter()
                       << std::setfill(' ') << std::setw(14) << "Totals" << PrintDelimiter()
                       << std::setfill(' ') << std::setw(18) << intervalGen
                       << PrintDelimiter()
                       << std::setfill(' ') << std::setw(17) << intervalXmtd
                       << PrintDelimiter()
                       << std::setfill(' ') << std::setw(30) << intervalComp
                       << PrintDelimiter()
                       << std::setfill(' ') << std::setw(16) << intervalPurged
                       << std::endl << std::endl;
         }

         aOutStream << std::setfill('-') << std::setw(118) << std::endl;
         if (mUseMC)
         {
            aOutStream << " " << std::setfill(' ') << std::setw(4) << mCurrentMC << PrintDelimiter();
         }
         aOutStream << std::setfill(' ') << std::setw(32) << "Totals "
                    << std::setw(17) << totalGen
                    << std::setw(17) << totalXmtd
                    << std::setw(30) << totalComp
                    << std::setw(16) << totalPurged;
      }
      else
      {
         if (mReportOptions.count("time_interval"))
         {
            aOutStream << std::setfill('-') << std::setw(101) << std::endl;
            if (mUseMC)
            {
               aOutStream << mCurrentMC << PrintDelimiter();
            }
            aOutStream << "Time " << FormatTime(endInterval) << PrintDelimiter()
                       << "Totals" << PrintDelimiter()
                       << intervalGen << PrintDelimiter()
                       << intervalXmtd << PrintDelimiter()
                       << intervalComp << PrintDelimiter()
                       << intervalPurged << std::endl << std::endl;
         }

         aOutStream << std::setfill('-') << std::setw(101) << std::endl;
         if (mUseMC)
         {
            aOutStream << mCurrentMC << PrintDelimiter();
         }
         aOutStream << "Totals" << PrintDelimiter()
                    << totalGen << PrintDelimiter()
                    << totalXmtd << PrintDelimiter()
                    << totalComp << PrintDelimiter()
                    << totalPurged;
      }
   }
}
