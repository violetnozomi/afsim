// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRIGGEREVENT_HPP
#define WSFCYBERTRIGGEREVENT_HPP

#include "wsf_cyber_export.h"

#include "WsfCyberTrigger.hpp"
#include "WsfEvent.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

//! An event for triggers instantiated from the global context.
class GlobalEvent : public WsfEvent
{
public:
   GlobalEvent(double aSimTime, std::string aTriggerName);

   ~GlobalEvent() override = default;

   EventDisposition Execute() override;

protected:
   std::string mTriggerName;
};

//! An event for triggers instantiated at the platform context.
class PlatformEvent : public GlobalEvent
{
public:
   PlatformEvent(double aSimTime, std::string aPlatformName, std::string aTriggerName);

   ~PlatformEvent() override = default;

   EventDisposition Execute() override;

protected:
   std::string mPlatformName;
};

} // namespace trigger
} // namespace cyber
} // namespace wsf

#endif
