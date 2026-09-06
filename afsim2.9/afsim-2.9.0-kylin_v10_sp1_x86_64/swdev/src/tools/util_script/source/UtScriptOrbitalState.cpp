// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptOrbitalState.hpp"

#include "UtEarth.hpp"
#include "UtOrbitalState.hpp"
#include "UtScriptCalendar.hpp"
#include "UtScriptVec3.hpp"

namespace ut
{
namespace script
{

//! Define script methods for WsfOrbitalState class
OrbitalState::OrbitalState(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("OrbitalState");

   mConstructible = true;
   mCloneable     = true;

   AddStaticMethod(ut::make_unique<Construct_OE>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_OE_Add>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_RV>("Construct"));
   AddStaticMethod(ut::make_unique<Construct_RV_Add>("Construct"));

   AddMethod(ut::make_unique<OrbitIsHyperbolic>());
   AddMethod(ut::make_unique<OrbitIsCircular>());
   AddMethod(ut::make_unique<OrbitIsEquatorial>());
   AddMethod(ut::make_unique<OrbitIsPrograde>());
}

void OrbitalState::Destroy(void* aObjectPtr)
{
   delete static_cast<ut::OrbitalState*>(aObjectPtr);
}

void* OrbitalState::Clone(void* aObjectPtr)
{
   return new ut::OrbitalState(*static_cast<ut::OrbitalState*>(aObjectPtr));
}

//! Script method definition for OrbitalState.Construct(), given orbital elements.
//! This Construct method defaults to an equatorial CoordinateSystem and a true of date ReferenceFrame.
UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, Construct_OE, 1, "OrbitalState", "OrbitalElements")
{
   auto orbitalElementsPtr = aVarArgs[0].GetPointer()->GetAppObject<UtOrbitalElements>();

   auto orbitalStatePtr = ut::make_unique<ut::OrbitalState>(ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                            ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE,
                                                            *orbitalElementsPtr);
   aReturnVal.SetPointer(new UtScriptRef(orbitalStatePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Script method definition for OrbitalState.Construct(), given user-defined orbital elements,
//! along with a coordinate system, and reference frame.
UT_DEFINE_SCRIPT_METHOD(OrbitalState,
                        ut::OrbitalState,
                        Construct_OE_Add,
                        3,
                        "OrbitalState",
                        "CoordinateSystem, ReferenceFrame, OrbitalElements")
{
   auto coordinateSystemPtr = aVarArgs[0].GetPointer()->GetAppObject<ut::OrbitalState::CoordinateSystem>();
   auto referenceFramePtr   = aVarArgs[1].GetPointer()->GetAppObject<ut::OrbitalState::ReferenceFrame>();
   auto orbitalElementsPtr  = aVarArgs[2].GetPointer()->GetAppObject<UtOrbitalElements>();

   auto orbitalStatePtr = ut::make_unique<ut::OrbitalState>(*coordinateSystemPtr, *referenceFramePtr, *orbitalElementsPtr);
   aReturnVal.SetPointer(new UtScriptRef(orbitalStatePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Script method definition for OrbitalState.Construct(), given an epoch and position/velocity vectors
//! This Construct method defaults to an equatorial CoordinateSystem and an ECI ReferenceFrame.
UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, Construct_RV, 3, "OrbitalState", "Calendar, Vec3, Vec3")
{
   auto calendarPtr = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();

   auto                   positionPtr = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto                   velocityPtr = aVarArgs[2].GetPointer()->GetAppObject<UtVec3d>();
   ut::OrbitalStateVector orbitalStateVec(*positionPtr, *velocityPtr);

   auto orbitalStatePtr = ut::make_unique<ut::OrbitalState>(*calendarPtr,
                                                            ut::EarthWGS84(),
                                                            ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                            ut::OrbitalState::ReferenceFrame::cECI,
                                                            orbitalStateVec);
   aReturnVal.SetPointer(new UtScriptRef(orbitalStatePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! Script method definition for OrbitalState.Construct(), given a calendar and position/velocity vectors,
//! along with a central body, coordinate system, and reference frame.
UT_DEFINE_SCRIPT_METHOD(OrbitalState,
                        ut::OrbitalState,
                        Construct_RV_Add,
                        6,
                        "OrbitalState",
                        "Calendar, CentralBody, CoordinateSystem, ReferenceFrame, Vec3, Vec3")
{
   auto calendarPtr         = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto centralBodyPtr      = aVarArgs[1].GetPointer()->GetAppObject<ut::CentralBody>();
   auto coordinateSystemPtr = aVarArgs[2].GetPointer()->GetAppObject<ut::OrbitalState::CoordinateSystem>();
   auto referenceFramePtr   = aVarArgs[3].GetPointer()->GetAppObject<ut::OrbitalState::ReferenceFrame>();

   auto                   positionPtr = aVarArgs[4].GetPointer()->GetAppObject<UtVec3d>();
   auto                   velocityPtr = aVarArgs[5].GetPointer()->GetAppObject<UtVec3d>();
   ut::OrbitalStateVector orbitalStateVec(*positionPtr, *velocityPtr);

   auto orbitalStatePtr =
      ut::make_unique<ut::OrbitalState>(*calendarPtr, *centralBodyPtr, *coordinateSystemPtr, *referenceFramePtr, orbitalStateVec);
   aReturnVal.SetPointer(new UtScriptRef(orbitalStatePtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, OrbitIsHyperbolic, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->OrbitIsHyperbolic());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, OrbitIsCircular, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->OrbitIsCircular());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, OrbitIsEquatorial, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->OrbitIsEquatorial());
}

UT_DEFINE_SCRIPT_METHOD(OrbitalState, ut::OrbitalState, OrbitIsPrograde, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->OrbitIsPrograde());
}

} // namespace script
} // namespace ut
