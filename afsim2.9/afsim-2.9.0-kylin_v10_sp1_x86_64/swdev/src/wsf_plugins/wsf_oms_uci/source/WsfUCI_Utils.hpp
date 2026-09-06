// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFUCI_UTILS_HPP
#define WSFUCI_UTILS_HPP

#include "wsf_oms_uci_export.h"


#include "uci/type/CapabilityAvailabilityEnum.h"
#include "uci/type/DateTimeType.h"
#include "uci/type/StandardIdentityEnum.h"
#include "uci/type/KinematicsType.h"
#include "uci/type/SubsystemStateEnum.h"

class UtCalendar;
class WsfArticulatedPart;
class WsfSensor;
class WsfTrack;
class WsfPlatform;
class WsfPlatformPart;
class WsfSimulation;

namespace wsf
{
   class UCI_Interface;
}

namespace UCI_Util
{
   constexpr double sNANOSECONDS_PER_SECOND = 1000000000.0;
   
   constexpr double sTOLERANCE = 1e-6;

   wsf::UCI_Interface* GetInterface(WsfPlatform* aPlatformPtr);

   wsf::UCI_Interface* GetInterface(WsfSimulation* aSimulationPtr);

   uci::type::DateTimeTypeValue ConvertDoubleToUCI_DateTime(double aTime);

   double ConvertUCI_DateTimeToDouble(uci::type::DateTimeTypeValue aUCI_DateTime);

   //Converts the absolute sim time to the required time stamp format
   uci::type::DateTimeTypeValue GetTimeStamp(WsfSimulation& aSimulation,
                                             double         aSimTime);

   void GetKinematics(uci::type::DateTimeTypeValue& aSimTime,
                      uci::type::KinematicsType&    aKinematics,
                      WsfSensor*                    aSensorPtr,
                      const WsfTrack&               aTrack);

   uci::type::SubsystemStateEnum::EnumerationItem GetSubsystemState(WsfPlatformPart* aPartPtr);

   std::string GetSubsystemStateString(const uci::type::SubsystemStateEnum::EnumerationItem& aState);

   std::string RemoveAngleBrackets(const std::string&);

   uci::type::StandardIdentityEnum::EnumerationItem GetStandardIdentity(const WsfTrack& aTrack);

   bool CheckLimits(const WsfArticulatedPart* aArticulatedPartPtr,
                    double&                   aAz,
                    double&                   aEl);

} // namespace UCI_Util

#endif
