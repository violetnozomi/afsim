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

#ifndef DETECTIONREPORT_HPP
#define DETECTIONREPORT_HPP

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

#include "post_processor_lib_export.h"

#include "Report.hpp"
class     UtInput;

class POST_PROCESSOR_LIB_EXPORT DetectionReport : public Report
{
   public:

      enum class DetectionReportType
      {
         cUNKNOWN,
         cALLDETECTIONEVENTS,
         cDETECTEDBY,
         cFIRSTDETECTIONS,
         cTOTALDETECTIONS,
         cACCESS
      };

      DetectionReport() = delete;
      explicit DetectionReport(const Options& aOptions);
      ~DetectionReport() override = default;

      //Access report structs
      struct DetectionInterval
      {
         DetectionInterval() = default;
         DetectionInterval(double aStart, double aEnd)
            : mStart(aStart)
            , mEnd(aEnd)
         {
            mDuration = mEnd - mStart;
         }
         double mStart;
         double mEnd;
         double mDuration;

         bool operator<(const DetectionInterval& aRhs) const;
      };

      struct DetectorDetecteeKey
      {
         DetectorDetecteeKey(const std::string& aDetector,
                             const std::string& aDetectee,
                             const std::string& aSensorName)
            : mDetector(aDetector)
            , mDetectee(aDetectee)
            , mSensorName(aSensorName) {}

         std::string           mDetector;
         std::string           mDetectee;
         std::string           mSensorName;

         bool operator<(const DetectorDetecteeKey& aRhs) const;
      };

      struct ChangedEvent
      {
         ChangedEvent(const DetectorDetecteeKey& aKey,
                      const DetectionInterval& aDetectionInterval)
            : mKey(aKey),
              mInterval(aDetectionInterval)
         {
         }
         DetectorDetecteeKey mKey;
         DetectionInterval mInterval;

         bool operator<(const ChangedEvent& aRhs) const;
      };

      using DetectorDetecteeMap = std::map< DetectorDetecteeKey, DetectionInterval >;
      using KeyInputs = std::priority_queue< ChangedEvent >;

      bool CreateRequiredData(std::string& aOutputDir) override;
      bool Filter(std::vector<std::string>& aLine) override;
      void PrintReportHeader(std::ostream& aStream) override;
      void PrintReport(std::istream& aInStream, std::ostream& aOutStream) override;
      bool ProcessInputConfiguration(UtInput& aInput) override;

   private:

      void SetSubReportType(const std::string& aTypeString);
      std::string GetSubReportTypeStr() const { return GetSubReportTypeStr(mSubReportType); }
      static std::string GetSubReportTypeStr(DetectionReportType aReportType);

      //Access Report
      void PrintAccessReport(std::ostream& aStream);
      bool UpdateAccessReport(std::vector<std::string>& aDataStrVec, double aEvtTime);

      DetectionReportType        mSubReportType;  //!< The sub report type for the detection report.
      std::unordered_set<std::string> mReportOptions;
      std::map<std::string, std::unordered_set<std::string>> mDetecting;
      std::unordered_set<std::string> mDetected;
      DetectorDetecteeMap mDetectorDetecteeMap;
      KeyInputs mKeyInputs;
};

#endif
