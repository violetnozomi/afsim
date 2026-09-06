// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSpaceScriptOrientation.hpp"

#include "UtMemory.hpp"
#include "WsfPlatform.hpp"
#include "WsfSpaceOrientation.hpp"

namespace wsf
{

namespace space
{

namespace script
{

Orientation::Orientation(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfSpaceOrientation");

   mConstructible = false;
   mCloneable     = false;

   AddStaticMethod(ut::make_unique<NONE>());
   AddStaticMethod(ut::make_unique<NADIR_WITH_ECI_VELOCITY_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<NADIR_WITH_ECEF_VELOCITY_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<SOLAR_WITH_NADIR_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<NADIR_WITH_SOLAR_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<ECI_VELOCITY_WITH_NADIR_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<ECI_VELOCITY_WITH_SOLAR_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<POINT_WITH_ORBIT_PLANE_CONSTRAINT>());
   AddStaticMethod(ut::make_unique<ENTITY_WITH_SOLAR_CONSTRAINT_1>("ENTITY_WITH_SOLAR_CONSTRAINT"));
   AddStaticMethod(ut::make_unique<ENTITY_WITH_NADIR_CONSTRAINT_1>("ENTITY_WITH_NADIR_CONSTRAINT"));
   AddStaticMethod(ut::make_unique<ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_1>("ENTITY_WITH_ORBIT_PLANE_CONSTRAINT"));
   AddStaticMethod(ut::make_unique<ENTITY_WITH_SOLAR_CONSTRAINT_2>("ENTITY_WITH_SOLAR_CONSTRAINT"));
   AddStaticMethod(ut::make_unique<ENTITY_WITH_NADIR_CONSTRAINT_2>("ENTITY_WITH_NADIR_CONSTRAINT"));
   AddStaticMethod(ut::make_unique<ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_2>("ENTITY_WITH_ORBIT_PLANE_CONSTRAINT"));

   AddMethod(ut::make_unique<SwapAxes>());
}

// virtual
void Orientation::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::Orientation*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, NONE, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationNone>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, NADIR_WITH_ECI_VELOCITY_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationNadirECI_Velocity>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, NADIR_WITH_ECEF_VELOCITY_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationNadirECEF_Velocity>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, SOLAR_WITH_NADIR_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationSolarNadir>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, NADIR_WITH_SOLAR_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationNadirSolar>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ECI_VELOCITY_WITH_NADIR_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationECI_VelocityNadir>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ECI_VELOCITY_WITH_SOLAR_CONSTRAINT, 0, "WsfSpaceOrientation", "")
{
   auto orientationPtr = ut::make_unique<OrientationECI_VelocitySolar>();
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ENTITY_WITH_SOLAR_CONSTRAINT_1, 1, "WsfSpaceOrientation", "WsfPlatform")
{
   auto platformPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   auto orientationPtr = ut::make_unique<OrientationEntitySolar>();
   if (platformPtr != nullptr)
   {
      orientationPtr->SetEntityName(platformPtr->GetName());
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ENTITY_WITH_NADIR_CONSTRAINT_1, 1, "WsfSpaceOrientation", "WsfPlatform")
{
   auto platformPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   auto orientationPtr = ut::make_unique<OrientationEntityNadir>();
   if (platformPtr != nullptr)
   {
      orientationPtr->SetEntityName(platformPtr->GetName());
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation,
                        wsf::space::Orientation,
                        ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_1,
                        1,
                        "WsfSpaceOrientation",
                        "WsfPlatform")
{
   auto platformPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfPlatform>();
   auto orientationPtr = ut::make_unique<OrientationEntityOrbitPlane>();
   if (platformPtr != nullptr)
   {
      orientationPtr->SetEntityName(platformPtr->GetName());
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ENTITY_WITH_SOLAR_CONSTRAINT_2, 1, "WsfSpaceOrientation", "WsfTrackId")
{
   auto trackIdPtr     = aVarArgs[0].GetPointer()->GetAppObject<WsfTrackId>();
   auto orientationPtr = ut::make_unique<OrientationEntitySolar>();
   if (trackIdPtr != nullptr)
   {
      orientationPtr->SetTrackId(*trackIdPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, ENTITY_WITH_NADIR_CONSTRAINT_2, 1, "WsfSpaceOrientation", "WsfTrackId")
{
   auto trackIdPtr     = aVarArgs[0].GetPointer()->GetAppObject<WsfTrackId>();
   auto orientationPtr = ut::make_unique<OrientationEntityNadir>();
   if (trackIdPtr != nullptr)
   {
      orientationPtr->SetTrackId(*trackIdPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation,
                        wsf::space::Orientation,
                        ENTITY_WITH_ORBIT_PLANE_CONSTRAINT_2,
                        1,
                        "WsfSpaceOrientation",
                        "WsfTrackId")
{
   auto trackIdPtr     = aVarArgs[0].GetPointer()->GetAppObject<WsfTrackId>();
   auto orientationPtr = ut::make_unique<OrientationEntityOrbitPlane>();
   if (trackIdPtr != nullptr)
   {
      orientationPtr->SetTrackId(*trackIdPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, POINT_WITH_ORBIT_PLANE_CONSTRAINT, 1, "WsfSpaceOrientation", "WsfGeoPoint")
{
   auto geoPointPtr    = aVarArgs[0].GetPointer()->GetAppObject<WsfGeoPoint>();
   auto orientationPtr = ut::make_unique<OrientationPointOrbitPlane>();
   if (geoPointPtr != nullptr)
   {
      orientationPtr->SetGeoPoint(*geoPointPtr);
   }
   aReturnVal.SetPointer(new UtScriptRef(orientationPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(Orientation, wsf::space::Orientation, SwapAxes, 0, "void", "")
{
   aObjectPtr->SetDefaultX_Aligned(!aObjectPtr->GetDefaultX_Aligned());
}

} // namespace script

} // namespace space

} // namespace wsf
