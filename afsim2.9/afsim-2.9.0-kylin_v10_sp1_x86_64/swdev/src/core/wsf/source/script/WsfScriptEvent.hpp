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

#ifndef WSFEXECUTESCRIPTEVENT_HPP
#define WSFEXECUTESCRIPTEVENT_HPP

#include "wsf_export.h"

#include "UtScriptData.hpp"
#include "WsfEvent.hpp"
#include "WsfStringId.hpp"
class WsfPlatform;
class WsfScriptProcessor;
class WsfScriptManager;

//! An event scheduled to execute a script on a given platform or processor.
class WSF_EXPORT WsfScriptEvent : public WsfEvent
{
public:
   WsfScriptEvent() = default;
   WsfScriptEvent(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId aScriptName, const UtScriptDataList& aScriptArgs);

   WsfScriptEvent(double                  aSimTime,
                  WsfScriptProcessor*     aProcessorPtr,
                  WsfStringId             aScriptName,
                  const UtScriptDataList& aScriptArgs);

   WsfScriptEvent(double aSimTime, WsfSimulation* aSimulationPtr, WsfStringId aScriptName, const UtScriptDataList& aScriptArgs);

   EventDisposition Execute() override;

private:
   //! The index of the platform in whose context the script is to execute.
   //! This will be zero if the script to be executed is in the simulation context.
   size_t mPlatformIndex{0};

   //! The name ID of the processor in whose context the script is to execute.
   //! This will be zero if the script to be executed in the platform or simulation context.
   WsfStringId mProcessorNameId;

   //! The name ID of the script to execute.
   WsfStringId mScriptName;

   //! The script arguments
   UtScriptDataList mScriptArgs;
};

#endif
