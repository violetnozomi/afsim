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

#include "UtScriptCoordinateSystem.hpp"

#include "UtOrbitalState.hpp"

namespace ut
{
namespace script
{

CoordinateSystem::CoordinateSystem(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("CoordinateSystem");

   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Equatorial>());
   AddStaticMethod(ut::make_unique<Ecliptic>());
}

void CoordinateSystem::Destroy(void* aObjectPtr)
{
   delete static_cast<ut::OrbitalState::CoordinateSystem*>(aObjectPtr);
}

bool CoordinateSystem::EqualTo(void* aLHS, void* aRHS)
{
   auto lhsPtr = static_cast<ut::OrbitalState::CoordinateSystem*>(aLHS);
   auto rhsPtr = static_cast<ut::OrbitalState::CoordinateSystem*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(CoordinateSystem, ut::OrbitalState::CoordinateSystem, Equatorial, 0, "CoordinateSystem", "")
{
   auto coordinateSystemPtr =
      ut::make_unique<ut::OrbitalState::CoordinateSystem>(ut::OrbitalState::CoordinateSystem::cEQUATORIAL);
   aReturnVal.SetPointer(new UtScriptRef(coordinateSystemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(CoordinateSystem, ut::OrbitalState::CoordinateSystem, Ecliptic, 0, "CoordinateSystem", "")
{
   auto coordinateSystemPtr =
      ut::make_unique<ut::OrbitalState::CoordinateSystem>(ut::OrbitalState::CoordinateSystem::cECLIPTIC);
   aReturnVal.SetPointer(new UtScriptRef(coordinateSystemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

} // namespace script
} // namespace ut
