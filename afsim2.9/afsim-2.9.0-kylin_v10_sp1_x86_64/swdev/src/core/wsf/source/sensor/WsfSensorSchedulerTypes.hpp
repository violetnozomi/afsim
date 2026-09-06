// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSENSORSCHEDULERTYPES_HPP
#define WSFSENSORSCHEDULERTYPES_HPP

#include <functional>
#include <memory>
#include <string>

class UtInput;
#include "wsf_export.h"
class WsfSensorScheduler;

namespace WsfSensorSchedulerTypes
{
using FactoryPtr = std::function<std::unique_ptr<WsfSensorScheduler>(const std::string&)>;

bool LoadInstance(UtInput& aInput, std::unique_ptr<WsfSensorScheduler>& aSensorSchedulerPtr);

WSF_EXPORT void AddObjectFactory(FactoryPtr aFactoryPtr);
} // namespace WsfSensorSchedulerTypes

#endif
