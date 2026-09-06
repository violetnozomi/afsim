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

#include "DetectionReport.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLexicalCast.hpp"
#include "UtStringUtil.hpp"

DetectionReport::DetectionReport(const Options& aOptions)
   : Report(Type::cDETECTION, aOptions),
     mSubReportType(DetectionReportType::cUNKNOWN),
     mDetecting(),
     mDetected()
{
}

bool DetectionReport::CreateRequiredData(std::string& aOutputDir)
{
   if (mSubReportType == DetectionReportType::cFIRSTDETECTIONS)
   {
      mRequiredData.insert(mRequiredData.end(), { "time", "sensor", "rcvr_lat",
                                                  "rcvr_lon", "rcvr_alt", "target_platform",
                                                  "target_lat", "target_lon", "target_alt"
                                                });

      for (const auto& option : mReportOptions)
      {
         if (option == "platform_type_of_detected_platform")
         {
            mRequiredData.emplace_back("target_type");
         }
         else if (option == "side_indicator")
         {
            mRequiredData.emplace_back("side");
         }
         /*else if (option == "range_of_detection")
         {
         mRequiredData.push_back("range");
         }*/
         else if (option == "azimuth_angle")
         {
            mRequiredData.emplace_back("rcvr->tgt_bearing");
         }
         else if (option == "elevation_angle")
         {
            mRequiredData.emplace_back("rcvr->tgt_elev");
         }
      }
   }
   else if (mSubReportType == DetectionReportType::cALLDETECTIONEVENTS)
   {

      if (mReportOptions.find("detailed_statistics") != mReportOptions.end())
      {
         mRequiredData.insert(mRequiredData.end(), { "time", "sensor", "rcvr_lat",
                                                     "rcvr_lon", "rcvr_alt", "target_platform",
                                                     "target_lat", "target_lon", "target_alt"
                                                   });
      }
      else if (mReportOptions.find("event_history") != mReportOptions.end())
      {
         mRequiredData.insert(mRequiredData.end(), { "time", "event", "sensor", "target_platform" });
      }
      else
      {
         //throw error
      }
   }
   return true;
}

bool DetectionReport::Filter(std::vector<std::string>& aLine)
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
   if (!(evtType == "SENSOR_DETECTION_ATTEMPT" ||
         evtType == "SENSOR_DETECTION_CHANGED"))
   {
      return true;
   }

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

   if (evtTime < mStartTime ||
       evtTime > mEndTime)
   {
      return true;
   }

   if (!mDetecting.empty())
   {
      size_t index = mEventToTagData[evtType]["platform"].mIndex;
      std::string platform = aLine[index];
      const auto& it = mDetecting.find(platform);
      if (it != mDetecting.end())
      {
         index = mEventToTagData[evtType]["sensor"].mIndex;
         std::string sensor = aLine[index];
         if (it->second.find(sensor) == it->second.end())
         {
            return true;
         }
      }
      else
      {
         return true;
      }
   }

   if (!mDetected.empty())
   {
      size_t index = mEventToTagData[evtType]["target_platform"].mIndex;
      if (mDetected.find(aLine[index]) == mDetected.end())
      {
         return true;
      }
   }

   if (mSubReportType == DetectionReportType::cACCESS)
   {
      if (evtType == "SENSOR_DETECTION_ATTEMPT")
      {
         return true;
      }
      return UpdateAccessReport(aLine, evtTime);
   }

   return false;
}

void DetectionReport::PrintReportHeader(std::ostream& aStream)
{
   std::vector<std::string> header;

   aStream << "Report:             " << Report::GetTypeStr(mReportType) << std::endl
           << "Report Name:        " << mReportName << std::endl
           << "Report Type:        " << GetSubReportTypeStr(mSubReportType) << std::endl
           << "Begin Report Time:  " << mStartTime << " " << mStartTimeUnits << std::endl
           << "End Report Time:    " << mEndTime << " " << mEndTimeUnits << std::endl
           << "Sim Date/Time:      " << FormatDateTime() << std::endl;

   aStream << "Detecting Sensors:";
   if (mDetecting.empty())
   {
      aStream << "  All Platforms" << std::endl;
   }
   else
   {
      size_t wdth = 2;
      for (auto& dt : mDetecting)
      {
         for (const auto& snr : dt.second)
         {
            aStream << std::setw(dt.first.size() + wdth) << dt.first;
            aStream << ':' << snr << std::endl;
            wdth = 20;
         }
      }
   }

   aStream << "Detected Platforms:";

   if (mDetected.empty())
   {
      aStream << " All Platforms" << std::endl;
   }
   else
   {
      size_t wdth = 1;
      for (auto& dtd : mDetected)
      {
         aStream << std::setw(dtd.size() + wdth) << dtd << std::endl;
         wdth = 20;
      }
   }

   aStream << std::endl;
   if (mDelimiterType == DelimiterType::cCOLUMN)
   {
      std::vector<size_t> numDashesVec;
      if (mUseMC)
      {
         aStream << std::setfill(' ') << std::setw(5) << "MC#";
      }

      if (mSubReportType == DetectionReportType::cACCESS)
      {
         aStream << std::setfill(' ') << std::setw(mMaxPlatformNameSize) << "Platform"
                 << std::setw(mMaxPlatformNameSize) << "Sensor"
                 << std::setw(mMaxPlatformNameSize) << "Target"
                 << std::setw(21) << "Start Time"
                 << std::setw(21) << "End Time"
                 << std::setw(mMaxTimeSize) << "Rel Start Time"
                 << std::setw(mMaxTimeSize) << "Rel End Time"
                 << std::setw(mMaxTimeSize) << "Duration";
      }
      else if (mSubReportType == DetectionReportType::cTOTALDETECTIONS)
      {
         aStream << std::setw(31) << "Platform"
                 << std::setw(29) << "Number of Platforms Detected";
         numDashesVec = { 5, 30, 28 };
      }
      else if (mSubReportType == DetectionReportType::cFIRSTDETECTIONS)
      {
         aStream << std::setfill(' ') << std::setw(12) << "Time"
                 << std::setw(31) << "Detecting Platform ID"
                 << std::setw(14) << "& Lat"
                 << std::setw(14) << "Lon"
                 << std::setw(10) << "Alt"
                 << std::setw(31) << "Detected Platform ID"
                 << std::setw(14) << "& Lat"
                 << std::setw(14) << "Lon"
                 << std::setw(10) << "Alt";
         numDashesVec = { 5, 11, 30, 13, 13, 9, 30, 13, 13, 9 };
         for (const auto& option : mReportOptions)
         {
            if (option == "platform_type_of_detected_platform")
            {
               aStream << std::setfill(' ') << std::setw(12) << "System Type";
               numDashesVec.push_back(11);
            }
            else if (option == "side_indicator")
            {
               aStream << std::setfill(' ') << std::setw(8) << "Side";
               numDashesVec.push_back(7);
            }
            /*else if (option.first == "range_of_detection")
            {
            aStream << "System Type" << PrintDelimiter();
            }*/
            else if (option == "azimuth_angle")
            {
               aStream << std::setfill(' ') << std::setw(11) << "Abso Az";
               numDashesVec.push_back(10);
            }
            else if (option == "elevation_angle")
            {
               aStream << std::setfill(' ') << std::setw(11) << "Abso El";
               numDashesVec.push_back(10);
            }
         }
      }
      else if (mSubReportType == DetectionReportType::cALLDETECTIONEVENTS)
      {
         if (mReportOptions.find("detailed_statistics") != mReportOptions.end())
         {
            header = { "Time", "Detecting Platform", "Detecting Sensor Name", "Lat", "Lon", "Alt", "Tgt Name", "Tgt Device", "Lat", "Lon", "Alt", "Tgt Beam", "Last Detect Action",
                       "Target Slant Range", "Min Range", "Max Range", "Tgt Speed", "Min Speed", "Max Speed", "Tgt Range Rate", "Min Range Rate", "Max Range Rate", "Tgt  Alt (AGL)", "Min Alt (AGL)", "Max Alt (AGL)",
                       "Tgt Alt (MSL)", "Min Alt (MSL)", "Max Alt (MSL)", "Tgt Radial Vel", "Min Radial Vel", "Max Radial Vel", "Tgt Az", "Min Az", "Max Az", "Tgt Elev", "Min Elev", "Max Elev", "Tgt Boresight Angle",
                       "Boresight Angle", "Beam Position", "IR Saturation", "IR Saturation Thresh", "Blind Speed", "Calculated SNR", "RCS", "RCS Aspect Angle", "RCS Az", "RCS Elev", "Signal Bandwidth", "Signal Pulse Length",
                       "Radar Constant", "Antenna Gain", "Scalloping Loss", "Scan Loss", "Coherent Gain", "Eclipsing Loss", "Normalized Filter Gain", "Atmospheric Loss", "Sensitivity Time Ctrl Gain", "Clutter Figure", "Clutter Factor",
                       "Moving Tgt Indicator Gain", "CNR", "Predetection SNR", "Final SNR", "Final SNR Thresh", "Jammed", "Grnd Clutter", "Low Freq", "High Freq", "Random Detection", "POD", "Verification Pulse", "POD Verify",
                       "Terrain Masking", "M", "N", "M out of N", "Time", "Outside Zone", "Cumm PD", "Cumm PD Thresh", "Prob of Acq Detection", "Prob of Acq Classification", "Prob of Acq Recognition", "Prob of Acq ID",
                       "Prob of Acq Correct ID", "Apparent IR Sig", "Spatial Freq", "Cd"
                     };
         }
         else if (mReportOptions.find("event_history") != mReportOptions.end())
         {
            header = { "Time", "Event", "Detecting Platform", "Target Platform", "Event Data" };
         }
         else
         {
            //throw UtException("***ERROR: Unknown options. Please ");
         }

         bool isFirst = true;
         for (const auto& hdr : header)
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
      }
      else if (mSubReportType == DetectionReportType::cDETECTEDBY)
      {
         aStream << "Time" << PrintDelimiter()
                 << "Detected Platforms";
      }
      aStream << std::endl;
      if (mSubReportType != DetectionReportType::cACCESS)
      {
         for (size_t i = 0; i < numDashesVec.size(); ++i)
         {
            size_t numDashes = numDashesVec.at(i);
            aStream << std::setfill('-') << std::setw(numDashes) << "";
            if (i < numDashesVec.size() - 1)
            {
               aStream << " ";
            }
         }
         aStream << std::endl;
      }
   }
   else
   {
      if (mUseMC)
      {
         aStream << "MC#" << PrintDelimiter();
      }

      if (mSubReportType == DetectionReportType::cACCESS)
      {
         aStream << "Platform" << PrintDelimiter()
                 << "Sensor" << PrintDelimiter()
                 << "Target_Platform" << PrintDelimiter()
                 << "Start Time" << PrintDelimiter()
                 << "End Time" << PrintDelimiter()
                 << "Rel Start Time" << PrintDelimiter()
                 << "Rel End Time" << PrintDelimiter()
                 << "Duration";
      }
      else if (mSubReportType == DetectionReportType::cTOTALDETECTIONS)
      {
         aStream << "Platform" << PrintDelimiter()
                 << "Number of Platforms Detected";
      }
      else if (mSubReportType == DetectionReportType::cFIRSTDETECTIONS)
      {
         header = { "Time", "Detecting Platform ID", " & Lat", "Lon", "Alt", "Detected Platform ID", "& Lat", "Lon", "Alt" };

         for (const auto& option : mReportOptions)
         {
            if (option == "platform_type_of_detected_platform")
            {
               header.emplace_back("System Type");
            }
            else if (option == "side_indicator")
            {
               header.emplace_back("Side");
            }
            /*else if (option == "range_of_detection")
            {
            aStream << "System Type" << PrintDelimiter();
            }*/
            else if (option == "azimuth_angle")
            {
               header.emplace_back("Abso Az");
            }
            else if (option == "elevation_angle")
            {
               header.emplace_back("Abso El");
            }
         }

         bool isFirst = true;
         for (const auto& hdr : header)
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
      }
      else if (mSubReportType == DetectionReportType::cALLDETECTIONEVENTS)
      {
         if (mReportOptions.find("detailed_statistics") != mReportOptions.end())
         {
            header = { "Time", "Detecting Platform", "Detecting Sensor Name", "Lat", "Lon", "Alt", "Tgt Name", "Tgt Device", "Lat", "Lon", "Alt", "Tgt Beam", "Last Detect Action",
                       "Target Slant Range", "Min Range", "Max Range", "Tgt Speed", "Min Speed", "Max Speed", "Tgt Range Rate", "Min Range Rate", "Max Range Rate", "Tgt  Alt (AGL)", "Min Alt (AGL)", "Max Alt (AGL)",
                       "Tgt Alt (MSL)", "Min Alt (MSL)", "Max Alt (MSL)", "Tgt Radial Vel", "Min Radial Vel", "Max Radial Vel", "Tgt Az", "Min Az", "Max Az", "Tgt Elev", "Min Elev", "Max Elev", "Tgt Boresight Angle",
                       "Boresight Angle", "Beam Position", "IR Saturation", "IR Saturation Thresh", "Blind Speed", "Calculated SNR", "RCS", "RCS Aspect Angle", "RCS Az", "RCS Elev", "Signal Bandwidth", "Signal Pulse Length",
                       "Radar Constant", "Antenna Gain", "Scalloping Loss", "Scan Loss", "Coherent Gain", "Eclipsing Loss", "Normalized Filter Gain", "Atmospheric Loss", "Sensitivity Time Ctrl Gain", "Clutter Figure", "Clutter Factor",
                       "Moving Tgt Indicator Gain", "CNR", "Predetection SNR", "Final SNR", "Final SNR Thresh", "Jammed", "Grnd Clutter", "Low Freq", "High Freq", "Random Detection", "POD", "Verification Pulse", "POD Verify",
                       "Terrain Masking", "M", "N", "M out of N", "Time", "Outside Zone", "Cumm PD", "Cumm PD Thresh", "Prob of Acq Detection", "Prob of Acq Classification", "Prob of Acq Recognition", "Prob of Acq ID",
                       "Prob of Acq Correct ID", "Apparent IR Sig", "Spatial Freq", "Cd"
                     };
         }
         else if (mReportOptions.find("event_history") != mReportOptions.end())
         {
            header = { "Time", "Event", "Detecting Platform", "Target Platform", "Event Data" };
         }
         else
         {
            //throw error
         }

         bool isFirst = true;
         for (const auto& hdr : header)
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
      }
      else if (mSubReportType == DetectionReportType::cDETECTEDBY)
      {
         aStream << "Time" << PrintDelimiter() << "Detected Platforms";
      }
      aStream << std::endl;
   }
}

void DetectionReport::SetSubReportType(const std::string& aTypeString)
{
   mSubReportType = DetectionReportType::cUNKNOWN;
   std::string typeStr(aTypeString);
   UtStringUtil::ToLower(typeStr);
   if (typeStr == "all_detection_events")
   {
      mSubReportType = DetectionReportType::cALLDETECTIONEVENTS;
   }
   else if (typeStr == "detected_by")
   {
      mSubReportType = DetectionReportType::cDETECTEDBY;
   }
   else if (typeStr == "first_detections")
   {
      mSubReportType = DetectionReportType::cFIRSTDETECTIONS;
   }
   else if (typeStr == "total_detections")
   {
      mSubReportType = DetectionReportType::cTOTALDETECTIONS;
   }
   else if (typeStr == "access")
   {
      mSubReportType = DetectionReportType::cACCESS;
   }
}

std::string DetectionReport::GetSubReportTypeStr(DetectionReportType aReportType)
{
   std::string srtStr("UNKNOWN");
   switch (aReportType)
   {
      case DetectionReportType::cACCESS:
         srtStr = "Access";
         break;
      case DetectionReportType::cALLDETECTIONEVENTS:
         srtStr = "All Detection Events";
         break;
      case DetectionReportType::cDETECTEDBY:
         srtStr = "Detected By";
         break;
      case DetectionReportType::cFIRSTDETECTIONS:
         srtStr = "First Detections";
         break;
      case DetectionReportType::cTOTALDETECTIONS:
         srtStr = "Total Detections";
         break;
      case DetectionReportType::cUNKNOWN:
      default:
         break;
   }
   return srtStr;
}

bool DetectionReport::ProcessInputConfiguration(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "report_type")
   {
      std::string subReportType;
      aInput.ReadValue(subReportType);
      SetSubReportType(subReportType);
   }
   else if (command == "options")
   {
      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         mReportOptions.emplace(command);
      }
   }
   else if (command == "detecting_sensors")
   {
      bool haveSensor = false;
      bool havePltfrm = false;
      std::string sensor;
      std::string platform;

      UtInputBlock block(aInput);
      while (block.ReadCommand(command))
      {
         if (command == "sensor")
         {
            aInput.ReadValue(sensor);
            haveSensor = true;
         }
         else if (command == "on_platform")
         {
            if (!haveSensor)
            {
               throw UtInput::BadValue(aInput, "Expected sensor input first.");

            }
            aInput.ReadValue(platform);
            havePltfrm = true;
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }

         if (havePltfrm && haveSensor)
         {
            auto it = mDetecting.find(platform);
            if (it != mDetecting.end())
            {
               if (it->second.find(sensor) == it->second.end())
               {
                  it->second.emplace(sensor);
               }
            }
            else
            {
               mDetecting[platform].emplace(sensor);
            }

            // reset
            havePltfrm = false;
            haveSensor = false;
         }
      }
   }
   else if (command == "detected_platforms")
   {
      UtInputBlock block(aInput);
      std::string pltfrmName;
      while (block.ReadCommand(pltfrmName))
      {
         mDetected.emplace(pltfrmName);
      }
   }
   else
   {
      myCommand = Report::ProcessInputConfiguration(aInput);
   }
   return myCommand;
}

void DetectionReport::PrintAccessReport(std::ostream & aStream)
{
   while (!mKeyInputs.empty())
   {
      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         if (mUseMC)
         {
            aStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
         }
         aStream << std::setw(mMaxPlatformNameSize) << mKeyInputs.top().mKey.mDetector
                 << std::setw(mMaxPlatformNameSize) << mKeyInputs.top().mKey.mSensorName
                 << std::setw(mMaxPlatformNameSize) << mKeyInputs.top().mKey.mDetectee
                 << std::setw(21) << FormatDateTime(mKeyInputs.top().mInterval.mStart)
                 << std::setw(21) << FormatDateTime(mKeyInputs.top().mInterval.mEnd)
                 << std::setw(mMaxTimeSize) << UtTime::ToString(mKeyInputs.top().mInterval.mStart, mTimeFormat)
                 << std::setw(mMaxTimeSize) << UtTime::ToString(mKeyInputs.top().mInterval.mEnd, mTimeFormat)
                 << std::setw(mMaxTimeSize) << UtTime::ToString(mKeyInputs.top().mInterval.mDuration, mTimeFormat) << std::endl;
      }
      else
      {
         if (mUseMC)
         {
            aStream << mCurrentMC << PrintDelimiter();
         }
         aStream << mKeyInputs.top().mKey.mDetector << PrintDelimiter()
                 << mKeyInputs.top().mKey.mSensorName << PrintDelimiter()
                 << mKeyInputs.top().mKey.mDetectee << PrintDelimiter()
                 << UtTime::ToString(mKeyInputs.top().mInterval.mStart, mTimeFormat) << PrintDelimiter()
                 << UtTime::ToString(mKeyInputs.top().mInterval.mEnd, mTimeFormat) << PrintDelimiter()
                 << FormatDateTime(mKeyInputs.top().mInterval.mStart) << PrintDelimiter()
                 << FormatDateTime(mKeyInputs.top().mInterval.mEnd) << PrintDelimiter()
                 << UtTime::ToString(mKeyInputs.top().mInterval.mDuration, mTimeFormat) << std::endl;
      }

      mKeyInputs.pop();
   }
}

//Update the Access Report Map
bool DetectionReport::UpdateAccessReport(std::vector<std::string>& aDataStrVec, double aEvtTime)
{
   const auto& evtIt = mEventToTagData.find("SENSOR_DETECTION_CHANGED");
   std::string platform = aDataStrVec[evtIt->second["platform"].mIndex];
   std::string sensor = aDataStrVec[evtIt->second["sensor"].mIndex];
   std::string target = aDataStrVec[evtIt->second["target_platform"].mIndex];
   auto detected = ut::lexical_cast<bool>(aDataStrVec[evtIt->second["detected"].mIndex]);

   DetectorDetecteeKey key(platform, target, sensor);
   DetectorDetecteeMap::iterator detectionIntervalListIter = mDetectorDetecteeMap.find(key);
   if (detectionIntervalListIter != mDetectorDetecteeMap.end())
   {
      if (detected)
      {
         return true; //A platform cannot have a re-detect a platform that is already being detected.
      }
      else
      {
         DetectionInterval& detectionInterval = detectionIntervalListIter->second;

         detectionInterval.mEnd = aEvtTime;
         detectionInterval.mDuration = detectionInterval.mEnd - detectionInterval.mStart;
         mKeyInputs.push(ChangedEvent(key, detectionInterval));
         mDetectorDetecteeMap.erase(detectionIntervalListIter);
      }
   }
   else
   {
      if (detected)
      {
         DetectionInterval detectionInterval(aEvtTime, mEndTime);

         mDetectorDetecteeMap[key] = detectionInterval;
      }
      else
      {
         DetectionInterval detectionInterval(mStartTime, aEvtTime);

         mKeyInputs.push(ChangedEvent(key, detectionInterval));
      }
   }
   mMaxPlatformNameSize = std::max({ platform.length() + 1, mMaxPlatformNameSize, sensor.length() + 1, target.length() + 1 },
   [](const size_t& s1, const std::size_t& s2) { return s1 < s2; });
   return false;
}

void DetectionReport::PrintReport(std::istream& aInStream,
                                  std::ostream& aOutStream)
{
   std::map<std::string, size_t> detectCounts;
   std::map<std::string, std::unordered_set<std::string>> firstDetections;

   std::string line;
   if (mSubReportType == DetectionReportType::cACCESS)
   {
      for (const auto& key : mDetectorDetecteeMap)
      {
         mKeyInputs.emplace(ChangedEvent(key.first, key.second));
      }

      PrintAccessReport(aOutStream);
      return;
   }
   
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
      bool isFirst = true;
      std::vector<std::string> dataStrVec;
      Report::Tokenize(line, dataStrVec);
      double evtTime = atof(dataStrVec[0].c_str());
      std::string evtType = dataStrVec[1];
      const auto& evtIt = mEventToTagData.find(evtType);

      if (mSubReportType == DetectionReportType::cTOTALDETECTIONS)
      {
         size_t index = evtIt->second["sensor"].mIndex;

         if (detectCounts.count(dataStrVec[index]) > 0)
         {
            ++detectCounts[dataStrVec[index]];
         }
         else
         {
            detectCounts[dataStrVec[index]] = 1;
         }
      }
      else if (mSubReportType == DetectionReportType::cFIRSTDETECTIONS)
      {
         isFirst = true;
         size_t detector = evtIt->second["sensor"].mIndex;
         size_t detected = evtIt->second["target_platform"].mIndex;
         const auto& it = firstDetections.find(dataStrVec[detector]);
         if (it != firstDetections.end())
         {
            if (it->second.find(dataStrVec[detected]) != it->second.end())
            {
               continue;
            }
         }

         firstDetections[dataStrVec[detector]].emplace(dataStrVec[detected]);
         if (mDelimiterType == DelimiterType::cCOLUMN)
         {
            size_t index = evtIt->second["sensor"].mIndex;
            if (mUseMC)
            {
               aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
            }
            aOutStream << " " << std::setw(8) << FormatTime(evtTime);

            FormatStringColumn(aOutStream, dataStrVec[index], 31);
            index = evtIt->second["rcvr_lat"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 14);
            index = evtIt->second["rcvr_lon"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 14);
            index = evtIt->second["rcvr_alt"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 10);
            index = evtIt->second["target_platform"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 31);
            index = evtIt->second["target_lat"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 14);
            index = evtIt->second["target_lon"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 14);
            index = evtIt->second["target_alt"].mIndex;
            FormatStringColumn(aOutStream, dataStrVec[index], 10);
            for (const auto& rqdDataItem : mRequiredData)
            {
               index = evtIt->second[rqdDataItem].mIndex;
               if (rqdDataItem == "target_type")
               {
                  FormatStringColumn(aOutStream, dataStrVec[index], 12);
               }
               else if (rqdDataItem == "side")
               {
                  FormatStringColumn(aOutStream, dataStrVec[index], 8);
               }
               /*else if (rqdDataItem == "range_of_detection")
               {
               mRequiredData.push_back("range");
               }*/
               else if (rqdDataItem == "rcvr->tgt_bearing")
               {
                  FormatStringColumn(aOutStream, dataStrVec[index], 11);
               }
               else if (rqdDataItem == "rcvr->tgt_elev")
               {
                  FormatStringColumn(aOutStream, dataStrVec[index], 11);
               }
            }
         }
         else
         {
            if (mUseMC)
            {
               aOutStream << mCurrentMC << PrintDelimiter();
            }
            for (const auto& rqdDataItem : mRequiredData)
            {
               if (isFirst)
               {
                  isFirst = false;
               }
               else
               {
                  aOutStream << PrintDelimiter();
               }
               size_t index = evtIt->second[rqdDataItem].mIndex;
               if (rqdDataItem == "time")
               {
                  aOutStream << FormatTime(evtTime);
               }
               else
               {
                  if (dataStrVec[index].empty())
                  {
                     aOutStream << "UNKNOWN";
                  }
                  else
                  {
                     aOutStream << dataStrVec[index];
                  }
               }
            }
         }

         aOutStream << std::endl;
      }
      else if (mSubReportType == DetectionReportType::cALLDETECTIONEVENTS)
      {
         isFirst = true;
         if (mUseMC)
         {
            aOutStream << mCurrentMC << PrintDelimiter();
         }
         for (const auto& rqdDataItem : mRequiredData)
         {
            size_t index = evtIt->second[rqdDataItem].mIndex;
            if (isFirst)
            {
               isFirst = false;
            }
            else
            {
               aOutStream << PrintDelimiter();
            }

            if (rqdDataItem == "time")
            {
               aOutStream << FormatTime(evtTime);
            }
            else
            {
               aOutStream << dataStrVec[index];
            }
         }
         aOutStream << std::endl;
      }
      else if (mSubReportType == DetectionReportType::cDETECTEDBY)
      {
         size_t index = evtIt->second["target_platform"].mIndex;
         if (mUseMC)
         {
            aOutStream << mCurrentMC << PrintDelimiter();
         }
         aOutStream << FormatTime(evtTime) << PrintDelimiter()
                    << dataStrVec[index] << std::endl;
      }
   }

   if (mSubReportType == DetectionReportType::cTOTALDETECTIONS)
   {
      size_t totalDetections = 0;
      if (mDelimiterType == DelimiterType::cCOLUMN)
      {
         for (auto cnt : detectCounts)
         {
            if (mUseMC)
            {
               aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
            }
            aOutStream << std::setfill(' ') << std::setw(31) << cnt.first;
            aOutStream << std::setfill(' ') << std::setw(29) << cnt.second;
            aOutStream << std::endl;
            totalDetections += cnt.second;
         }
         aOutStream << std::setfill('-') << std::setw(66) << std::endl;
         if (mUseMC)
         {
            aOutStream << std::setfill(' ') << std::setw(5) << mCurrentMC;
         }
         aOutStream << std::setfill(' ') << std::setw(31) << "Totals";
         aOutStream << std::setfill(' ') << std::setw(29) << totalDetections;
      }
      else
      {
         for (auto cnt : detectCounts)
         {
            if (mUseMC)
            {
               aOutStream << mCurrentMC << PrintDelimiter();
            }
            aOutStream << cnt.first << PrintDelimiter()
                       << cnt.second << std::endl;
            totalDetections += cnt.second;
         }
         aOutStream << std::setfill('-') << std::setw(61) << std::endl;
         if (mUseMC)
         {
            aOutStream << mCurrentMC << " ";
         }
         aOutStream << "Totals " << totalDetections << std::endl;
      }
   }
}

bool DetectionReport::DetectorDetecteeKey::operator<(const DetectorDetecteeKey& aRhs) const
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

bool DetectionReport::DetectionInterval::operator<(const DetectionInterval& aRhs) const
{
   if (aRhs.mStart != mStart)
   {
      return (aRhs.mStart < mStart);
   }
   else
   {
      return (aRhs.mEnd < mEnd);
   }
}

bool DetectionReport::ChangedEvent::operator<(const ChangedEvent & aRhs) const
{
   return mInterval < aRhs.mInterval;
}
