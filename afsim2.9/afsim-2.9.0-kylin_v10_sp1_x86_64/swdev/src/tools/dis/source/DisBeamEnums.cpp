// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisBeamEnums.hpp"

namespace DisEnum
{
namespace beam
{
namespace function
{
const std::string& ToString(DisEnum8 aFunction)
{
   static const std::string STR[] = {"-",
                                     "Search",
                                     "Height Finding",
                                     "Acquisition",
                                     "Tracking",
                                     "Acquisition and tracking",
                                     "Command guidance",
                                     "Illumination",
                                     "Ranging",
                                     "Missile beacon",
                                     "Missile Fusing",
                                     "Active radar missile seeker",
                                     "Jamming",
                                     "IFF",
                                     "Navigation / Weather",
                                     "Meteorological",
                                     "Data transmission",
                                     "Navigational directional beacon",
                                     "Time-Shared Search",
                                     "Time-Shared Acquisition",
                                     "Time-Shared Track",
                                     "Time-Shared Command Guidance",
                                     "Time-Shared Illumination",
                                     "Time-Shared Jamming"};
   size_t                   index = IsValid(aFunction) ? static_cast<size_t>(aFunction) : 0;
   return STR[index];
}
} // namespace function

namespace hdtj
{
const std::string& ToString(DisEnum8 aHDTJ)
{
   static const std::string STR[] = {
      "Not Selected",
      "Selected",
   };
   size_t index = IsValid(aHDTJ) ? static_cast<size_t>(aHDTJ) : 0;
   return STR[index];
}
} // namespace hdtj

namespace status
{
std::string ToString(DisEnum8 aStatus)
{
   static const std::string ActiveSTR[] = {
      "Active",
      "Deactivated",
   };
   return ActiveSTR[(aStatus & 0x01)];
}
} // namespace status
} // namespace beam
} // namespace DisEnum
