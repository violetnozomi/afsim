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

#include "WsfScriptAtmosphere.hpp"

#include "UtCalendar.hpp"
#include "UtLLAPos.hpp"
#include "WsfAtmosphere.hpp"
#include "WsfGeoPoint.hpp"

WsfScriptAtmosphere::WsfScriptAtmosphere(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAtmosphere");

   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<ModelType>());
   AddMethod(ut::make_unique<Density>());
}


void WsfScriptAtmosphere::Destroy(void* aObjectPtr)
{
   delete static_cast<wsf::space::Atmosphere*>(aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphere, wsf::space::Atmosphere, ModelType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetType());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAtmosphere, wsf::space::Atmosphere, Density, 2, "double", "Calendar, WsfGeoPoint")
{
   auto     calPtr = aVarArgs[0].GetPointer()->GetAppObject<UtCalendar>();
   auto     geoPtr = aVarArgs[1].GetPointer()->GetAppObject<WsfGeoPoint>();
   UtLLAPos lla{geoPtr->GetLat(), geoPtr->GetLon(), geoPtr->GetAlt()};
   aReturnVal.SetDouble(aObjectPtr->GetDensity(*calPtr, lla));
}
