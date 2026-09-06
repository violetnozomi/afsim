// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#include "test_zone.hpp"
#include <logger.hpp>

int main (int argc, char *argv[])
{
   il::logger::Init_Loggers (">> %Message%");
   il::logger::Set_Global_Log_Level(il::logger::TRACE);

   test_zone();

   return 0;
}