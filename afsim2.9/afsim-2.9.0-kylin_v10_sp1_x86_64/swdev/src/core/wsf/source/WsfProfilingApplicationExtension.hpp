// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPROFILINGAPPLICATIONEXTENSION_HPP
#define WSFPROFILINGAPPLICATIONEXTENSION_HPP

#include "wsf_export.h"

#include "WsfApplicationExtension.hpp"

/// Application extension enabling profiling of C++ AFSIM code.
class WSF_EXPORT WsfProfilingApplicationExtension : public WsfApplicationExtension
{
public:
   /// @brief Parse command-line arguments, and stash them away
   ///    for the profiling system to use later.
   int ProcessCommandLine(WsfStandardApplication::Options& aOptions, int aArgc, char* aArgv[]) override;

   void PrintCommandLineArguments() const override;
};

#endif
