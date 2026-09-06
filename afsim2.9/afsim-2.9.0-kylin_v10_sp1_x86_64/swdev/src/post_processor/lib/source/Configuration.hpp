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

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <memory>
#include <string>

#include "post_processor_lib_export.h"

#include "Options.hpp"
#include "Report.hpp"

#include "UtSpan.hpp"

class POST_PROCESSOR_LIB_EXPORT Configuration
{
   public:

      Configuration() = default;
      Configuration(ut::span<char*> args);
      Configuration(const Configuration& aConfiguration);
      explicit Configuration(const std::string& aConfigurationFile);
      ~Configuration() = default;

      static void ShowUsage();

      bool Execute();

      Options& GetOptions() { return mOptions; }

      void SetReportType(const std::string& aTypeString);
      std::string GetReportTypeStr() const { return Report::GetTypeStr(mReportType); }

   private:

      void ProcessCommandLineOptions(ut::span<char*> args);
      void ProcessConfigurationFile();
      bool ValidateOptions(); // TODO-AWK

      Report& CreateReport();
      void InitReport();

      Options                 mOptions{};
      Report::Type            mReportType{Report::Type::cUNKNOWN};
      std::unique_ptr<Report> mReportPtr{nullptr};
};

#endif
