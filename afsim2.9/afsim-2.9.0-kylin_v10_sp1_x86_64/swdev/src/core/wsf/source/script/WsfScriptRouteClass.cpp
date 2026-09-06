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

#include "WsfScriptRouteClass.hpp"

#include <memory>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptClassDefine.hpp"
#include "WsfCallback.hpp"
#include "WsfCallbackTypes.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfRoute.hpp"
#include "WsfRouteTypes.hpp"
#include "WsfScriptCallback.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrainProfiler.hpp"

static const int cCONST_ROUTE_ID = 5;
// This define should be used at the beginning of any script method which modifies the route.
#define FORBID_CONSTANT_ROUTE                                          \
   if (aReference.GetAuxData() == cCONST_ROUTE_ID)                     \
   {                                                                   \
      UT_SCRIPT_ABORT("Route may not be modified, use Copy() first."); \
   }
static UtScriptClass* gRouteClassPtr = nullptr;

//! Return a script reference to this route, managed by the application.
UtScriptRef* WsfRoute::ScriptRef()
{
   return new UtScriptRef(this, gRouteClassPtr, GetReferenceCount());
}

//! Return a script reference to this route, managed by the scripting language.
UtScriptRef* WsfRoute::ScriptRefManage()
{
   return new UtScriptRef(this, gRouteClassPtr, UtScriptRef::cMANAGE);
}

//! Return a const script reference to this route, managed by the application.
//! Script accessors will be forbidden from modifying this object.
UtScriptRef* WsfRoute::ConstScriptRef() const
{
   WsfRoute*    thisPtr = const_cast<WsfRoute*>(this);
   UtScriptRef* refPtr  = thisPtr->ScriptRef();
   refPtr->SetAuxData(cCONST_ROUTE_ID);
   return refPtr;
}

WsfScriptRouteClass::WsfScriptRouteClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRoute");

   mConstructible = true;
   mCloneable     = true;
   mContainer     = true;
   SetContainerKeyTypeId("int");
   SetContainerDataTypeId("WsfWaypoint");

   AddStaticMethod(ut::make_unique<Create_1>("Create")); // Create(string aRouteName);
   AddStaticMethod(ut::make_unique<CopyGlobal>());       // CopyGlobal(string aGlobalRouteName);
   AddStaticMethod(ut::make_unique<FindGlobal>());       // FindGlobal(string aGlobalRouteName);
   AddStaticMethod(ut::make_unique<GlobalRoutes>());

   AddMethod(ut::make_unique<Print>("Print"));
   AddMethod(ut::make_unique<Append_1>("Append")); // Append(WsfWaypoint aWaypoint)
   AddMethod(ut::make_unique<Append_2>("Append")); // Append(WsfRoute aRoute)
   AddMethod(ut::make_unique<Append_3>("Append")); // Append(WsfGeoPoint aGeoPoint, double aSpeed)
   AddMethod(ut::make_unique<Append_4>("Append")); // Append(string aRouteName)
   AddMethod(ut::make_unique<Insert_1>("Insert")); // Insert(int aBeforeIndex, WsfWaypoint aNewWaypoint)
   AddMethod(ut::make_unique<Remove_1>("Remove")); // Remove(int aIndex)
   AddMethod(ut::make_unique<Copy>());
   AddMethod(ut::make_unique<SetAltitude_1>("SetAltitude"));
   AddMethod(ut::make_unique<SetAltitude_2>("SetAltitude"));
   AddMethod(ut::make_unique<GetSpeed>());
   AddMethod(ut::make_unique<SetSpeed_1>("SetSpeed"));
   AddMethod(ut::make_unique<SetSpeed_2>("SetSpeed"));
   AddMethod(ut::make_unique<Transform>());

   AddMethod(ut::make_unique<AddCallbackToWaypoint>()); // string aCallbackName, int aWaypointIndex
   AddMethod(ut::make_unique<AddCallbackToLastWaypoint_1>("AddCallbackToLastWaypoint")); // string aCallbackName
   AddMethod(ut::make_unique<AddCallbackToLastWaypoint_2>("AddCallbackToLastWaypoint")); // string aCallbackName, string
                                                                                         // aProcessorName
   AddMethod(ut::make_unique<AddCallbackToLastWaypoint_3>("AddCallbackToLastWaypoint")); // Method aCallback

   AddMethod(ut::make_unique<Size>("Size"));
   AddMethod(ut::make_unique<Size>("GetSize")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<GetWaypointAt>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Waypoint_1>("Waypoint"));
   AddMethod(ut::make_unique<Waypoint_2>("Waypoint"));
   AddMethod(ut::make_unique<SetPauseTime>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<PauseTime>());    // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TotalLength>());
   AddMethod(ut::make_unique<Intersect>());

   AddMethod(ut::make_unique<DistanceAlongRoute>());
   AddMethod(ut::make_unique<DistanceFromRoute>());
   AddMethod(ut::make_unique<LocationAtDistance>());
   AddMethod(ut::make_unique<SubrouteByDistance>());

   AddMethod(ut::make_unique<GetIterator>());
   AddMethod(ut::make_unique<Get>()); // NO_DOC | HIDDEN
   AddMethod(ut::make_unique<Set>()); // NO_DOC | HIDDEN
   AddMethod(ut::make_unique<Front>());
   AddMethod(ut::make_unique<Back>());

   AddMethod(ut::make_unique<TerrainProfiledClone>());

   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);

   assert(gRouteClassPtr == nullptr);
   gRouteClassPtr = this;
};

WsfScriptRouteClass::~WsfScriptRouteClass()
{
   if (gRouteClassPtr == this)
   {
      gRouteClassPtr = nullptr;
   }
}

// virtual
void* WsfScriptRouteClass::Create(const UtScriptContext& /*aContext*/)
{
   return new WsfRoute();
}

// virtual
void* WsfScriptRouteClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfRoute*>(aObjectPtr)->Clone();
}

// virtual
void WsfScriptRouteClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfRoute*>(aObjectPtr);
}

// virtual
void WsfScriptRouteClass::OnNewScriptRef(UtScriptRef& aReference)
{
   // If an unmanaged reference is made, change to externally managed
   if (aReference.GetMemManagement() != UtScriptRef::cMANAGE)
   {
      WsfRoute* routePtr = static_cast<WsfRoute*>(aReference.GetAppObject());
      if (routePtr != nullptr)
      {
         aReference.SetExternalReference(routePtr->GetReferenceCount());
      }
   }
}

//! Print()
#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Print, 0, "void", "")
{
   auto out = ut::log::info() << "Route:";
   aObjectPtr->Print(out);
}

//! Append(WsfWaypoint aWaypoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Append_1, 1, "void", "WsfWaypoint")
{
   FORBID_CONSTANT_ROUTE
   WsfWaypoint* waypointPtr = static_cast<WsfWaypoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Append(*waypointPtr);
}

//! Append(WsfRoute aRoute)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Append_2, 1, "void", "WsfRoute")
{
   FORBID_CONSTANT_ROUTE
   WsfRoute* routePtr = static_cast<WsfRoute*>(aVarArgs[0].GetPointer()->GetAppObject());
   aObjectPtr->Append(*routePtr);
}

//! Append(WsfGeoPoint aGeoPoint, double aSpeed)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Append_3, 2, "void", "WsfGeoPoint, double")
{
   FORBID_CONSTANT_ROUTE
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       speed       = aVarArgs[1].GetDouble();
   if (speed < 0.0)
   {
      speed = WsfPath::cDOUBLE_NOT_SET;
   }
   WsfWaypoint* waypointPtr = new WsfWaypoint(geoPointPtr->GetLat(), geoPointPtr->GetLon(), geoPointPtr->GetAlt(), speed);
   aObjectPtr->Append(*waypointPtr);
   delete waypointPtr;
}

//! Append(string aRouteName)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Append_4, 1, "void", "string")
{
   FORBID_CONSTANT_ROUTE
   WsfRoute* routePtr = WsfRouteTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (routePtr != nullptr)
   {
      aObjectPtr->Append(*routePtr);
   }
}

//! Insert(int aBeforeIndex, WsfWaypoint aNewWaypoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Insert_1, 2, "void", "int, WsfWaypoint")
{
   FORBID_CONSTANT_ROUTE
   int          wptIndex = aVarArgs[0].GetInt();
   WsfWaypoint* wptPtr   = static_cast<WsfWaypoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   if (wptIndex >= 0 && wptIndex < int(aObjectPtr->GetSize()))
   {
      aObjectPtr->Insert(wptIndex, *wptPtr);
   }
   else
   {
      auto out = ut::log::error() << "Index out of range.";
      out.AddNote() << "Script: WsfRoute.Insert()";
      out.AddNote() << "Index: " << wptIndex;
      out.AddNote() << "Size: " << aObjectPtr->GetSize();
   }
}

//! Remove(int aIndex)
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Remove_1, 1, "void", "int")
{
   FORBID_CONSTANT_ROUTE
   int wptIndex = aVarArgs[0].GetInt();
   if (wptIndex >= 0 && wptIndex < (int)aObjectPtr->GetSize())
   {
      aObjectPtr->Remove(wptIndex);
   }
   else
   {
      auto out = ut::log::error() << "Index out of range.";
      out.AddNote() << "Script: WsfRoute.Remove()";
      out.AddNote() << "Index: " << wptIndex;
      out.AddNote() << "Size: " << aObjectPtr->GetSize();
   }
}

//! WsfRoute route = Copy();
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Copy, 0, "WsfRoute", "")
{
   // Get a pointer to the current route
   aReturnVal.SetPointer(aObjectPtr->Clone()->ScriptRefManage());
}

//! WsfRoute route = CopyGlobal(string aGlobalRouteName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, CopyGlobal, 1, "WsfRoute", "string")
{
   WsfRoute* routePtr = WsfRouteTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
   aReturnVal.SetPointer(routePtr->ScriptRefManage());
}

//! (unmodifiable) WsfRoute route = FindGlobal(string aGlobalRouteName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, FindGlobal, 1, "WsfRoute", "string")
{
   WsfRoute* routePtr = WsfRouteTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (routePtr != nullptr)
   {
      aReturnVal.SetPointer(routePtr->ConstScriptRef());
      // Marking this route as unmodifiable.
      aReturnVal.GetPointer()->SetAuxData(cCONST_ROUTE_ID);
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

//! WsfRoute route = Create(string aRouteName);
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Create_1, 1, "WsfRoute", "string")
{
   // Get a pointer to the already defined route using the name.
   WsfRoute* routePtr = new WsfRoute(aVarArgs[0].GetString());
   aReturnVal.SetPointer(routePtr->ScriptRefManage());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, GlobalRoutes, 0, "Array<string>", "")
{
   auto                   resultVecPtr = ut::make_unique<ut::script::DataList>();
   std::vector<WsfRoute*> globalRoutes;
   WsfRouteTypes::Get(*SCENARIO).GetCurrentTypes(globalRoutes);
   for (const auto& route : globalRoutes)
   {
      if (route->IsGlobalRoute())
      {
         resultVecPtr->emplace_back(route->GetName());
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//! SetAltitude(double aAltitude)
//! The altitude is applied to all waypoints in the route. Altitude reference not changed.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SetAltitude_1, 1, "void", "double")
{
   FORBID_CONSTANT_ROUTE
   double altitude = aVarArgs[0].GetDouble();
   for (unsigned int i = 0; i < aObjectPtr->GetSize(); ++i)
   {
      (*aObjectPtr)[i].SetAlt(altitude);
   }
}

//! SetAltitude(double aAltitude, string aAltitudeRef)
//! The altitude is applied to all waypoints in the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SetAltitude_2, 2, "void", "double, string")
{
   FORBID_CONSTANT_ROUTE
   double      altitude    = aVarArgs[0].GetDouble();
   std::string altitudeRef = aVarArgs[0].GetString();
   for (unsigned int i = 0; i < aObjectPtr->GetSize(); ++i)
   {
      (*aObjectPtr)[i].SetAlt(altitude);
      if (altitudeRef == "DEFAULT")
      {
         (*aObjectPtr)[i].SetAltRef(WsfPath::cALT_REF_MOVER_DEFAULT);
      }
      else if (altitudeRef == "MSL")
      {
         (*aObjectPtr)[i].SetAltRef(WsfPath::cALT_REF_MSL);
      }
      else if (altitudeRef == "AGL")
      {
         (*aObjectPtr)[i].SetAltRef(WsfPath::cALT_REF_AGL);
      }
      // else if (altitudeRef == "")
      //{
      //    do nothing; could set it to cALT_REF_DEFAULT
      // }
   }
}

//! GetSpeed(int index)
//! returns the speed of the index waypoint
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, GetSpeed, 1, "double", "int")
{
   int index = aVarArgs[0].GetInt();
   aReturnVal.SetDouble((*aObjectPtr)[index].GetSpeed());
}

//! SetSpeed(double aSpeed)
//! The speed is applied to all waypoints in the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SetSpeed_1, 1, "void", "double")
{
   FORBID_CONSTANT_ROUTE
   double speed = aVarArgs[0].GetDouble();
   for (unsigned int i = 0; i < aObjectPtr->GetSize(); ++i)
   {
      (*aObjectPtr)[i].SetSpeed(speed);
   }
}

//! SetSpeed(int index, double aSpeed)
//! The speed is applied to the index waypoint in the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SetSpeed_2, 2, "void", "int, double")
{
   FORBID_CONSTANT_ROUTE
   double speed = aVarArgs[1].GetDouble();
   int    index = aVarArgs[0].GetInt();
   (*aObjectPtr)[index].SetSpeed(speed);
}

//! Transform(double aLatitude, double aLongitude, double aHeading)
//! Note: All units are degrees.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Transform, 3, "void", "double, double, double")
{
   FORBID_CONSTANT_ROUTE
   double refLat  = aVarArgs[0].GetDouble();
   double refLon  = aVarArgs[1].GetDouble();
   double heading = aVarArgs[2].GetDouble();

   aObjectPtr->Transform(refLat, refLon, heading);
}

//! AddCallbackToWaypoint adds the specified callback to the waypoint located at the specified index.
//! If the callback exists in the callback type list, it is cloned and added to the waypoint.
//! Otherwise, the callback is added as an action id and is assumed to exist on the platform.
//! aVarArgs[0] The callback's name.
//! aVarArgs[1] The waypoint's index in the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, AddCallbackToWaypoint, 2, "void", "string, int")
{
   FORBID_CONSTANT_ROUTE
   WsfStringId callbackId    = aVarArgs[0].GetString();
   int         waypointIndex = aVarArgs[1].GetInt();
   if (waypointIndex < static_cast<int>(aObjectPtr->GetSize()))
   {
      WsfCallback* callbackPtr = WsfCallbackTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
      if (callbackPtr != nullptr)
      {
         (*aObjectPtr)[waypointIndex].SetCallback(callbackPtr);
      }
      else
      {
         (*aObjectPtr)[waypointIndex].SetScriptId(callbackId);
      }
   }
}

//! AddCallbackToLastWaypoint adds the specified callback to the last waypoint in the route.
//! If the callback exists in the callback type list, it is cloned and added to the waypoint.
//! Otherwise, the callback is added as an action id and is assumed to exist on the platform.
//! aVarArgs[0] The callback's name.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, AddCallbackToLastWaypoint_1, 1, "void", "string")
{
   FORBID_CONSTANT_ROUTE
   WsfStringId callbackId    = aVarArgs[0].GetString();
   int         waypointIndex = static_cast<int>(aObjectPtr->GetSize()) - 1;
   if (waypointIndex >= 0)
   {
      WsfCallback* callbackPtr = WsfCallbackTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
      if (callbackPtr != nullptr)
      {
         (*aObjectPtr)[waypointIndex].SetCallback(callbackPtr);
      }
      else
      {
         (*aObjectPtr)[waypointIndex].SetScriptId(callbackId);
      }
   }
}

// route.AddCallbackToLastWaypoint("script_name", "processor_name");
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, AddCallbackToLastWaypoint_2, 2, "void", "string, string")
{
   FORBID_CONSTANT_ROUTE
   int waypointIndex = static_cast<int>(aObjectPtr->GetSize()) - 1;
   if (waypointIndex >= 0)
   {
      (*aObjectPtr)[waypointIndex].SetCallback(new WsfScriptCallback(aVarArgs[0].GetString(), aVarArgs[1].GetString()));
   }
}

// route.AddCallbackToLastWaypoint(Method);
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, AddCallbackToLastWaypoint_3, 1, "void", "Method")
{
   FORBID_CONSTANT_ROUTE
   int waypointIndex = static_cast<int>(aObjectPtr->GetSize()) - 1;
   if (waypointIndex >= 0)
   {
      (*aObjectPtr)[waypointIndex].SetCallback(
         new WsfScriptCallback(*(UtScriptMethod*)aVarArgs[0].GetPointer()->GetAppObject()));
   }
}

//! Size Returns the number of waypoints in the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Size, 0, "int", "")
{
   aReturnVal.SetInt((*aObjectPtr).GetSize());
}

//! GetWaypointAt Return the waypoint at the specified index.
//!  Deprecated in favor of Waypoint()
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, GetWaypointAt, 1, "WsfGeoPoint", "int") // NO_DOC | DEPRECATED
{
   static bool deprecationShown = false;
   if (!deprecationShown)
   {
      auto out = ut::log::warning() << "WsfRoute.GetWaypointAt() is deprecated.";
      out.AddNote() << "Please use Waypoint().";
      deprecationShown = true;
   }

   unsigned int waypointIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (aObjectPtr->GetSize() > waypointIndex)
   {
      WsfWaypoint* wpPtr = &(*aObjectPtr)[waypointIndex];
      double       lat;
      double       lon;
      double       alt;
      wpPtr->GetLocationLLA(lat, lon, alt);
      WsfGeoPoint* geoPointPtr = new WsfGeoPoint(lat, lon, alt);
      aReturnVal.SetPointer(new UtScriptRef(geoPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

//! Waypoint() Return the location at the waypoint at the specified index.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Waypoint_1, 1, "WsfWaypoint", "int")
{
   int          index       = aVarArgs[0].GetInt();
   UtScriptRef* wayptRefPtr = nullptr; // Script reference to the waypoint
   if (index >= 0 && index < (int)aObjectPtr->GetSize())
   {
      WsfWaypoint& wpt = (*aObjectPtr)[index];
      wayptRefPtr      = new UtScriptRef(&wpt, aReturnClassPtr, aObjectPtr->GetWaypointReferenceCount());
   }
   else
   {
      wayptRefPtr = new UtScriptRef(nullptr, aReturnClassPtr);
   }
   aReturnVal.SetPointer(wayptRefPtr);
}

//! Waypoint() Return the location at the waypoint with the specified label.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Waypoint_2, 1, "WsfWaypoint", "string")
{
   int          indexAtLabel = aObjectPtr->GetWaypointIndexAt(aVarArgs[0].GetString());
   UtScriptRef* wayptRefPtr  = nullptr; // Script reference to the waypoint
   if (indexAtLabel < (int)aObjectPtr->GetSize())
   {
      WsfWaypoint& wpt = (*aObjectPtr)[indexAtLabel];
      wayptRefPtr      = new UtScriptRef(&wpt, aReturnClassPtr, aObjectPtr->GetWaypointReferenceCount());
   }
   else
   {
      wayptRefPtr = new UtScriptRef(nullptr, aReturnClassPtr);
   }
   aReturnVal.SetPointer(wayptRefPtr);
}

//! PauseTime Returns the pause_time for the waypoint
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, PauseTime, 1, "double", "int") // NO_DOC | DEPRECATED
{
   static bool deprecationShown = false;
   if (!deprecationShown)
   {
      ut::log::warning() << "WsfRoute.PauseTime() is deprecated.";
      deprecationShown = true;
   }

   double       pauseTime     = 0.0;
   unsigned int waypointIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (aObjectPtr->GetSize() > waypointIndex)
   {
      WsfWaypoint& wpt = (*aObjectPtr)[waypointIndex];
      pauseTime        = wpt.GetPauseTime();
   }
   aReturnVal.SetDouble(pauseTime);
}

//! SetPauseTime Sets the pause time for a waypoint.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SetPauseTime, 2, "void", "int, double") // NO_DOC | DEPRECATED
{
   static bool deprecationShown = false;
   if (!deprecationShown)
   {
      ut::log::warning() << "WsfRoute.SetPauseTime() is deprecated.";
      deprecationShown = true;
   }

   unsigned int waypointIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (aObjectPtr->GetSize() > waypointIndex)
   {
      WsfWaypoint& wpt = (*aObjectPtr)[waypointIndex];
      wpt.SetPauseTime(aVarArgs[1].GetDouble());
   }
}

//! TotalLength Returns the total length of the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, TotalLength, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetTotalLength());
}

//! Intersects two routes returning a list of intersection points.
//! Returns an array containing 3 elements for each intersection:
//! [ int waypoint_index_1, int waypoint_index_2, WsfGeoPoint location ]
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Intersect, 1, "Array<Object>", "WsfRoute")
{
   FORBID_CONSTANT_ROUTE
   WsfRoute*                                routePtr = static_cast<WsfRoute*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<WsfRoute::RouteIntersection> intersections;
   aObjectPtr->Intersect(*routePtr, intersections);
   UtScriptDataList*     resultVecPtr     = new UtScriptDataList;
   static UtScriptClass* geoPointClassPtr = aContext.GetTypes()->GetClass("WsfGeoPoint");
   for (WsfRoute::RouteIntersection& inter : intersections)
   {
      resultVecPtr->emplace_back(inter.mWaypointIndex1);
      resultVecPtr->emplace_back(inter.mWaypointIndex2);
      WsfGeoPoint* geoPointPtr          = new WsfGeoPoint(inter.mPoint.GetLat(), inter.mPoint.GetLon(), 0.0);
      UtScriptRef* geoPointScriptRefPtr = new UtScriptRef(geoPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, DistanceAlongRoute, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       point[2]    = {geoPointPtr->GetLat(), geoPointPtr->GetLon()};
   double       distanceFrom, distanceAlong;
   if (aObjectPtr->ProjectOntoRouteFast(point, distanceFrom, distanceAlong) != -1)
   {
      aReturnVal.SetDouble(distanceAlong);
   }
   else
   {
      aReturnVal.SetDouble(-1.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, DistanceFromRoute, 1, "double", "WsfGeoPoint")
{
   WsfGeoPoint* geoPointPtr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       point[2]    = {geoPointPtr->GetLat(), geoPointPtr->GetLon()};
   double       distanceFrom, distanceAlong;
   if (aObjectPtr->ProjectOntoRouteFast(point, distanceFrom, distanceAlong) != -1)
   {
      aReturnVal.SetDouble(distanceFrom);
   }
   else
   {
      aReturnVal.SetDouble(-1.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, LocationAtDistance, 1, "WsfGeoPoint", "double")
{
   double distance = UtMath::Limit(aVarArgs[0].GetDouble(), 0.0, aObjectPtr->GetTotalLength() - 1.0E-6);
   double location[2];
   int    segmentIndex;
   if (aObjectPtr->PointAlongRoute(distance, location, segmentIndex))
   {
      WsfGeoPoint* geoPointPtr = new WsfGeoPoint(location[0], location[1], 0.0);
      aReturnVal.SetPointer(new UtScriptRef(geoPointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

// Returns a sub-route between two distances from the start of the route.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, SubrouteByDistance, 2, "WsfRoute", "double, double")
{
   double distance1 = aVarArgs[0].GetDouble();
   double distance2 = aVarArgs[1].GetDouble();
   distance1        = UtMath::Limit(distance1, 0.0, aObjectPtr->GetTotalLength() - 1.0E-6);
   distance2        = UtMath::Limit(distance2, 0.0, aObjectPtr->GetTotalLength() - 1.0E-6);
   // Wait until we are don't to reverse the route if necessary
   bool isReverse = distance1 > distance2;
   if (isReverse)
   {
      std::swap(distance1, distance2);
   }
   double location1[2];
   double location2[2];
   int    segmentIndex1, segmentIndex2;
   if (aObjectPtr->PointAlongRoute(distance1, location1, segmentIndex1) &&
       aObjectPtr->PointAlongRoute(distance2, location2, segmentIndex2))
   {
      WsfRoute* newRoutePtr = new WsfRoute;
      if (segmentIndex1 != segmentIndex2)
      {
         aObjectPtr->GetSubroute(segmentIndex1 + 1, segmentIndex2, *newRoutePtr);
      }
      WsfWaypoint firstPt = (*aObjectPtr)[segmentIndex1 + 1];
      firstPt.SetLat(location1[0]);
      firstPt.SetLon(location1[1]);
      newRoutePtr->Insert(0, firstPt);
      WsfWaypoint lastPt = (*aObjectPtr)[segmentIndex2];
      lastPt.SetLat(location2[0]);
      lastPt.SetLon(location2[1]);
      newRoutePtr->Append(lastPt);
      if (isReverse)
      {
         WsfRoute inverseRoute;
         newRoutePtr->GetSubroute((int)newRoutePtr->GetSize(), 0, inverseRoute);
         std::swap(inverseRoute, *newRoutePtr);
      }
      aReturnVal.SetPointer(newRoutePtr->ScriptRefManage());
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}

// Return a reference to a waypoint owned by this route.  The reference may become invalid
// if the route is changed.
UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Get, 1, "WsfWaypoint", "int") // NO_DOC | HIDDEN
{
   int index = aVarArgs[0].GetInt();
   if (index >= 0 && index < (int)aObjectPtr->GetSize())
   {
      WsfWaypoint& wpt = (*aObjectPtr)[index];
      // GetWaypointReferenceCount() allows for safe access to waypoint references
      UtScriptRef* wptRef = new UtScriptRef(&wpt, aReturnClassPtr, aObjectPtr->GetWaypointReferenceCount());
      aReturnVal.SetPointer(wptRef);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Set, 2, "void", "int, WsfWaypoint") // NO_DOC | HIDDEN
{
   FORBID_CONSTANT_ROUTE
   int index = aVarArgs[0].GetInt();
   if (index >= 0 && index < (int)aObjectPtr->GetSize())
   {
      (*aObjectPtr)[index] = *static_cast<WsfWaypoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Front, 0, "WsfWaypoint", "")
{
   int index = 0;
   if (index < (int)aObjectPtr->GetSize())
   {
      WsfWaypoint& wpt = (*aObjectPtr)[index];
      // GetWaypointReferenceCount() allows for safe access to waypoint references
      UtScriptRef* wptRef = new UtScriptRef(&wpt, aReturnClassPtr, aObjectPtr->GetWaypointReferenceCount());
      aReturnVal.SetPointer(wptRef);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, Back, 0, "WsfWaypoint", "")
{
   int index = aObjectPtr->GetSize() - 1;
   if (index >= 0)
   {
      WsfWaypoint& wpt = (*aObjectPtr)[index];
      // GetWaypointReferenceCount() allows for safe access to waypoint references
      UtScriptRef* wptRef = new UtScriptRef(&wpt, aReturnClassPtr, aObjectPtr->GetWaypointReferenceCount());
      aReturnVal.SetPointer(wptRef);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, TerrainProfiledClone, 4, "WsfRoute", "double, double, double, double")
{
   // Input Args = double aTerrainClearance, double aMaxGradient, double aMinGradient, double aMaxPostSpacing.

   // Instantiate a utility object of WsfTerrainProfiler.  It needs a reference to WsfTerrain in its constructor.
   WsfTerrainProfiler profiler(SIMULATION->GetTerrainInterface());

   // Copy the supplied WsfRoute (the "this" object, with all its Lat, Lon points) to a WsfTerrainProfiler::InputRoute object.
   WsfTerrainProfiler::InputRoute inputRoute;
   int                            size = (*aObjectPtr).GetSize();
   for (int i = 0; i != size; ++i)
   {
      const WsfWaypoint& wpRef = (*aObjectPtr)[(unsigned int)i];
      inputRoute.emplace_back(wpRef.GetLat(), wpRef.GetLon());
   }

   WsfTerrainProfiler::OutputRoute outputRoute;
   bool                            ok = profiler.CreateProfile(aVarArgs[0].GetDouble(), // aTerrainClearance
                                    aVarArgs[1].GetDouble(), // aMaxGradient
                                    aVarArgs[2].GetDouble(), // aMinGradient
                                    aVarArgs[3].GetDouble(), // aMaxPostSpacing
                                    inputRoute,              // aInputRoute
                                    outputRoute,             // aOutputRoute
                                    true);
   if (ok)
   {
      // Return the new Route to the User:
      WsfRoute* routePtr = new WsfRoute();

      // Now loop through the outputRoute waypoints, and push them into the returned object:
      WsfTerrainProfiler::OutputRoute::const_iterator iter = outputRoute.begin();
      WsfWaypoint                                     wp;
      while (iter != outputRoute.end())
      {
         wp.SetLat(iter->mLatDeg);
         wp.SetLon(iter->mLonDeg);
         wp.SetAlt(iter->mAdjPathAlt);

         routePtr->Append(wp);
         ++iter;
      }
      aReturnVal.SetPointer(routePtr->ScriptRefManage());
   }
   else
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr));
   }
}

// =================================================================================================
// =================================================================================================

class WSF_EXPORT WsfScriptRouteIterator : public UtScriptIterator::Iterator
{
public:
   WsfScriptRouteIterator(const UtScriptRef& aRef)
      : mReference(aRef)
      , mCurrentIndex(0)
   {
   }

   ~WsfScriptRouteIterator() override = default;

   bool HasNext() override
   {
      WsfRoute* routePtr = static_cast<WsfRoute*>(mReference.GetAppObject());
      return (mCurrentIndex < routePtr->GetSize());
   }

   const UtScriptData& Next() override
   {
      UtScriptData rVal;
      WsfRoute*    routePtr = static_cast<WsfRoute*>(mReference.GetAppObject());
      if (mCurrentIndex < routePtr->GetSize())
      {
         WsfWaypoint&          wpt         = routePtr->GetWaypointAt(mCurrentIndex);
         static UtScriptClass* wptClassPtr = mReference.GetScriptClass()->GetTypes()->GetClass("WsfWaypoint");
         // GetWaypointReferenceCount() allows for safe access to waypoint references
         mCurrentData.SetPointer(new UtScriptRef(&wpt, wptClassPtr, routePtr->GetWaypointReferenceCount()));
         ++mCurrentIndex;
         return mCurrentData;
      }
      return mInvalidData;
   }

   UtScriptRef  mReference;
   UtScriptData mCurrentData;
   UtScriptData mInvalidData;
   size_t       mCurrentIndex;
};

// =================================================================================================
//! Script iterator for waypoints in a route.
class WSF_EXPORT WsfScriptRouteIteratorClass : public UtScriptIterator
{
public:
   WsfScriptRouteIteratorClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
      : UtScriptIterator(aClassName, aTypesPtr)
   {
      AddImplicitCastType("Iterator");
   }
   ~WsfScriptRouteIteratorClass() override {}

private:
};

UT_DEFINE_SCRIPT_METHOD(WsfScriptRouteClass, WsfRoute, GetIterator, 0, "WsfRouteIterator", "")
{
   WsfScriptRouteIterator* iterPtr = new WsfScriptRouteIterator(aReference);
   iterPtr->mCurrentIndex          = 0;
   iterPtr->mReference             = aReference;
   aReturnVal.SetPointer(new UtScriptRef(iterPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptRouteClass, WsfRoute)

// =================================================================================================

//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfRoute::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptRouteClass>(aClassName, aScriptTypesPtr);
}

std::unique_ptr<UtScriptClass> WsfRoute::CreateIteratorScriptClass(const std::string& aClassName,
                                                                   UtScriptTypes*     aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptRouteIteratorClass>(aClassName, aScriptTypesPtr);
}
