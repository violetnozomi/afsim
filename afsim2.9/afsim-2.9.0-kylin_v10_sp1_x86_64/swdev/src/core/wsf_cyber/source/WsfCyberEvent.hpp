// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBEREVENT_HPP
#define WSFCYBEREVENT_HPP

#include "wsf_cyber_export.h"

#include <string>

#include "WsfEvent.hpp"

namespace wsf
{
namespace cyber
{

//! An event associated with cyber. These typically involve scheduled time delays
//! to provide a high level, generic simulation of specific operations involved in cyber
//! warfare that are not specifically modeled.
class WSF_CYBER_EXPORT Event : public WsfEvent
{
public:
   enum class Type : int
   {
      cSCAN_DELAY,
      cATTACK_DELAY,
      cATTACK_DETECTION_DELAY,
      cATTACK_RECOVERY_DELAY,
      cOTHER,
      cNONE
   };

   Event(double aSimTime, Type aEventType, std::string aVictim, size_t aKey);

   ~Event() override = default;

   EventDisposition Execute() override;

   Type               GetType() const { return mEventType; }
   const std::string& GetVictim() const { return mVictim; }
   size_t             GetKey() const { return mKey; }

private:
   Type        mEventType;
   std::string mVictim;
   size_t      mKey;
};

} // namespace cyber
} // namespace wsf

#endif
