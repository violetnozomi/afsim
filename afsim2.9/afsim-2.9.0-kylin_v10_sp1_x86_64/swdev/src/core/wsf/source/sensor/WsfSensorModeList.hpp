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

#ifndef WSFSENSORMODELIST_HPP
#define WSFSENSORMODELIST_HPP

#include "wsf_export.h"

#include "WsfModeList.hpp"

class WsfSensor;
class WsfSensorMode;

//! A specialization of WsfModeList that provides for automatic linking of sensors to modes.
//! A sensor is not required to use this, but it must use it if uses modes AND it wants to
//! allow modes to invoke WsfSensorComponent methods.
class WSF_EXPORT WsfSensorModeList : public WsfModeList
{
   friend class WsfSensor;

public:
   WsfSensorModeList(WsfSensorMode* aTemplatePtr);
   WsfSensorModeList(const WsfSensorModeList& aSrc);
   WsfSensorModeList operator=(const WsfSensorModeList& aSrc) = delete;
   WsfSensorModeList(const WsfSensorModeList& aSrc, WsfSensorMode* aTemplatePtr, const std::vector<WsfSensorMode*>& aModeList);
   ~WsfSensorModeList() override = default;

   void AddMode(WsfMode* aModePtr) override;

private:
   void SetSensor(WsfSensor* aSensorPtr);

   //! Pointer to the sensor that owns the mode list.
   WsfSensor* mSensorPtr{nullptr};
};

#endif
