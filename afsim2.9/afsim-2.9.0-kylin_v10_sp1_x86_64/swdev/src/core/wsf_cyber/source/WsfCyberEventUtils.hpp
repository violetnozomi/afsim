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

#ifndef WSFCYBEREVENTUTILS_HPP
#define WSFCYBEREVENTUTILS_HPP

#include "wsf_cyber_export.h"

#include <ostream>
#include <sstream>

namespace wsf
{
namespace cyber
{
class Engagement;
} // namespace cyber
namespace event
{

namespace utils
{
std::string WSF_CYBER_EXPORT GetCyberEvent(const std::string& aEvent, const cyber::Engagement& aEngagement);
std::string WSF_CYBER_EXPORT GetThresholdDraw(double aThreshold, double aDraw);
std::string WSF_CYBER_EXPORT GetAttackFailureReason(const cyber::Engagement& aEngagement);
std::string WSF_CYBER_EXPORT GetAttackReportStatus(const cyber::Engagement& aEngagement);
std::string WSF_CYBER_EXPORT GetImmunityStatus(const cyber::Engagement& aEngagement);
std::string WSF_CYBER_EXPORT GetScanFailureReason(const cyber::Engagement& aEngagement);
} // namespace utils

namespace utilsCSV
{
std::string WSF_CYBER_EXPORT GetCyberEvent(const std::string& aEventName, const cyber::Engagement& aEngagement);
} // namespace utilsCSV

} // namespace event
} // namespace wsf

#endif
