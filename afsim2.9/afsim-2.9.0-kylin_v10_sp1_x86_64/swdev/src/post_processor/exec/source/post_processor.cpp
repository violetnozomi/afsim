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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>

#include "Configuration.hpp"
#include "Report.hpp"
#include "UtException.hpp"
#include "UtWallClock.hpp"

#include "post_processor_version_defines.hpp"

//! Post Processor is a non-graphical application used to post-process the CSV event output from AFSIM.

int main(int argc, char* argv[])
{
   UtWallClock wallClock;

   std::cout << "post_processor : Version " << POST_PROCESSOR_VERSION
             << "; built " << __DATE__ << ' ' << __TIME__ << std::endl;

   Configuration config({argv, static_cast<size_t>(argc)});
   try
   {
      if (!config.Execute())
      {
         std::cerr << " '" << config.GetReportTypeStr() << "' report not generated" << std::endl;
      }
      std::cout << " Processing Time: " << wallClock.GetClock() << " seconds" << std::endl;

      return 0;
   }
   catch (UtException& exception)
   {
      std::cerr << exception.what() << std::endl;
      std::cerr << " Report not generated" << std::endl;
      return 1;
   }
}
