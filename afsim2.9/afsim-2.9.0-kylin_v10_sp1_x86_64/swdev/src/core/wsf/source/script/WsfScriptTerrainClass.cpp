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

#include "script/WsfScriptTerrainClass.hpp"

#include "UtMemory.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

WsfScriptTerrainClass::WsfScriptTerrainClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfTerrain");

   mConstructible = true;

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<BathymetryElevApprox>());
   AddMethod(ut::make_unique<BathymetryElevInterp>());
   AddMethod(ut::make_unique<TerrainElevApprox>());
   AddMethod(ut::make_unique<TerrainElevInterp>());
   AddMethod(ut::make_unique<VegetationElevApprox>());
   AddMethod(ut::make_unique<VegetationElevInterp>());
   AddStaticMethod(ut::make_unique<MaskedByTerrain>());
   AddStaticMethod(ut::make_unique<MaskedByVegetation>());
   AddStaticMethod(ut::make_unique<MaskedByUnderwaterTerrain>());
}

WsfScriptTerrainClass::~WsfScriptTerrainClass() {}

#include "script/WsfScriptDefs.hpp"

// virtual
void* WsfScriptTerrainClass::Create(const UtScriptContext& aInstance)
{
   return new wsf::Terrain(WsfScriptContext::GetSIMULATION(aInstance)->GetTerrainInterface());
}

// virtual
void WsfScriptTerrainClass::Destroy(void* aObjectPtr)
{
   wsf::Terrain* objPtr = static_cast<wsf::Terrain*>(aObjectPtr);
   delete objPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, BathymetryElevApprox, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetBathymetryElevApprox(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, BathymetryElevInterp, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetBathymetryElevInterp(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, TerrainElevApprox, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetElevApprox(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, TerrainElevInterp, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetElevInterp(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, VegetationElevApprox, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetVegElevApprox(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, VegetationElevInterp, 2, "double", "double, double")
{
   double lat = aVarArgs[0].GetDouble();
   double lon = aVarArgs[1].GetDouble();
   float  elev;
   aObjectPtr->GetVegElevInterp(lat, lon, elev);
   aReturnVal.SetDouble((double)elev);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, MaskedByTerrain, 3, "bool", "WsfGeoPoint, WsfGeoPoint, double")
{
   WsfGeoPoint* srcPtr      = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* dstPtr      = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       radiusScale = aVarArgs[2].GetDouble();
   bool         masked      = SIMULATION->GetTerrainInterface()->MaskedByTerrain(srcPtr->GetLat(),
                                                                    srcPtr->GetLon(),
                                                                    srcPtr->GetAlt(),
                                                                    dstPtr->GetLat(),
                                                                    dstPtr->GetLon(),
                                                                    dstPtr->GetAlt(),
                                                                    srcPtr->GetDistanceFrom(*dstPtr),
                                                                    radiusScale);
   aReturnVal.SetBool(masked);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, MaskedByVegetation, 3, "bool", "WsfGeoPoint, WsfGeoPoint, double")
{
   WsfGeoPoint* srcPtr      = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* dstPtr      = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       radiusScale = aVarArgs[2].GetDouble();
   bool         masked      = SIMULATION->GetTerrainInterface()->MaskedByVegetation(srcPtr->GetLat(),
                                                                       srcPtr->GetLon(),
                                                                       srcPtr->GetAlt(),
                                                                       dstPtr->GetLat(),
                                                                       dstPtr->GetLon(),
                                                                       dstPtr->GetAlt(),
                                                                       srcPtr->GetDistanceFrom(*dstPtr),
                                                                       radiusScale);
   aReturnVal.SetBool(masked);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptTerrainClass, wsf::Terrain, MaskedByUnderwaterTerrain, 3, "bool", "WsfGeoPoint, WsfGeoPoint, double")
{
   WsfGeoPoint* srcPtr      = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* dstPtr      = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       radiusScale = aVarArgs[2].GetDouble();
   bool         masked      = SIMULATION->GetTerrainInterface()->MaskedByUnderwaterTerrain(srcPtr->GetLat(),
                                                                              srcPtr->GetLon(),
                                                                              srcPtr->GetAlt(),
                                                                              dstPtr->GetLat(),
                                                                              dstPtr->GetLon(),
                                                                              dstPtr->GetAlt(),
                                                                              srcPtr->GetDistanceFrom(*dstPtr),
                                                                              radiusScale);
   aReturnVal.SetBool(masked);
}
