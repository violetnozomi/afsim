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

#ifndef REPORTBASE_HPP
#define REPORTBASE_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "post_processor_lib_export.h"

#include "UtAngle.hpp"
#include "UtCalendar.hpp"
class     UtCalendar;
class     UtInput;
#include "UtTime.hpp"

#include "Options.hpp"

// =================================================================================================

class POST_PROCESSOR_LIB_EXPORT Header
{
   public:
      enum class DataType
      {
         Bool,
         Double,
         Int,
         Lat,
         Lon,
         String,
         Time,
         Angle
      };

      enum class CountType
      {
         cNONE,
         cCOUNT,
         cINDEX
      };

      struct DataTag
      {
         std::string name = "";
         CountType   countType = CountType::cNONE;
         DataType    dataType = DataType::String;
      };

      Header(const std::string&          aName,
             const std::vector<DataTag>& aDataTags)
         : mName(aName),
           mDataTags(aDataTags) {}

      static Header::DataType GetDataType(const std::string& aType);

      const std::string& GetName() const { return mName; }
      const std::vector<DataTag>& GetDataTags() const { return mDataTags; }

   private:
      std::string          mName;
      std::vector<DataTag> mDataTags;
};

class POST_PROCESSOR_LIB_EXPORT Report
{
   public:

      enum class AngleUnitType
      {
         cDEGREES,
         cRADIANS
      };

      enum class LengthUnitType
      {
         cFEET,
         cMETERS
      };

      enum class TrackNumericalType
      {
         cNUMERICAL,
         cJTIDS
      };

      enum class DelimiterType
      {
         cUNKNOWN,
         cCOMMA,
         cCOLUMN,
         cSEMICOLON,
         cSPACE,
         cTAB
      };

      enum class Type
      {
         cUNKNOWN,
         cCOMMUNICATION,
         cDETECTION,
         cDSV,
         cENGAGEMENT,
         cTRAJECTORY,
         cECLIPSE
      };

      struct  TagData
      {
         size_t           mIndex;
         Header::DataType mType;
      };

      Report() = delete;
      Report(const Type       aReportType,
             const Options&   aOptions);
      virtual ~Report() = default;

      bool PrintReport();
      void ProcessData(std::istream& aInStream);

      virtual bool CreateRequiredData(std::string& aOutputDir) { return true; }
      virtual bool ProcessInputConfiguration(UtInput& aInput);
      virtual void PrintReportHeader(std::ostream& aStream) {}
      virtual bool Filter(std::vector<std::string>& aLine) { return false; }

      std::string PrintDelimiter() const;
      void SetDelimiter(const std::string& aTypeString);
      void SetDateTime(const std::vector<std::string>& aDataStrVec);

      double FormatAngle(double aAngle) const;
      UtTime FormatTime(double aSimTime) const;

      static void FormatNumberColumn(std::ostream& aStream,
                                     size_t        aNumSpaces,
                                     int           aNumDigits,
                                     int           aInt);
      static void FormatStringColumn(std::ostream& aStream,
                                     std::string&  aString,
                                     int           aNumSpaces);

      static std::string GetTypeStr(Report::Type aReportType);

      static Header::DataTag ParseHeaderTag(const std::string& aString,
                                            bool               aExpectType = false);

      static void Tokenize(const std::string&        aStr,
                           std::vector<std::string>& aTokens,
                           const char                aDelimiter = ',');
      static std::vector<std::string> Tokenize(const std::string& aStr,
                                               char               aDelimiter);

   protected:

      void PrepareExtension(std::string& aOutputDir);

      virtual void PrintReport(std::istream& aInStream,
                               std::ostream& aOutStream) = 0;
      virtual void ProcessHeaders(std::istream& aStream);
      Header ProcessHeaderLine(const std::string& aLine);
      //virtual void GenerateHeaders(std::vector<Header>& headers) {}

      void SetLatLonFormat(int& aFormat) { mLatLonFormat = aFormat; }

      static std::string SubstituteOutputFileVariables(const std::string& aOutputFile,
                                                       size_t             aMC_Number,
                                                       UtCalendar&        aCalendar);

      static std::vector<std::string> ExtractHeaders(std::istream& aStream);
      std::string FormatDateTime(double aSimTime = 0);
      std::string FormatDateTime(int    aYear,
                                 int    aMonth,
                                 int    aDay,
                                 int    aHour,
                                 int    aMinute,
                                 double aSecond);

      // Option data
      Type                       mReportType;
      std::string                mReportName;
      std::string                mDataFile;
      std::string                mOutputDir;
      size_t                     mStartMC;
      size_t                     mEndMC;
      UtCalendar                 mSimDateTime;

      // Base Data
      DelimiterType              mDelimiterType = DelimiterType::cCOMMA;

      double                     mStartTime = 0.0;
      std::string                mStartTimeUnits = "seconds";
      double                     mEndTime = std::numeric_limits<double>::max();
      std::string                mEndTimeUnits = "seconds";

      size_t                     mCurrentMC = mStartMC;
      bool                       mUseMC = false;

      std::map<std::string, std::map<std::string, TagData>> mEventToTagData;
      std::vector<std::string>                              mEvents;

      std::vector<std::string>   mHeader;
      bool                       mWriteHeader = true;
      std::iostream::pos_type    mEndOfHeaders = 0;

      // Formatting
      int                        mTimeFormat = UtTime::FmtS + 5 + UtTime::FmtShowPoint + UtTime::FmtNoSuffix;
      int                        mLatLonFormat = UtAngle::FmtDMS + 2;
      AngleUnitType              mAngleUnits = AngleUnitType::cDEGREES;
      int                        mInterval = 1;
      bool                       mSingleOutputFile = true;
      TrackNumericalType         mTrackNum = TrackNumericalType::cNUMERICAL;
      LengthUnitType             mLengthUnits = LengthUnitType::cMETERS;
      int                        mPrecision = 8;
      size_t                     mMaxPlatformNameSize = 8;
      size_t                     mMaxTimeSize = 15;

      // Other
      std::vector<std::string>            mRequiredData;
      std::queue<std::iostream::pos_type> mFilteredDataLoc;
};

namespace report
{

namespace util
{

template<class T, class U>
bool Contains(const T& aContainer, const U& aValue)
{
   auto it = std::find(std::begin(aContainer), std::end(aContainer), aValue);
   return it != std::end(aContainer);
}

}

}

#endif
