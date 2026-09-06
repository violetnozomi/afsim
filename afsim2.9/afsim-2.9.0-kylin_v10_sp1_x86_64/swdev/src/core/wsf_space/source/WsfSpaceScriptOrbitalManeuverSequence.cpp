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

#include "WsfSpaceScriptOrbitalManeuverSequence.hpp"

#include <memory>

#include "UtMemory.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalManeuver.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"
class WsfScenario;
#include "WsfSimulation.hpp"

namespace wsf
{

namespace space
{

namespace script
{

OrbitalMissionSequence::OrbitalMissionSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalEvent(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalMissionSequence");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<AddEvent>());
   AddMethod(ut::make_unique<AddManeuver>());
}

void* OrbitalMissionSequence::Create(const UtScriptContext& aContext)
{
   return new WsfOrbitalMissionSequence(*SCENARIO);
}

// virtual
void* OrbitalMissionSequence::Clone(void* aObjectPtr)
{
   auto* objPtr = static_cast<WsfOrbitalMissionSequence*>(aObjectPtr);
   assert(objPtr != nullptr);
   return objPtr->Clone();
}

// virtual
void OrbitalMissionSequence::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfOrbitalMissionSequence*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(OrbitalMissionSequence, OrbitalMissionSequence, Construct, 0, "WsfOrbitalMissionSequence", "")
{
   auto* sequence = new WsfOrbitalMissionSequence(SIMULATION->GetScenario());
   aReturnVal.SetPointer(new UtScriptRef(sequence, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalMissionSequence, WsfOrbitalMissionSequence, AddManeuver, 1, "void", "WsfOrbitalManeuver")
{
   auto objPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalManeuver>();
   aObjectPtr->AddMissionEvent(ut::clone(objPtr));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalMissionSequence, WsfOrbitalMissionSequence, AddEvent, 1, "void", "WsfOrbitalEvent")
{
   auto objPtr = aVarArgs[0].GetPointer()->GetAppObject<WsfOrbitalEvent>();
   aObjectPtr->AddMissionEvent(ut::clone(objPtr));
}

OrbitalManeuverSequence::OrbitalManeuverSequence(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : OrbitalMissionSequence(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfOrbitalManeuverSequence");

   AddStaticMethod(ut::make_unique<Construct>());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalManeuverSequence, OrbitalManeuverSequence, Construct, 0, "WsfOrbitalManeuverSequence", "")
{
   auto* sequence = new WsfOrbitalMissionSequence(SIMULATION->GetScenario());
   aReturnVal.SetPointer(new UtScriptRef(sequence, aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace script

} // namespace space

} // namespace wsf
