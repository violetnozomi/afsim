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

#ifndef ENGAGEMENTREPORT_HPP
#define ENGAGEMENTREPORT_HPP

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

#include "post_processor_lib_export.h"

#include "Report.hpp"
class     UtInput;

class POST_PROCESSOR_LIB_EXPORT EngagementReport : public Report
{
   public:

      EngagementReport() = delete;
      explicit EngagementReport(const Options& aOptions);
      ~EngagementReport() override = default;

      bool CreateRequiredData(std::string& aOutputDir) override;
      bool Filter(std::vector<std::string>& aLine) override;
      void PrintReportHeader(std::ostream& aStream) override;
      void PrintReport(std::istream& aInStream, std::ostream& aOutStream) override;
      bool ProcessInputConfiguration(UtInput& aInput) override;

   private:
      enum class SubReport
      {
         Unknown,
         GAH, //Geometric_Action_History
         DFH, //damage_factor_history
         TEH, //track_event_history
         WEX   //weapon_expenditures
      };

      void SetSubreport(const std::string& aSubReport);
      std::string GetSubreportStr() { return GetSubreportStr(mSubReportType); }
      std::string GetSubreportStr(SubReport& aSubReport);
      SubReport                       mSubReportType = SubReport::Unknown;
      std::unordered_set<std::string> mReportOptions;
      std::unordered_set<std::string> mTakenBy;
      std::unordered_set<std::string> mTakenAgainst;
      std::unordered_set<std::string> mActionTypes;
      std::unordered_set<std::string> mWeaponTypes;
      std::unordered_set<std::string> mAOI;
      std::unordered_set<std::string> mDEOptions;
      std::string           mReferenceSet;
};

#endif
