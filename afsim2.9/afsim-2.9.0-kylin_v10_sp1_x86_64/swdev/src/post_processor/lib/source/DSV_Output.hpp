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

#ifndef DSV_OUTPUT_HPP
#define DSV_OUTPUT_HPP

#include <map>
#include <unordered_map>

#include "post_processor_lib_export.h"

#include "Report.hpp"
#include "UtMemory.hpp"

class POST_PROCESSOR_LIB_EXPORT DSV_Output : public Report
{
   public:

      struct EventData
      {
         std::string mBaseFileName;
         bool        mSetIndex = false;
         size_t      mIndex = 0;
      };

      using EventMap = std::unordered_map<std::string, EventData>;

      DSV_Output() = delete;
      explicit DSV_Output(const Options& aOptions);
      ~DSV_Output() override = default;

      bool CreateRequiredData(std::string& aOutputDir) override;
      bool Filter(std::vector<std::string>& aLine) override;
      void PrintReportHeader(std::ostream& aStream) override;
      bool StreamIsOpen(size_t aIndex) const;
      std::ofstream& StreamRef(size_t aIndex) { return *mFileStreams[aIndex]; }

   private:

      void PrintReport(std::istream& aInStream,
                       std::ostream& aOutStream) override;
      //! Process the CSV data tags (headers)
      //! @note Headers are necessary for DSV Report to work properly
      void ProcessHeaders(std::istream& aStream) override;

      void AddEvent(const std::string& aEventName,
                    const std::string& aBaseFilename = "");
      bool OpenFile(const std::string& aFileName,
                    size_t             aIndex);

      void PrintData(std::ostream&            aStream,
                     std::vector<std::string> aEventVec);
      void PrintCount(std::ostream&             aStream,
                      std::vector<std::string>& aEvent,
                      size_t&                   aIndex);

      void PrintHeaderData(std::ostream&            aStream,
                           std::vector<std::string> aEventVec);
      void PrintHeaderCount(std::ostream&             aStream,
                            std::vector<std::string>& aEvent,
                            size_t&                   aIndex);

      void UpdateMaxCount(const std::vector<std::string>& aEventData);

      EventMap                                                          mEventMap;

      std::vector<std::string>                                          mFileNames;
      std::vector<std::unique_ptr<std::ofstream>>                       mFileStreams;
      std::unordered_map<std::string, std::map<size_t, std::string>>    mEventTagsByIndex;
      std::unordered_map<std::string, bool>                             mHeaderPrinted;
      std::map<std::string, std::map<size_t, std::vector<std::string>>> mCountHeader;
      std::map<std::string, std::map<size_t, size_t>>                   mCountHeaderMax;
};

#endif
