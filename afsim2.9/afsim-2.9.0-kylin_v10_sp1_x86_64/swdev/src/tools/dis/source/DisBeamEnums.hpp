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

#ifndef DisBeamEnums_hpp
#define DisBeamEnums_hpp

#include <string>

#include "DisTypes.hpp"

// The following Enumerations used within the DIS Beam Record are obtained from
// the SISO-REF-010-00v20-0 (published 19 November 2013).

namespace DisEnum
{
namespace beam
{
namespace function
{
enum FunctionType
{
   Other                         = 0,
   Search                        = 1,
   HeightFinding                 = 2,
   Acquisition                   = 3,
   Tracking                      = 4,
   AcquisitionAndTracking        = 5,
   CommandGuidance               = 6,
   Illumination                  = 7,
   Ranging                       = 8,
   MissileBeacon                 = 9,
   MissileFusing                 = 10,
   ActiveRadarMissileSeeker      = 11,
   Jamming                       = 12,
   IFF                           = 13,
   Navigation_Weather            = 14,
   Meteorological                = 15,
   DataTransmission              = 16,
   NavigationalDirectionalBeacon = 17,
   TimeSharedSearch              = 18,
   TimeSharedAcquisition         = 19,
   TimeSharedTrack               = 20,
   TimeSharedCommandGuidance     = 21,
   TimeSharedIllumination        = 22,
   TimeSharedJamming             = 23
};
inline bool IsValid(DisEnum8 aFunction)
{
   return (aFunction <= TimeSharedJamming);
}
const std::string& ToString(DisEnum8 aFunction);
} // namespace function
namespace hdtj
{
enum HDTJType
{
   NotSelected = 0,
   Selected    = 1
};
inline bool IsValid(DisEnum8 aHDTJ)
{
   return (aHDTJ <= Selected);
}
const std::string& ToString(DisEnum8 aHDTJ);
} // namespace hdtj
namespace status
{
enum StatusType
{
   Active      = 0,
   Deactivated = 1
};
inline bool IsValid(DisEnum8 /*aStatus*/)
{
   // Bit 1 is defined on or off, other bits are padding, so the status
   //    field can never be invalid.
   return true;
}
std::string ToString(DisEnum8 aStatus);
} // namespace status
} // namespace beam
} // namespace DisEnum

#endif // DisBeamEnums_hpp
