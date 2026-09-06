// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSOSM_INTERFACE_HPP
#define WSFSOSM_INTERFACE_HPP

#include "wsf_sosm_export.h"

#include <map>
#include <string>

#include "UtCallbackHolder.hpp"
#include "WsfScenarioExtension.hpp"

class SOSM_Manager;
class UtInput;
class WsfPlatform;
class WsfSensorMode;

//! A scenario extension that provides an interface to the  Spectral Optical Sensor Model (SOSM).
//!
//! This class implements the processing for the 'sosm_interface' input block, which is
//! provided by the user to local SOSM input files.

class WSF_SOSM_EXPORT WsfSOSM_Interface : public WsfScenarioExtension
{
public:
   WsfSOSM_Interface();
   ~WsfSOSM_Interface() override;

   void AddedToScenario() override;
   bool Complete() override;
   bool ProcessInput(UtInput& aInput) override;

   std::string GetSensorType(WsfSensorMode* aSensorModePtr) const;
   std::string GetTargetType(WsfPlatform* aTargetPtr) const;

   SOSM_Manager& GetManager() const { return *mManagerPtr; }

protected:
   void FileReferenced(const std::string& aFileName);

   SOSM_Manager*                              mManagerPtr;
   typedef std::map<std::string, std::string> TypeMap;
   TypeMap                                    mSensorTypeMap;
   TypeMap                                    mTargetTypeMap;
   UtCallbackHolder                           mCallbacks;
};

#endif
