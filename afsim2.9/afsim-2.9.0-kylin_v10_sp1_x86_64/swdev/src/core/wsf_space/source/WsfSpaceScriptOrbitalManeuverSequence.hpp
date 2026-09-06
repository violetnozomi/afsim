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

#ifndef WSFSCRIPTORBITALMANEUVERSEQUENCE_HPP
#define WSFSCRIPTORBITALMANEUVERSEQUENCE_HPP

#include "wsf_space_export.h"

#include "WsfSpaceScriptOrbitalManeuvers.hpp"

namespace wsf
{

namespace space
{

namespace script
{

//! A scriptable version of WsfOrbitalMissionSequence.
class WSF_SPACE_EXPORT OrbitalMissionSequence : public OrbitalEvent
{
public:
   OrbitalMissionSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalMissionSequence() override {}

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(AddManeuver);
   UT_DECLARE_SCRIPT_METHOD(AddEvent);
};

class WSF_SPACE_EXPORT OrbitalManeuverSequence : public OrbitalMissionSequence
{
public:
   OrbitalManeuverSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitalManeuverSequence() override {}

   UT_DECLARE_SCRIPT_METHOD(Construct);
};

} // namespace script

} // namespace space

} // namespace wsf

#endif
