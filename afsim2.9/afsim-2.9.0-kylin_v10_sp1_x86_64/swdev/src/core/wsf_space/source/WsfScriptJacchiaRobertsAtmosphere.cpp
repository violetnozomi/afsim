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

#include "WsfScriptJacchiaRobertsAtmosphere.hpp"

#include "WsfJacchiaRobertsAtmosphere.hpp"

WsfScriptJacchiaRobertsAtmosphere::WsfScriptJacchiaRobertsAtmosphere(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptAtmosphere(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfJacchiaRobertsAtmosphere");

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<SolarFlux>());
   AddMethod(ut::make_unique<AverageSolarFlux>());
   AddMethod(ut::make_unique<GeomagneticIndex>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJacchiaRobertsAtmosphere,
                        wsf::space::JacchiaRobertsAtmosphere,
                        Construct,
                        3,
                        "WsfJacchiaRobertsAtmosphere",
                        "double, double, double")
{
   auto   termPtr   = ut::make_unique<wsf::space::JacchiaRobertsAtmosphere>();
   double solarFlux = aVarArgs[0].GetDouble();
   if (solarFlux <= 0.0)
   {
      UT_SCRIPT_ABORT("Solar flux must be positive.");
   }
   double avgSolarFlux = aVarArgs[1].GetDouble();
   if (avgSolarFlux <= 0.0)
   {
      UT_SCRIPT_ABORT("Average solar flux must be positive.");
   }
   double geomagneticIndex = aVarArgs[2].GetDouble();
   if (geomagneticIndex < 0.0 || geomagneticIndex > 9.0)
   {
      UT_SCRIPT_ABORT("Geomagnetic Index must be in the range [0, 9.0].");
   }
   termPtr->SetSolarFlux(solarFlux);
   termPtr->SetAverageSolarFlux(avgSolarFlux);
   termPtr->SetGeomagneticIndex(geomagneticIndex);
   aReturnVal.SetPointer(new UtScriptRef(termPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJacchiaRobertsAtmosphere, wsf::space::JacchiaRobertsAtmosphere, SolarFlux, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetSolarFlux());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJacchiaRobertsAtmosphere, wsf::space::JacchiaRobertsAtmosphere, AverageSolarFlux, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAverageSolarFlux());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptJacchiaRobertsAtmosphere, wsf::space::JacchiaRobertsAtmosphere, GeomagneticIndex, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetGeomagneticIndex());
}
