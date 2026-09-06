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

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <limits>
#include <string>

#include "post_processor_lib_export.h"

struct POST_PROCESSOR_LIB_EXPORT Options
{
   std::string  mConfigurationFile{};                          //!< Configuration filename.
   std::string  mReportName{};                                 //!< Report output filename(s).
   std::string  mDataFile{};                                   //!< Data filename.
   std::string  mOutputDir{};                                  //!< Output file directory/path.
   size_t       mStartMC{1};                                   //!< Starting Monte-Carlo Run Number
   size_t       mEndMC{ std::numeric_limits<size_t>::max()};   //!< Ending Monte-Carlo Run Number
};

#endif
