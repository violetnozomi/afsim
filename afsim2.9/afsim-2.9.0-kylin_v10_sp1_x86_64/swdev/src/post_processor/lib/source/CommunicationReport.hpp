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

#ifndef COMMUNICATIONREPORT_HPP
#define COMMUNICATIONREPORT_HPP

#include <iostream>
#include <map>
#include <vector>

#include "post_processor_lib_export.h"

#include "Report.hpp"
class     UtInput;

class POST_PROCESSOR_LIB_EXPORT CommunicationReport : public Report
{
   public:

      CommunicationReport() = delete;
      explicit CommunicationReport(const Options& aOptions);
      ~CommunicationReport() override = default;

      bool CreateRequiredData(std::string& aOutputDir) override { return true; }
      bool Filter(std::vector<std::string>& aLine) override;
      void PrintReportHeader(std::ostream& aStream) override;
      void PrintReport(std::istream& aInStream, std::ostream& aOutStream) override;
      bool ProcessInputConfiguration(UtInput& aInput) override;

   private:
      std::string                mSubReportType{"traffic_counts"};
      std::map<std::string, int> mReportOptions{};
      std::map<std::string, int> mTransmitters{};
      std::map<std::string, int> mReceivers{};
      std::map<std::string, int> mNetworks{};
      std::map<std::string, int> mMessagesGen{};
      std::map<std::string, int> mMessagesXmtd{};
      std::map<std::string, int> mMessagesComp{};
      std::map<std::string, int> mMessagesPurged{};
      std::map<std::string, int> mExists{};
};

#endif
