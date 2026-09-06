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

#include "WsfScriptWaypointClass.hpp"

#include <memory>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringUtil.hpp"
#include "WsfCallbackTypes.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfScriptCallback.hpp"
#include "WsfScriptContext.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"
#include "WsfWaypoint.hpp"

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfWaypoint::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptWaypointClass>(aClassName, aScriptTypesPtr);
}

WsfScriptWaypointClass::WsfScriptWaypointClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   mConstructible = true;
   mCloneable     = true;

   SetClassName("WsfWaypoint");

   AddMethod(ut::make_unique<Latitude>());
   AddMethod(ut::make_unique<Longitude>());
   AddMethod(ut::make_unique<Altitude>());
   AddMethod(ut::make_unique<AltitudeAGL>());
   AddMethod(ut::make_unique<AltitudeMSL>());
   AddMethod(ut::make_unique<SetAltitude>());
   AddMethod(ut::make_unique<SetAltitudePrevious>());
   AddMethod(ut::make_unique<Heading>());
   AddMethod(ut::make_unique<SetHeading>());
   AddMethod(ut::make_unique<SetLocation>());
   AddMethod(ut::make_unique<Index>());
   AddMethod(ut::make_unique<Location>());

   AddMethod(ut::make_unique<SetGoToLabel>());
   AddMethod(ut::make_unique<GoToLabel>());
   AddMethod(ut::make_unique<SetLabel>());
   AddMethod(ut::make_unique<Label>());
   AddMethod(ut::make_unique<SetCallback_1>("SetCallback"));
   AddMethod(ut::make_unique<SetCallback_2>("SetCallback"));
   AddMethod(ut::make_unique<SetCallback_3>("SetCallback"));

   AddMethod(ut::make_unique<Speed>());
   AddMethod(ut::make_unique<SetSpeed>());
   AddMethod(ut::make_unique<ClimbRate>());
   AddMethod(ut::make_unique<SetClimbRate>());
   AddMethod(ut::make_unique<LinearAcceleration>());
   AddMethod(ut::make_unique<SetLinearAcceleration>());
   AddMethod(ut::make_unique<RadialAcceleration>());
   AddMethod(ut::make_unique<SetRadialAcceleration>());
   AddMethod(ut::make_unique<Mode>());
   AddMethod(ut::make_unique<SetMode>());

   AddMethod(ut::make_unique<SetSwitch>());
   AddMethod(ut::make_unique<Switch>());
   AddMethod(ut::make_unique<SetSwitchTime>());
   AddMethod(ut::make_unique<SetSwitchDistance>());
   AddMethod(ut::make_unique<SetTimeToPoint>());
   AddMethod(ut::make_unique<SetEndOfPathOption>());
   AddStaticMethod(ut::make_unique<Create_1>("Create"));
   AddStaticMethod(ut::make_unique<Create_2>("Create"));
   AddStaticMethod(ut::make_unique<Create_3>("Create"));

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

#include "script/WsfScriptDefs.hpp"

// virtual
void* WsfScriptWaypointClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfWaypoint();
}

// virtual
void* WsfScriptWaypointClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfWaypoint*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptWaypointClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfWaypoint*>(aObjectPtr);
}

namespace
{

bool SetEndOfPath(WsfWaypoint& aWpt, const std::string& aEndOfPath)
{
   bool        set       = true;
   std::string endOfPath = aEndOfPath;
   UtStringUtil::ToUpper(endOfPath);
   // Set the end of path option
   if ((endOfPath == "MOVER_DEFAULT") || (endOfPath.empty()))
   {
      aWpt.SetEndOfPathOption(WsfPath::cEP_MOVER_DEFAULT);
   }
   else if (endOfPath == "EXTRAPOLATE")
   {
      aWpt.SetEndOfPathOption(WsfPath::cEP_EXTRAPOLATE);
   }
   else if (endOfPath == "STOP")
   {
      aWpt.SetEndOfPathOption(WsfPath::cEP_STOP);
   }
   else if (endOfPath == "REMOVE")
   {
      aWpt.SetEndOfPathOption(WsfPath::cEP_REMOVE);
   }
   else
   {
      set = false;
   }
   return set;
}

WsfWaypoint* CreatePrivate(double             aLat,
                           double             aLon,
                           double             aAlt,
                           double             aSpeed,
                           const std::string& aWaypointType,
                           const std::string& aEndOfPathOption)
{
   // Create the waypoint
   WsfWaypoint* pointPtr = new WsfWaypoint(aLat, aLon, aAlt, aSpeed);

   std::string wptType = aWaypointType;
   UtStringUtil::ToUpper(wptType);
   // Determine the type
   if ((wptType == "LATITUDE_AND_LONGITUDE") || (aWaypointType.empty()))
   {
      pointPtr->SetPointType(WsfWaypoint::cLATITUDE_AND_LONGITUDE);
   }
   else if (wptType == "RELATIVE_OFFSET")
   {
      pointPtr->SetPointType(WsfWaypoint::cRELATIVE_OFFSET);
   }
   else if (wptType == "ABSOLUTE_HEADING_DISTANCE")
   {
      pointPtr->SetPointType(WsfWaypoint::cABSOLUTE_HEADING | WsfWaypoint::cDISTANCE);
   }
   else if (wptType == "ABSOLUTE_HEADING_TIME")
   {
      pointPtr->SetPointType(WsfWaypoint::cABSOLUTE_HEADING | WsfWaypoint::cTIME);
   }
   else if (wptType == "ABSOLUTE_HEADING")
   {
      pointPtr->SetPointType(WsfWaypoint::cABSOLUTE_HEADING);
   }
   else if (wptType == "RELATIVE_HEADING_DISTANCE")
   {
      pointPtr->SetPointType(WsfWaypoint::cRELATIVE_HEADING | WsfWaypoint::cDISTANCE);
   }
   else if (wptType == "RELATIVE_HEADING_TIME")
   {
      pointPtr->SetPointType(WsfWaypoint::cRELATIVE_HEADING | WsfWaypoint::cTIME);
   }
   else if (wptType == "RELATIVE_HEADING")
   {
      pointPtr->SetPointType(WsfWaypoint::cRELATIVE_HEADING);
   }
   else if (wptType == "ALTITUDE_DISTANCE")
   {
      pointPtr->SetPointType(WsfWaypoint::cDISTANCE);
   }
   else if (wptType == "ALTITUDE_TIME")
   {
      pointPtr->SetPointType(WsfWaypoint::cTIME);
   }
   else if (wptType == "SPEED_DISTANCE")
   {
      pointPtr->SetPointType(WsfWaypoint::cDISTANCE);
   }
   else if (wptType == "SPEED_TIME")
   {
      pointPtr->SetPointType(WsfWaypoint::cTIME);
   }
   SetEndOfPath(*pointPtr, aEndOfPathOption);
   return pointPtr;
}
} // namespace


UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Latitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLat());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Longitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetLon());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Altitude, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetAlt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, AltitudeMSL, 0, "double", "")
{
   if (aObjectPtr->GetAltRef() == WsfPath::cALT_REF_AGL)
   {
      if (aObjectPtr->GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         wsf::Terrain terrain(SIMULATION->GetTerrainInterface());
         float        terrainHeight(0);
         terrain.GetElevInterp(aObjectPtr->GetLat(), aObjectPtr->GetLon(), terrainHeight);
         aReturnVal.SetDouble(aObjectPtr->GetAlt() + terrainHeight);
      }
      else
      {
         UT_SCRIPT_ABORT("Cannot get MSL altitude for non-LLA point with AGL altitude");
      }
   }
   else
   {
      aReturnVal.SetDouble(aObjectPtr->GetAlt());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, AltitudeAGL, 0, "double", "")
{
   if (aObjectPtr->GetAltRef() == WsfPath::cALT_REF_AGL)
   {
      aReturnVal.SetDouble(aObjectPtr->GetAlt());
   }
   else
   {
      if (aObjectPtr->GetPointType() & WsfWaypoint::cLATITUDE_AND_LONGITUDE)
      {
         wsf::Terrain terrain(SIMULATION->GetTerrainInterface());
         float        terrainHeight(0);
         terrain.GetElevInterp(aObjectPtr->GetLat(), aObjectPtr->GetLon(), terrainHeight);
         aReturnVal.SetDouble(aObjectPtr->GetAlt() - terrainHeight);
      }
      else
      {
         UT_SCRIPT_ABORT("Cannot get AGL altitude for non-LLA point with MSL altitude");
      }
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetAltitude, 2, "void", "double,bool")
{
   double alt   = aVarArgs[0].GetDouble();
   bool   isAgl = aVarArgs[1].GetBool();
   aObjectPtr->SetAlt(alt);
   aObjectPtr->SetAltRef(isAgl ? WsfPath::cALT_REF_AGL : WsfPath::cALT_REF_MSL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetAltitudePrevious, 0, "void", "")
{
   aObjectPtr->SetAlt(WsfPath::cUSE_PREVIOUS);
   aObjectPtr->SetAltRef(WsfPath::cALT_REF_MSL);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Heading, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeading() * UtMath::cDEG_PER_RAD);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetHeading, 1, "void", "double")
{
   aObjectPtr->SetHeading(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Location, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* geoPointPtr = new WsfGeoPoint(aObjectPtr->GetLat(), aObjectPtr->GetLon(), aObjectPtr->GetAlt());
   aReturnVal.SetPointer(new UtScriptRef(geoPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetLocation, 1, "void", "WsfGeoPoint")
{
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetLat(geoPointPtr->GetLat());
   aObjectPtr->SetLon(geoPointPtr->GetLon());
   aObjectPtr->SetAlt(geoPointPtr->GetAlt());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Index, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetPositionInRoute());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetGoToLabel, 1, "void", "string")
{
   aObjectPtr->SetGotoId(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, GoToLabel, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetGotoId().GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetLabel, 1, "void", "string")
{
   aObjectPtr->SetLabelId(WsfStringId(aVarArgs[0].GetString()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Label, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetLabelId().GetString());
}

// point.SetCallback("script_name");
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetCallback_1, 1, "void", "string")
{
   WsfCallback* callbackPtr = WsfCallbackTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
   if (callbackPtr != nullptr)
   {
      aObjectPtr->SetCallback(callbackPtr);
   }
   else
   {
      aObjectPtr->SetScriptId(WsfStringId(aVarArgs[0].GetString()));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Speed, 0, "double", "")
{
   aReturnVal.SetDouble((aObjectPtr->GetSpeed() == WsfPath::cUSE_PREVIOUS) ? -1.0 : aObjectPtr->GetSpeed());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetSpeed, 1, "void", "double")
{
   double speed = aVarArgs[0].GetDouble();
   aObjectPtr->SetSpeed((speed < 0.0) ? WsfPath::cUSE_PREVIOUS : speed);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, ClimbRate, 0, "double", "")
{
   aReturnVal.SetDouble((aObjectPtr->GetClimbRate() == WsfPath::cUSE_PREVIOUS) ? -1.0 : aObjectPtr->GetClimbRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetClimbRate, 1, "void", "double")
{
   double climbRate = aVarArgs[0].GetDouble();
   aObjectPtr->SetClimbRate((climbRate < 0.0) ? WsfPath::cUSE_PREVIOUS : climbRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, LinearAcceleration, 0, "double", "")
{
   aReturnVal.SetDouble((aObjectPtr->GetLinearAccel() == WsfPath::cUSE_PREVIOUS) ? -1.0 : aObjectPtr->GetLinearAccel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetLinearAcceleration, 1, "void", "double")
{
   double accel = aVarArgs[0].GetDouble();
   aObjectPtr->SetLinearAccel((accel < 0.0) ? WsfPath::cUSE_PREVIOUS : accel);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, RadialAcceleration, 0, "double", "")
{
   aReturnVal.SetDouble((aObjectPtr->GetRadialAccel() == WsfPath::cUSE_PREVIOUS) ? -1.0 : aObjectPtr->GetRadialAccel());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetRadialAcceleration, 1, "void", "double")
{
   double accel = aVarArgs[0].GetDouble();
   aObjectPtr->SetRadialAccel((accel < 0.0) ? WsfPath::cUSE_PREVIOUS : accel);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Mode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetNewModeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetMode, 1, "void", "string")
{
   aObjectPtr->SetNewModeId(aVarArgs[0].GetString());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetSwitch, 1, "void", "string")
{
   std::string switchType = aVarArgs[0].GetString();
   UtStringUtil::ToUpper(switchType);
   if (switchType == "APPROACH")
   {
      aObjectPtr->SetSwitch(WsfPath::cSWITCH_ON_APPROACH);
   }
   else if (switchType == "PASSING")
   {
      aObjectPtr->SetSwitch(WsfPath::cSWITCH_ON_PASSING);
   }
   else if (switchType == "DEFAULT")
   {
      aObjectPtr->SetSwitch(WsfPath::cSWITCH_MOVER_DEFAULT);
   }
   else
   {
      UT_SCRIPT_ABORT(std::string("Invalid switch type: ") + switchType);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Switch, 0, "string", "")
{
   switch (aObjectPtr->GetSwitch())
   {
   case WsfPath::cSWITCH_MOVER_DEFAULT:
      aReturnVal.SetString("default");
      break;
   case WsfPath::cSWITCH_ON_TIME:
      aReturnVal.SetString("time");
      break;
   case WsfPath::cSWITCH_ON_DISTANCE:
      aReturnVal.SetString("distance");
      break;
   case WsfPath::cSWITCH_ON_APPROACH:
      aReturnVal.SetString("approach");
      break;
   case WsfPath::cSWITCH_ON_PASSING:
      aReturnVal.SetString("passing");
      break;
   case WsfPath::cSWITCH_EXTERNAL:
      aReturnVal.SetString("external");
      break;
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetSwitchTime, 1, "void", "double")
{
   aObjectPtr->SetDistanceOrTime(aVarArgs[0].GetDouble());
   aObjectPtr->SetSwitch(WsfPath::cSWITCH_ON_TIME);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetSwitchDistance, 1, "void", "double")
{
   aObjectPtr->SetDistanceOrTime(aVarArgs[0].GetDouble());
   aObjectPtr->SetSwitch(WsfPath::cSWITCH_ON_DISTANCE);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetTimeToPoint, 1, "void", "double")
{
   if ((aObjectPtr->GetPointType() & WsfWaypoint::cLOCATION_MASK) != 0)
   {
      aObjectPtr->SetPointType(aObjectPtr->GetPointType() | WsfWaypoint::cTIME_TO_POINT);
      aObjectPtr->SetDistanceOrTime(aVarArgs[0].GetDouble());
   }
}

namespace
{

bool SetEndOfPath(WsfWaypoint& aWpt, const std::string& aEndOfPath);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetEndOfPathOption, 1, "void", "string")
{
   const std::string option = aVarArgs[0].GetString();
   if (!SetEndOfPath(*aObjectPtr, option))
   {
      UT_SCRIPT_ABORT("Invalid end of path option: " + option);
   }
}

// point.SetCallback("script_name", "processor_name");
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetCallback_2, 2, "void", "string, string")
{
   aObjectPtr->SetCallback(new WsfScriptCallback(aVarArgs[0].GetString(), aVarArgs[1].GetString()));
}

// point.SetCallback("script_name", "processor_name");
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, SetCallback_3, 1, "void", "Method")
{
   UtScriptMethod* methodPtr = static_cast<UtScriptMethod*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->SetCallback(new WsfScriptCallback(*methodPtr));
}

//! WsfWaypoint waypoint = Create(double aLat, double aLon, double aAlt, double aSpeed, string aPointType);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Create_1, 5, "WsfWaypoint", "double, double, double, double, string")
{
   WsfWaypoint* pointPtr = CreatePrivate(aVarArgs[0].GetDouble(),
                                         aVarArgs[1].GetDouble(),
                                         aVarArgs[2].GetDouble(),
                                         aVarArgs[3].GetDouble(),
                                         aVarArgs[4].GetString(),
                                         "");

   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! WsfWaypoint waypoint = Create(double aLat, double aLon, double aAlt, double aSpeed, string aPointType, string aEndOfPathOption);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Create_2, 6, "WsfWaypoint", "double, double, double, double, string, string")
{
   WsfWaypoint* pointPtr = CreatePrivate(aVarArgs[0].GetDouble(),
                                         aVarArgs[1].GetDouble(),
                                         aVarArgs[2].GetDouble(),
                                         aVarArgs[3].GetDouble(),
                                         aVarArgs[4].GetString(),
                                         aVarArgs[5].GetString());

   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! WsfWaypoint waypoint = Create(WsfGeoPoint aGeoPoint, double speed);
UT_DEFINE_SCRIPT_METHOD(WsfScriptWaypointClass, WsfWaypoint, Create_3, 2, "WsfWaypoint", "WsfGeoPoint, double")
{
   // Default return
   WsfWaypoint* waypointPtr = nullptr;

   // Get the geo point
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());

   // Create the waypoint
   if (geoPointPtr != nullptr)
   {
      double speed = aVarArgs[1].GetDouble();
      if (speed < 0.0)
      {
         speed = WsfPath::cDOUBLE_NOT_SET;
      }
      waypointPtr = new WsfWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), speed);
   }

   aReturnVal.SetPointer(new UtScriptRef(waypointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptWaypointClass, WsfWaypoint)
