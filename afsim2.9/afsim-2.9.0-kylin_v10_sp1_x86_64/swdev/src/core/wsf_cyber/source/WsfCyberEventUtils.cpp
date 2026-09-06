// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberEventUtils.hpp"

#include <ostream>
#include <sstream>

#include "UtTime.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfEventOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace event
{

namespace utils
{

// ===================================================================================================
std::string GetCyberEvent(const std::string& aEvent, const cyber::Engagement& aEngagement)
{
   return aEvent + ' ' + aEngagement.GetAttacker() + ' ' + aEngagement.GetVictim() +
          " Attack_Type: " + aEngagement.GetAttackType();
}

// ===================================================================================================
std::string GetThresholdDraw(double aThreshold, double aDraw)
{
   std::stringstream out;
   out << "Threshold: " << aThreshold << " Draw: " << aDraw;
   return out.str();
}

// ===================================================================================================
std::string GetAttackFailureReason(const cyber::Engagement& aEngagement)
{
   switch (aEngagement.GetAttackFailureReason())
   {
   case cyber::Engagement::cATTACK_RANDOM_DRAW:
      return "random_draw";
   case cyber::Engagement::cATTACK_IMMUNITY:
      return "blocked_by_immunity";
   case cyber::Engagement::cATTACK_NOT_VULNERABLE:
      return "user_defined_not_vulnerable_to_attack";
   default:
      return "";
   }
}

// ===================================================================================================
std::string GetAttackReportStatus(const cyber::Engagement& aEngagement)
{
   if (aEngagement.GetAttackFailureReason() == cyber::Engagement::cATTACK_RANDOM_DRAW)
   {
      return aEngagement.GetStatusReportSuccess() ? "Succeeded" : "Failed";
   }
   return "Unknown";
}

// ===================================================================================================
std::string GetImmunityStatus(const cyber::Engagement& aEngagement)
{
   return aEngagement.IsVictimImmune() ? "Immune" : "Not_Immune";
}

// ===================================================================================================
std::string GetScanFailureReason(const cyber::Engagement& aEngagement)
{
   switch (aEngagement.GetScanFailureReason())
   {
   case cyber::Engagement::cSCAN_DETECTED:
      return "scan_detected";
   case cyber::Engagement::cSCAN_IMMUNITY:
      return "immunity";
   default:
      return "";
   }
}

} // namespace utils

namespace utilsCSV
{

// ===================================================================================================
std::string GetCyberEvent(const std::string& aEventName, const cyber::Engagement& aEngagement)
{
   return aEventName + ',' + aEngagement.GetAttacker() + ',' + aEngagement.GetVictim() + ',' + aEngagement.GetAttackType();
}

} // namespace utilsCSV
} // namespace event
} // namespace wsf
