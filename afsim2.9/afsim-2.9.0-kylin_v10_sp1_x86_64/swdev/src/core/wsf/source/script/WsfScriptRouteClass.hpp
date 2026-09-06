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

#ifndef WSFSCRIPTROUTECLASS_HPP
#define WSFSCRIPTROUTECLASS_HPP

#include "WsfScriptAuxDataUtil.hpp"
#include "WsfScriptObjectClass.hpp"

//! The script interface 'class'
class WSF_EXPORT WsfScriptRouteClass : public WsfScriptObjectClass
{
public:
   WsfScriptRouteClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptRouteClass() override;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;
   void  OnNewScriptRef(UtScriptRef& aReference) override;

   UT_DECLARE_SCRIPT_METHOD(Create_1); // Create(string aRouteName);
   UT_DECLARE_SCRIPT_METHOD(CopyGlobal);
   UT_DECLARE_SCRIPT_METHOD(FindGlobal);
   UT_DECLARE_SCRIPT_METHOD(GlobalRoutes);

   UT_DECLARE_SCRIPT_METHOD(Print);
   UT_DECLARE_SCRIPT_METHOD(Append_1); // Append(WsfWaypoint aWaypoint)
   UT_DECLARE_SCRIPT_METHOD(Append_2); // Append(WsfRoute aRoute)
   UT_DECLARE_SCRIPT_METHOD(Append_3); // Append(WsfGeoPoint aGeoPoint, double aSpeed)
   UT_DECLARE_SCRIPT_METHOD(Append_4); // Append(string aRouteName)
   UT_DECLARE_SCRIPT_METHOD(Insert_1);
   UT_DECLARE_SCRIPT_METHOD(Remove_1);
   UT_DECLARE_SCRIPT_METHOD(Copy);
   UT_DECLARE_SCRIPT_METHOD(SetAltitude_1);
   UT_DECLARE_SCRIPT_METHOD(SetAltitude_2);
   UT_DECLARE_SCRIPT_METHOD(GetSpeed);
   UT_DECLARE_SCRIPT_METHOD(SetSpeed_1);
   UT_DECLARE_SCRIPT_METHOD(SetSpeed_2);
   UT_DECLARE_SCRIPT_METHOD(Transform);
   UT_DECLARE_SCRIPT_METHOD(AddCallbackToWaypoint);
   UT_DECLARE_SCRIPT_METHOD(AddCallbackToLastWaypoint_1);
   UT_DECLARE_SCRIPT_METHOD(AddCallbackToLastWaypoint_2);
   UT_DECLARE_SCRIPT_METHOD(AddCallbackToLastWaypoint_3);
   UT_DECLARE_SCRIPT_METHOD(Size);
   UT_DECLARE_SCRIPT_METHOD(GetWaypointAt); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(Waypoint_1);
   UT_DECLARE_SCRIPT_METHOD(Waypoint_2);
   UT_DECLARE_SCRIPT_METHOD(SetPauseTime); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(PauseTime);    // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(TotalLength);
   UT_DECLARE_SCRIPT_METHOD(Intersect);

   UT_DECLARE_SCRIPT_METHOD(DistanceAlongRoute);
   UT_DECLARE_SCRIPT_METHOD(DistanceFromRoute);

   UT_DECLARE_SCRIPT_METHOD(LocationAtDistance);
   UT_DECLARE_SCRIPT_METHOD(SubrouteByDistance);

   UT_DECLARE_SCRIPT_METHOD(GetIterator);
   UT_DECLARE_SCRIPT_METHOD(Get); // NO_DOC | HIDDEN
   UT_DECLARE_SCRIPT_METHOD(Set); // NO_DOC | HIDDEN
   UT_DECLARE_SCRIPT_METHOD(Front);
   UT_DECLARE_SCRIPT_METHOD(Back);

   UT_DECLARE_SCRIPT_METHOD(TerrainProfiledClone);

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()
};

#endif
