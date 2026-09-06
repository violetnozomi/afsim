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

#include "Report.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "UtAngle.hpp"
#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLexicalCast.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"
#include "UtTime.hpp"
#include "UtWallClock.hpp"

// =================================================================================================
Report::Report(const Report::Type aReportType,
               const Options&     aOptions)
   : mReportType(aReportType),
     mReportName(aOptions.mReportName),
     mDataFile(aOptions.mDataFile),
     mOutputDir(aOptions.mOutputDir),
     mStartMC(aOptions.mStartMC),
     mEndMC(aOptions.mEndMC)
{
}

// =================================================================================================
void Report::PrepareExtension(std::string& aOutputDir)
{
   UtPath path(aOutputDir);
   path.MakeFullPath();                            // Ensure it is a fully qualified path name.
   aOutputDir = path.GetNormalizedPath();          // Normalized path does NOT contain a trailing '/' or '\'.
   if (path.Exists())
   {
      if (!path.IsDirectory())
      {
         throw UtException("***ERROR: '" + aOutputDir + "' already exists and is not a directory");
      }
   }
   else if (!path.Mkdir())
   {
      throw UtException("***ERROR: Unable to create directory '" + aOutputDir + "'");
   }
}

// =================================================================================================
bool Report::ProcessInputConfiguration(UtInput& aInput)
{
   bool myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "single_output_file")
   {
      aInput.ReadValue(mSingleOutputFile);
   }
   else if (command == "write_header_information")
   {
      aInput.ReadValue(mWriteHeader);
   }
   else if (command == "delimiter")
   {
      std::string delimiterStr;
      aInput.ReadValue(delimiterStr);
      SetDelimiter(delimiterStr);
   }
   else if (command == "length_units")
   {
      std::string lengthUnits;
      aInput.ReadValue(lengthUnits);
      if (lengthUnits == "feet")
      {
         mLengthUnits = LengthUnitType::cFEET;
      }
      else if (lengthUnits == "meters")
      {
         mLengthUnits = LengthUnitType::cMETERS;
      }
      else
      {
         throw UtInput::BadValue(aInput, "bad value for length_units, options are [ feet | meters ].");
      }
   }
   else if (command == "angle_units")
   {
      std::string angleUnits;
      aInput.ReadValue(angleUnits);
      if (angleUnits == "degrees")
      {
         mAngleUnits = AngleUnitType::cDEGREES;
      }
      else if (angleUnits == "radians")
      {
         mAngleUnits = AngleUnitType::cRADIANS;
      }
      else
      {
         throw UtInput::BadValue(aInput, "bad value for angle_units, options are [ degrees | radians ].");
      }
   }
   else if (command == "track_number_options")
   {
      std::string trackNumOpt;
      aInput.ReadValue(trackNumOpt);
      if (trackNumOpt == "numerical")
      {
         mTrackNum = TrackNumericalType::cNUMERICAL;
      }
      else if (trackNumOpt == "jtids")
      {
         mTrackNum = TrackNumericalType::cJTIDS;
      }
      else
      {
         throw UtInput::BadValue(aInput, "bad value for track_number_options, options are [ numerical | jtids ].");
      }
   }
   else if (command == "start_time")
   {
      aInput.ReadValueOfType(mStartTime, UtInput::cTIME);
   }
   else if (command == "end_time")
   {
      aInput.ReadValueOfType(mEndTime, UtInput::cTIME);
      std::string endTimeStr = UtTime::ToString(mEndTime, mTimeFormat);
      mMaxTimeSize = endTimeStr.length() + 1 < mMaxTimeSize ? mMaxTimeSize : endTimeStr.length() + 1;
   }
   else if (command == "interval")
   {
      aInput.ReadValueOfType(mInterval, UtInput::cTIME);
   }
   else if (command == "time_format")
   {
      std::string format;
      aInput.ReadValue(format);
      UtStringUtil::ToLower(format);
      int decimalFormat = 0;
      std::string::size_type decimalPos = format.find_last_of('.');
      if (decimalPos != std::string::npos)
      {
         int decimalCount = 0;
         std::string fractionFormat = format.substr(decimalPos + 1);
         if (!fractionFormat.empty())
         {
            if (fractionFormat.find_first_not_of("0123456789") == std::string::npos)
            {
               // New preferred format - hms.#
               decimalCount = UtStringUtil::ToInt(fractionFormat);
               aInput.ValueInClosedRange(decimalCount, 0, 15);
            }
            else
            {
               // Old format hms.nnn
               decimalCount = static_cast<int>(format.size() - (decimalPos + 1));
            }
         }
         decimalFormat = UtTime::FmtShowPoint | decimalCount;
         format = format.substr(0, decimalPos);
      }
      if (format == "s")
      {
         mTimeFormat = UtTime::FmtS | decimalFormat | UtTime::FmtNoSuffix;
      }
      else if (format == "m:s")
      {
         mTimeFormat = UtTime::FmtMS | decimalFormat | UtTime::FmtNoSuffix;
      }
      else if (format == "h:m:s")
      {
         mTimeFormat = UtTime::FmtHMS | decimalFormat | UtTime::FmtNoSuffix;
      }
      else
      {
         throw UtInput::BadValue(aInput, "time_format must be in form: [[h:]m:]s[.#] ");
      }
      std::string endTimeStr = UtTime::ToString(mEndTime, mTimeFormat);
      mMaxTimeSize = endTimeStr.length() + 1 < mMaxTimeSize ? mMaxTimeSize : endTimeStr.length() + 1;
   }
   else if (command == "lat_lon_format")
   {
      std::string format;
      aInput.ReadValue(format);
      UtStringUtil::ToLower(format);
      int decimalCount = -1;
      int defaultDecimalCount = 0;
      std::string::size_type decimalPos = format.find_last_of('.');
      if (decimalPos != std::string::npos)
      {
         std::string fractionFormat = format.substr(decimalPos + 1);
         if (!fractionFormat.empty())
         {
            if (fractionFormat.find_first_not_of("0123456789") == std::string::npos)
            {
               // New preferred format - <dms>.<number>
               decimalCount = UtStringUtil::ToInt(fractionFormat);
               aInput.ValueInClosedRange(decimalCount, 0, 15);
            }
            else
            {
               // Old format <dms>.nnn
               decimalCount = static_cast<int>(format.size() - (decimalPos + 1));
            }
         }
         format = format.substr(0, decimalPos);
      }

      if (format == "d")
      {
         defaultDecimalCount = 6;
         mLatLonFormat = UtAngle::FmtD;
      }
      else if (format == "d:m")
      {
         defaultDecimalCount = 4;
         mLatLonFormat = UtAngle::FmtDM;
      }
      else if (format == "d:m:s")
      {
         defaultDecimalCount = 2;
         mLatLonFormat = UtAngle::FmtDMS;
      }
      else
      {
         throw UtInput::BadValue(aInput, "lat_lon_format must be in form: d[:m[:s]][.#] ");
      }
      if (decimalCount == -1)
      {
         decimalCount = defaultDecimalCount;
      }
      mLatLonFormat += decimalCount;
   }
   else if (command == "precision")
   {
      aInput.ReadValue(mPrecision);
      aInput.ValueGreater(mPrecision, 0);
   }
   else if (command == "use_monte_carlo")
   {
      aInput.ReadValue(mUseMC);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool Report::PrintReport()
{
   bool isMC = mDataFile.find("%d") != std::string::npos;

   if (isMC &&
       (mReportName.find("%d") == std::string::npos) &&
       (mOutputDir.find("%d") == std::string::npos))
   {
      mReportName += "_%d";
   }

   // If DSV report, report_name is the output directory name appended to regular output directory
   std::string outputDir = mOutputDir;
   if (mReportType == Type::cDSV)
   {
      outputDir += ('/' + mReportName);
   }

   std::string outputFile = mOutputDir + "/" + mReportName + ".txt";

   UtCalendar calendar;
   calendar.SetCurrentDateAndTime();

   mCurrentMC = mStartMC;
   do
   {
      std::string outDir = SubstituteOutputFileVariables(outputDir, mCurrentMC, calendar);
      std::string inFile = SubstituteOutputFileVariables(mDataFile, mCurrentMC, calendar);
      std::string outFile = SubstituteOutputFileVariables(outputFile, mCurrentMC, calendar);

      std::ifstream inStream(inFile);
      if (inStream.fail())
      {
         break;
      }

      PrepareExtension(outDir);

      std::ofstream outStream(outFile, std::ofstream::out);
      if (outStream.fail())
      {
         throw UtException("***ERROR: Output file '" + outFile + "' could not be opened for writing");
      }

      std::cout << " Generating report '" << SubstituteOutputFileVariables(mReportName, mCurrentMC, calendar) << "'";
      if (isMC && mUseMC) { std::cout << " for run " << mCurrentMC; }
      std::cout << " ..." << std::endl;

      std::cout << "   Parsing headers ..." << std::endl;

      ProcessHeaders(inStream);

      if (!CreateRequiredData(outDir))
      {
         std::cout << "**** ERROR: Failed to create the required data for the report" << std::endl;
         return false;
      }

      ProcessData(inStream);

      if (mWriteHeader)
      {
         PrintReportHeader(outStream);
      }

      std::cout << "   Printing report ..." << std::endl;

      PrintReport(inStream, outStream);

      inStream.close();
      outStream.close();

      ++mCurrentMC;
   }
   while (isMC && mUseMC && (mCurrentMC <= mEndMC));

   std::cout << "\n " << mCurrentMC - 1 << " report(s) output" << std::endl << std::endl;

   return true;
}

// =================================================================================================
void Report::ProcessData(std::istream& aInStream)
{
   mFilteredDataLoc = std::queue<std::iostream::pos_type>();

   std::string line;
   while (getline(aInStream, line))
   {
      std::vector<std::string> dataStrVec;
      Report::Tokenize(line, dataStrVec);
      if (dataStrVec[1] == "SIMULATION_STARTING")
      {
         SetDateTime(dataStrVec);
      }
      if (Filter(dataStrVec))
      {
         mFilteredDataLoc.push(aInStream.tellg());
      }
   }
   aInStream.clear();
   aInStream.seekg(mEndOfHeaders);
}

void Report::SetDateTime(const std::vector<std::string>& aDataStrVec)
{
   auto evtIter = mEventToTagData.find("SIMULATION_STARTING");
   if (evtIter != mEventToTagData.end())
   {
      int year = atoi(aDataStrVec[evtIter->second["year"].mIndex].c_str());
      int month = atoi(aDataStrVec[evtIter->second["month"].mIndex].c_str());
      int day = atoi(aDataStrVec[evtIter->second["day"].mIndex].c_str());
      int hour = atoi(aDataStrVec[evtIter->second["hour"].mIndex].c_str());
      int minute = atoi(aDataStrVec[evtIter->second["minute"].mIndex].c_str());
      double second = atof(aDataStrVec[evtIter->second["second"].mIndex].c_str());
      mSimDateTime.SetDate(year, month, day);
      mSimDateTime.SetTime(hour, minute, second);
   }
   else
   {
      throw UtException("***** Error: Unknown start date and time of simulation data. Add \"SIMULATION_STARTING\" event to CSV output file.");
   }
}
// =================================================================================================
void Report::Tokenize(const std::string&        aStr,
                      std::vector<std::string>& aTokens,
                      const char                aDelimiter /* = ','*/)
{
   std::string cell;
   std::stringstream lineStream(aStr);
   while (getline(lineStream, cell, aDelimiter))
   {
      aTokens.emplace_back(cell);
   }
}

// =================================================================================================
std::vector<std::string> Report::Tokenize(const std::string& aStr,
                                          char               aDelimiter)
{
   std::vector<std::string> tokens;
   Tokenize(aStr, tokens, aDelimiter);
   return tokens;
}

// =================================================================================================
std::string Report::PrintDelimiter() const
{
   switch (mDelimiterType)
   {
      case DelimiterType::cCOMMA:
         return ",";
      case DelimiterType::cSEMICOLON:
         return ";";
      case DelimiterType::cTAB:
         return "\t";
      case DelimiterType::cSPACE:
         return " ";
      default:
         return "";
   }
}

// =================================================================================================
void Report::SetDelimiter(const std::string& aTypeString)
{
   std::string typeStr(aTypeString);
   UtStringUtil::ToLower(typeStr);
   if (typeStr == "comma")
   {
      mDelimiterType = DelimiterType::cCOMMA;
   }
   else if (typeStr == "semicolon")
   {
      mDelimiterType = DelimiterType::cSEMICOLON;
   }
   else if (typeStr == "tab")
   {
      mDelimiterType = DelimiterType::cTAB;
   }
   else if (typeStr == "space")
   {
      mDelimiterType = DelimiterType::cSPACE;
   }
   else if (typeStr == "column")
   {
      mDelimiterType = DelimiterType::cCOLUMN;
   }
   else
   {
      mDelimiterType = DelimiterType::cUNKNOWN;
   }
}

// =================================================================================================
double Report::FormatAngle(double aAngle) const
{
   double angle = aAngle;
   if (mAngleUnits == AngleUnitType::cDEGREES)
   {
      angle *= UtMath::cDEG_PER_RAD;
   }

   if (fabs(aAngle) < 1.0E-12)
   {
      angle = 0.0;
   }
   return angle;
}

// =================================================================================================
UtTime Report::FormatTime(double aSimTime) const
{
   return UtTime(aSimTime, mTimeFormat);
}

// =================================================================================================
void Report::FormatNumberColumn(std::ostream& aStream,
                                size_t        aNumSpaces,
                                int           aNumDigits,
                                int           aInt)
{
   if (aNumSpaces > 0)
   {
      aStream << std::setfill(' ') << std::setw(aNumSpaces) << "";
   }

   if (aNumDigits > 0)
   {
      aStream << std::setfill('0') << std::setw(aNumDigits);
   }
   aStream << aInt;
}

// =================================================================================================
void Report::FormatStringColumn(std::ostream& aStream,
                                std::string&  aString,
                                int           aNumSpaces)
{
   aStream << std::setfill(' ') << std::setw(aNumSpaces) << aString;
}

// =================================================================================================
Header::DataTag Report::ParseHeaderTag(const std::string& aString,
                                       bool               aExpectType)
{
   Header::DataTag result;
   result.name = aString;

   if (aExpectType)
   {
      size_t first = aString.find('<');
      size_t last = aString.find('>');
      if ((first != std::string::npos) &&
          (last != std::string::npos))
      {
         result.name = aString.substr(0, first);
         result.dataType = Header::GetDataType(aString.substr(first + 1, last - first - 1));
      }
      else
      {
         std::cout << "***WARNING: Header tag'" << aString << "' does not specify the expected data type" << std::endl;
      }
   }

   if (aString.find("_count") != std::string::npos)
   {
      result.countType = Header::CountType::cCOUNT;
   }
   else if (aString.find("_#") != std::string::npos)
   {
      result.countType = Header::CountType::cINDEX;
   }

   //if (aExpectType)
   //{
   //   std::smatch match;
   //   if (std::regex_search(aString, match, std::regex("<(.*)>$")))
   //   {
   //      // Set the type and remove the typename
   //      result.name = match.prefix();
   //      result.dataType = Header::GetDataType(match[1].str());
   //   }
   //   else
   //   {
   //      cout << "***WARNING: Header tag'" << aString << "' does not specify the expected data type" << endl;
   //   }
   //}
   //
   //if (std::regex_search(aString, std::regex("_count")))
   //{
   //   result.countType = Header::CountType::cCOUNT;
   //}
   //else if (std::regex_search(aString, std::regex("_#")))
   //{
   //   result.countType = Header::CountType::cINDEX;
   //}
   return result;
}


// =================================================================================================
void Report::ProcessHeaders(std::istream& aStream)
{
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
      for (const auto& tag : eventHeader.GetDataTags())
      {
         if (mEventToTagData[evtName].find(tag.name) == std::end(mEventToTagData[evtName]))
         {
            TagData td;
            td.mIndex = index;
            td.mType = tag.dataType;
            mEventToTagData[evtName][tag.name] = td;
         }
         ++index;
      }
   }
}

// =================================================================================================
std::vector<Header::DataTag> ParseEventTags(const std::string& aEventName,
                                            const std::string& eventTagsString)
{
   std::vector<Header::DataTag> result;
   std::vector<std::string> tokens = Report::Tokenize(eventTagsString, ',');
   for (const auto& token : tokens)
   {
      // Copy while iterating for now because ParseHeaderTag modifies the first argument
      auto t = Report::ParseHeaderTag(token, true);
      // Check to see if the event tag has been used already
      auto it = find_if(std::begin(result), std::end(result), [&](const Header::DataTag & tag) { return tag.name == t.name; });
      if (it != result.end())
      {
         std::cout << "***WARNING: Duplicate header: Event: " << aEventName
                   << " Header: " << token
                   << " indexes: " << it - result.begin()
                   << ", " << result.size()
                   << std::endl;
         continue;
      }
      result.emplace_back(t);
   }
   return result;
}

// =================================================================================================
Header Report::ProcessHeaderLine(const std::string& aLine)
{
   auto firstDelimiterLoc = aLine.find_first_of(',');
   std::string eventName = aLine.substr(0, firstDelimiterLoc);
   auto dataTags = ParseEventTags(eventName, aLine.substr(firstDelimiterLoc + 1));
   return Header{ eventName, dataTags };
}

// =================================================================================================
Header::DataType Header::GetDataType(const std::string& aType)
{
   if (aType == "int")
   {
      return DataType::Int;
   }
   else if (aType == "double")
   {
      return DataType::Double;
   }
   else if (aType == "string")
   {
      return DataType::String;
   }
   else if (aType == "time")
   {
      return DataType::Time;
   }
   else if (aType == "lat")
   {
      return DataType::Lat;
   }
   else if (aType == "lon")
   {
      return DataType::Lon;
   }
   else if (aType == "angle")
   {
      return DataType::Angle;
   }
   // Default to String if not specified
   return DataType::String;
}

// =================================================================================================
//static
std::string Report::GetTypeStr(Report::Type aReportType)
{
   switch (aReportType)
   {
      case Report::Type::cCOMMUNICATION:
         return "Communication";
      case Report::Type::cDETECTION:
         return "Detection";
      case Report::Type::cENGAGEMENT:
         return "Engagement";
      case Report::Type::cECLIPSE:
         return "Eclipse";
      case Report::Type::cTRAJECTORY:
         return "Trajectory";
      case Report::Type::cDSV:
         return "DSV";
      case Report::Type::cUNKNOWN:
      default:
         return "UNKNOWN";
   }
}

// =================================================================================================
//! Performs substitution for naming output files
//!    %d  - Current Monte-Carlo Number
//!    %D  - Date   M-D-Y
//!    %T  - Time   HHMMSS
std::string Report::SubstituteOutputFileVariables(const std::string& aOutputFile,
                                                  size_t             aMC_Number,
                                                  UtCalendar&        aCalendar)
{
   std::string newString = aOutputFile;

   std::ostringstream time;
   time << std::setfill('0') << std::setw(2) << aCalendar.GetHour()
        << std::setw(2) << aCalendar.GetMinute()
        << std::setw(2) << static_cast<int>(aCalendar.GetSecond());

   UtStringUtil::ReplaceAll(newString, "%d", std::to_string(aMC_Number));
   UtStringUtil::ReplaceAll(newString, "%D", std::to_string(aCalendar.GetMonth()) + "-" + std::to_string(aCalendar.GetDay()) + "-" + std::to_string(aCalendar.GetYear()));
   UtStringUtil::ReplaceAll(newString, "%T", time.str());

   return newString;
}

// Extracts the header lines from a stream, leaving the stream position
// at the end of the headers.
//static
std::vector<std::string> Report::ExtractHeaders(std::istream& aStream)
{
   std::vector<std::string> result;
   auto endOfHeaders = aStream.tellg();
   std::string line;
   while (getline(aStream, line))
   {
      // Ignore any comment line
      if (UtStringUtil::StartsWith(line, "#"))
      {
         continue;
      }

      // If not a comment nor a header, reset the stream position to the end of the last header
      if (!UtStringUtil::StartsWith(line, "!"))
      {
         aStream.clear();
         aStream.seekg(endOfHeaders);
         break;
      }
      line.erase(0, 1);
      UtStringUtil::TrimWhiteSpace(line);
      result.emplace_back(line);
      endOfHeaders = aStream.tellg();
   }
   return result;
}

std::string Report::FormatDateTime(double aSimTime)
{
   UtCalendar cal = mSimDateTime;
   cal.AdvanceTimeBy(aSimTime);
   return FormatDateTime(cal.GetYear(),
                         cal.GetMonth(),
                         cal.GetDay(),
                         cal.GetHour(),
                         cal.GetMinute(),
                         cal.GetSecond());
}

std::string Report::FormatDateTime(int    aYear,
                                   int    aMonth,
                                   int    aDay,
                                   int    aHour,
                                   int    aMinute,
                                   double aSecond)
{
   std::stringstream ss;
   ss << aMonth << '-' << aDay << '-' << aYear << ' ' << aHour << ':' << aMinute << ':' << aSecond;
   return ss.str();
}
