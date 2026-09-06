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

#ifndef TRAJECTORYREPORT_HPP
#define TRAJECTORYREPORT_HPP

#include <iostream>
//#include <string>
//#include <map>

#include "post_processor_lib_export.h"

#include "Options.hpp"
#include "Report.hpp"
class     UtInput;

class POST_PROCESSOR_LIB_EXPORT TrajectoryReport : public Report
{
   public:

      TrajectoryReport() = delete;
      explicit TrajectoryReport(const Options& aOptions);
      ~TrajectoryReport() override = default;


      bool CreateRequiredData(std::string& aOutputDir) override { return true; }
      bool Filter(std::vector<std::string>& aLine) override { return false; }
      void PrintReportHeader(std::ostream& aStream) override {}
      void PrintReport(std::istream& aInStream, std::ostream& aOutStream) override {}

   private:

      void ProcessData(std::istream& aInStream, std::ostream& aOutStream) { /* TODO-AWK */ }
      void ProcessData(UtInput& aInput, std::ostream& aStream) {}

      //std::string                mParticipantType;
      //std::map<std::string, int> mParticipantOptions;
      //std::map<std::string, int> mDataOptions;
};

#endif
