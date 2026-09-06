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

#ifndef ECLIPSEREPORT_HPP
#define ECLIPSEREPORT_HPP

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

#include "post_processor_lib_export.h"

#include "Report.hpp"
class     UtInput;

class POST_PROCESSOR_LIB_EXPORT EclipseReport : public Report
{
   public:

      EclipseReport() = delete;
      explicit EclipseReport(const Options& aOptions);
      ~EclipseReport() override = default;

      //Time elements for Eclipse events
      struct EclipseInterval
      {
         EclipseInterval() = default;
         EclipseInterval(const EclipseInterval& aEclipseInterval);
         EclipseInterval(double aStart,
                         double aEnd);
         double mStart;
         double mEnd;
         double mDuration;

         bool operator<(const EclipseInterval& aRhs) const;
      };

      //Eclipse events
      struct EclipseEvent
      {
         EclipseEvent() = default;
         EclipseEvent(const std::string& aPlatform, const EclipseInterval& aEclipseInterval);
         std::string mPlatform;
         EclipseInterval mInterval;

         bool operator<(const EclipseEvent& aRhs) const;
      };

      using EclipseMap = std::map< std::string, EclipseInterval >;
      using EclipseQueue = std::priority_queue<EclipseEvent>;

      bool CreateRequiredData(std::string& aOutputDir) override;
      bool Filter(std::vector<std::string>& aLine) override;
      void PrintReportHeader(std::ostream& aStream) override;
      void PrintReport(std::istream& aInStream, std::ostream& aOutStream) override;
      bool ProcessInputConfiguration(UtInput& aInput) override;
      bool UpdateEclipseReport(std::vector<std::string>& aDataStrVec, double aEvtTime);

   private:

      //Tracks the platforms that the user wants to have data for in the report
      std::unordered_set<std::string> mPlatforms;

      //Tracks the set of platforms that currently has an ECLIPSE_ENTRY event without a corresponding ECLIPSE_EXIT event.
      EclipseMap mEclipseMap;

      //Tracks the intervals of all platforms' eclipse entry and exit
      EclipseQueue mEclipseQueue;
};

#endif
