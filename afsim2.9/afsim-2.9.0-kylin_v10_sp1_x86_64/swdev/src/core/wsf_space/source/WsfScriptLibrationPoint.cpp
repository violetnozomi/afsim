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

#include "WsfScriptLibrationPoint.hpp"

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptVec3.hpp"
#include "WsfLibrationPoint.hpp"

WsfScriptLibrationPoint::WsfScriptLibrationPoint(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLibrationPoint");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<System>());
   AddMethod(ut::make_unique<LocationECI>());
   AddMethod(ut::make_unique<VelocityECI>());
   AddMethod(ut::make_unique<TransformECI_PositionToRF>());
   AddMethod(ut::make_unique<TransformECI_VelocityToRF>());
   AddMethod(ut::make_unique<TransformRF_PositionToECI>());
   AddMethod(ut::make_unique<TransformRF_VelocityToECI>());
}

void WsfScriptLibrationPoint::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::LibrationPoint*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, Construct, 1, "WsfLibrationPoint", "WsfLibrationPointSystem")
{
   auto systemPtr    = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::LibrationPoint::System>();
   auto librationPtr = ut::make_unique<wsf::space::LibrationPoint>(*systemPtr);
   aReturnVal.SetPointer(new UtScriptRef(librationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, System, 0, "WsfLibrationPointSystem", "")
{
   auto systemPtr = ut::make_unique<wsf::space::LibrationPoint::System>(aObjectPtr->GetSystem());
   aReturnVal.SetPointer(new UtScriptRef{systemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, LocationECI, 2, "Vec3", "WsfLibrationPointSelection, Calendar")
{
   auto pointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::LibrationPoint::Point>();
   auto epoch    = aVarArgs[1].GetPointer()->GetAppObject<UtCalendar>();
   auto pos      = aObjectPtr->GetPositionECI(*pointPtr, *epoch);
   aReturnVal.SetPointer(UtScriptVec3::Create(pos));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, VelocityECI, 2, "Vec3", "WsfLibrationPointSelection, Calendar")
{
   auto pointPtr = aVarArgs[0].GetPointer()->GetAppObject<wsf::space::LibrationPoint::Point>();
   auto epoch    = aVarArgs[1].GetPointer()->GetAppObject<UtCalendar>();
   auto vel      = aObjectPtr->GetVelocityECI(*pointPtr, *epoch);
   aReturnVal.SetPointer(UtScriptVec3::Create(vel));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, TransformECI_PositionToRF, 2, "Vec3", "Calendar, Vec3")
{
   auto epoch  = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto posECI = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto posRF  = aObjectPtr->TransformPositionECItoRF(*epoch, *posECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(posRF));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, TransformECI_VelocityToRF, 3, "Vec3", "Calendar, Vec3, Vec3")
{
   auto epoch  = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto posECI = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto velECI = aVarArgs[2].GetPointer()->GetAppObject<UtVec3d>();
   auto velRF  = aObjectPtr->TransformVelocityECItoRF(*epoch, *posECI, *velECI);
   aReturnVal.SetPointer(UtScriptVec3::Create(velRF));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, TransformRF_PositionToECI, 2, "Vec3", "Calendar, Vec3")
{
   auto epoch  = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto posRF  = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto posECI = aObjectPtr->TransformPositionRFtoECI(*epoch, *posRF);
   aReturnVal.SetPointer(UtScriptVec3::Create(posECI));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPoint, wsf::space::LibrationPoint, TransformRF_VelocityToECI, 3, "Vec3", "Calendar, Vec3, Vec3")
{
   auto epoch  = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto posRF  = aVarArgs[1].GetPointer()->GetAppObject<UtVec3d>();
   auto velRF  = aVarArgs[2].GetPointer()->GetAppObject<UtVec3d>();
   auto velECI = aObjectPtr->TransformVelocityRFtoECI(*epoch, *posRF, *velRF);
   aReturnVal.SetPointer(UtScriptVec3::Create(velECI));
}

WsfScriptLibrationPointSystem::WsfScriptLibrationPointSystem(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLibrationPointSystem");
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<SUN_EARTH>());
   AddStaticMethod(ut::make_unique<EARTH_MOON>());
   AddStaticMethod(ut::make_unique<SUN_JUPITER>());
}

void WsfScriptLibrationPointSystem::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::LibrationPoint::System*>(aObjectPtr);
}

bool WsfScriptLibrationPointSystem::EqualTo(void* aLHS, void* aRHS)
{
   auto lhsPtr = static_cast<wsf::space::LibrationPoint::System*>(aLHS);
   auto rhsPtr = static_cast<wsf::space::LibrationPoint::System*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSystem,
                        wsf::space::LibrationPoint::System,
                        SUN_EARTH,
                        0,
                        "WsfLibrationPointSystem",
                        "")
{
   auto systemPtr = ut::make_unique<wsf::space::LibrationPoint::System>(wsf::space::LibrationPoint::System::cSUN_EARTH);
   aReturnVal.SetPointer(new UtScriptRef{systemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSystem,
                        wsf::space::LibrationPoint::System,
                        EARTH_MOON,
                        0,
                        "WsfLibrationPointSystem",
                        "")
{
   auto systemPtr = ut::make_unique<wsf::space::LibrationPoint::System>(wsf::space::LibrationPoint::System::cEARTH_MOON);
   aReturnVal.SetPointer(new UtScriptRef{systemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSystem,
                        wsf::space::LibrationPoint::System,
                        SUN_JUPITER,
                        0,
                        "WsfLibrationPointSystem",
                        "")
{
   auto systemPtr = ut::make_unique<wsf::space::LibrationPoint::System>(wsf::space::LibrationPoint::System::cSUN_JUPITER);
   aReturnVal.SetPointer(new UtScriptRef{systemPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

WsfScriptLibrationPointSelection::WsfScriptLibrationPointSelection(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfLibrationPointSelection");
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<L1>());
   AddStaticMethod(ut::make_unique<L2>());
   AddStaticMethod(ut::make_unique<L3>());
   AddStaticMethod(ut::make_unique<L4>());
   AddStaticMethod(ut::make_unique<L5>());
}

void WsfScriptLibrationPointSelection::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::LibrationPoint::Point*>(aObjectPtr);
}

bool WsfScriptLibrationPointSelection::EqualTo(void* aLHS, void* aRHS)
{
   auto lhsPtr = static_cast<wsf::space::LibrationPoint::Point*>(aLHS);
   auto rhsPtr = static_cast<wsf::space::LibrationPoint::Point*>(aRHS);
   return *lhsPtr == *rhsPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSelection, wsf::space::LibrationPoint::Point, L1, 0, "WsfLibrationPointSelection", "")
{
   auto pointPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(wsf::space::LibrationPoint::Point::cL1);
   aReturnVal.SetPointer(new UtScriptRef{pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSelection, wsf::space::LibrationPoint::Point, L2, 0, "WsfLibrationPointSelection", "")
{
   auto pointPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(wsf::space::LibrationPoint::Point::cL2);
   aReturnVal.SetPointer(new UtScriptRef{pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSelection, wsf::space::LibrationPoint::Point, L3, 0, "WsfLibrationPointSelection", "")
{
   auto pointPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(wsf::space::LibrationPoint::Point::cL3);
   aReturnVal.SetPointer(new UtScriptRef{pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSelection, wsf::space::LibrationPoint::Point, L4, 0, "WsfLibrationPointSelection", "")
{
   auto pointPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(wsf::space::LibrationPoint::Point::cL4);
   aReturnVal.SetPointer(new UtScriptRef{pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptLibrationPointSelection, wsf::space::LibrationPoint::Point, L5, 0, "WsfLibrationPointSelection", "")
{
   auto pointPtr = ut::make_unique<wsf::space::LibrationPoint::Point>(wsf::space::LibrationPoint::Point::cL5);
   aReturnVal.SetPointer(new UtScriptRef{pointPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE});
}
