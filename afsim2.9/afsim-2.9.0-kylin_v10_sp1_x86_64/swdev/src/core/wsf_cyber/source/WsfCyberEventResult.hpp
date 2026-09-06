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

#ifndef WSFCYBEREVENTRESULT_HPP
#define WSFCYBEREVENTRESULT_HPP

#include "wsf_cyber_export.h"

#include "WsfEventResult.hpp"

namespace wsf
{
namespace cyber
{
class Engagement;

namespace trigger
{
class Trigger;

} // namespace trigger
} // namespace cyber

namespace event
{

// ===================================================================================================
class WSF_CYBER_EXPORT CyberResult : public Result
{
public:
   CyberResult() = default;
   CyberResult(double aSimTime, const cyber::Engagement& aEngagement, Settings aSettings, std::string aEventName)
      : Result(aSimTime, std::move(aSettings), std::move(aEventName))
      , mEngagement(aEngagement)
   {
   }

   const cyber::Engagement& GetEngagement() const { return mEngagement; }

protected:
   const cyber::Engagement& mEngagement;
};

// ===================================================================================================
class WSF_CYBER_EXPORT CyberTriggerResult : public Result
{
public:
   CyberTriggerResult(double aSimTime, const cyber::trigger::Trigger& aTrigger, Settings aSettings, std::string aEventName)
      : Result(aSimTime, std::move(aSettings), std::move(aEventName))
      , mTrigger(aTrigger)
   {
   }

   const wsf::cyber::trigger::Trigger& GetTrigger() const { return mTrigger; }

protected:
   virtual std::string FormatOutput(const std::string& aObserverType, bool aIsCSV = false) const;

   const cyber::trigger::Trigger& mTrigger;
};

} // namespace event
} // namespace wsf

#endif
