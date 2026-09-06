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

#ifndef WSFSCRIPTTASKPROCESSORCLASS_HPP
#define WSFSCRIPTTASKPROCESSORCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptTaskManagerClass.hpp"

//! Define script methods for WsfTaskProcessor.
class WSF_EXPORT WsfScriptTaskProcessorClass : public WsfScriptTaskManagerClass
{
public:
   WsfScriptTaskProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(EnterState);
   UT_DECLARE_SCRIPT_METHOD(TracksInState);
   UT_DECLARE_SCRIPT_METHOD(CountInState);
   UT_DECLARE_SCRIPT_METHOD(StateOfTrack);
   UT_DECLARE_SCRIPT_METHOD(SetEvaluationInterval);
   UT_DECLARE_SCRIPT_METHOD(SetTransitionTime);
};

#endif
