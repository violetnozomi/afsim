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

#include "WsfScriptZoneClass.hpp"

#include <memory>

#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptColor.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfScenario.hpp"
#include "WsfScriptContext.hpp"
#include "WsfScriptManager.hpp"
#include "WsfSimulation.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneSet.hpp"

// =================================================================================================
WsfScriptZoneClass::WsfScriptZoneClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : WsfScriptObjectClass(aClassName, aTypesPtr)
{
   SetClassName("WsfZone");

   // Add each of the static method objects to the class.
   AddStaticMethod(ut::make_unique<CreateCircular>());   // CreateCircular(double minRadius, double maxRadius, double
                                                         // startAngle, double stopAngle)
   AddStaticMethod(ut::make_unique<CreateElliptical>()); // CreateElliptical(double latAxis, double lonAxis, double
                                                         // startAngle, double stopAngle)
   AddStaticMethod(ut::make_unique<CreateSpherical>());  // CreateSpherical(double minRadius, double maxRadius, double
                                                         // startAngle, double stopAngle)
   AddStaticMethod(ut::make_unique<CreatePolygonalAbsolute>("CreatePolygonal")); // CreatePolygonal(Array<WsfGeoPoint>)
                                                                                 // //absolute points
   AddStaticMethod(ut::make_unique<CreatePolygonalRelative>("CreatePolygonal")); // CreatePolygonal(WsfPlatform,
                                                                                 // Array<Vec3>) //relative points
   AddStaticMethod(ut::make_unique<FindZone>());

   // Add each of the method objects to the class.
   AddMethod(ut::make_unique<DebugDrawZone>("DebugDrawZone")); // DebugDrawZone()
   AddMethod(ut::make_unique<DebugDrawZoneRelative>("DebugDrawZone")); // DebugDrawZone(WsfGeoPoint eyePoint, double lookHeading)
   AddMethod(ut::make_unique<DebugDrawCentroid>());                    // DebugDrawCentroid()
   AddMethod(ut::make_unique<DebugDrawBounds>());                      // DebugDrawBounds()

   AddMethod(ut::make_unique<Draw>("Draw"));         // DebugDrawZone()
   AddMethod(ut::make_unique<DrawRelative>("Draw")); // DebugDrawZone(WsfDraw, WsfGeoPoint eyePoint, double lookHeading)
   AddMethod(ut::make_unique<DrawCentroid>());       // DrawCentroid(WsfDraw)
   AddMethod(ut::make_unique<DrawBounds>());         // DrawBounds(WsfDraw)

   AddMethod(ut::make_unique<MaxPoint>());                       // MaxPoint()
   AddMethod(ut::make_unique<MinPoint>());                       // MinPoint()
   AddMethod(ut::make_unique<ModifierValue>());                  // ModifierValue(string)
   AddMethod(ut::make_unique<PenetrationDistance>());            // PenetrationDistance(WsfGeoPoint, WsfGeoPoint);
   AddMethod(ut::make_unique<Position_1>("Location"));           // Location()
   AddMethod(ut::make_unique<Position_1>("Position"));           // Position()
   AddMethod(ut::make_unique<Position_1>("Centroid"));           // Centroid()
   AddMethod(ut::make_unique<Reference>());                      // Reference()
   AddMethod(ut::make_unique<PointIsInside>("PointIsInside"));   // PointIsInside(WsfGeoPoint,WsfGeoPoint,double,double)
   AddMethod(ut::make_unique<PointIsInside_2>("PointIsInside")); // PointIsInside(WsfPlatform,WsfGeoPoint)
   AddMethod(ut::make_unique<PointIsInside_3>("PointIsInside")); // PointIsInside(WsfGeoPoint)
   AddMethod(ut::make_unique<SetPolyPoints>());                  // SetPolyPoints(Array<WsfGeoPoint> points)
   AddMethod(ut::make_unique<PolyPoints>());

   // Zone color methods
   AddMethod(ut::make_unique<GetFillColor>());
   AddMethod(ut::make_unique<GetLineColor>());
   AddMethod(ut::make_unique<SetFillColor>());
   AddMethod(ut::make_unique<SetLineColor>());


   WsfScriptAuxDataUtil::AddAuxDataScriptMethods(*this);
}

// =================================================================================================
//! virtual
void* WsfScriptZoneClass::Create(const UtScriptContext& aInstance)
{
   return nullptr;
}

// =================================================================================================
//! virtual
void* WsfScriptZoneClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfZone*>(aObjectPtr)->Clone();
}

// =================================================================================================
//! virtual
void WsfScriptZoneClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfZone*>(aObjectPtr);
}

#include "script/WsfScriptDefs.hpp"

// =================================================================================================
//! zone.DebugDrawZone()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DebugDrawZone, 0, "void", "")
{
   WsfDraw draw(*SIMULATION);
   aObjectPtr->DebugDrawZone(draw);
}

// =================================================================================================
//! zone.DebugDrawZone()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DebugDrawZoneRelative, 2, "void", "WsfGeoPoint, double")
{
   WsfGeoPoint* eyePtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
   double       eyeWCS[3];
   eyePtr->GetLocationWCS(eyeWCS);
   double  look = aVarArgs[1].GetDouble();
   WsfDraw draw(*SIMULATION);
   aObjectPtr->DebugDrawZoneRelative(draw, eyeWCS, look);
}

// =================================================================================================
//! zone.DebugDrawCentroid()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DebugDrawCentroid, 0, "void", "")
{
   WsfDraw draw(*SIMULATION);
   aObjectPtr->DebugDrawCentroid(draw);
}

// =================================================================================================
//! zone.DebugDrawBounds()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DebugDrawBounds, 0, "void", "")
{
   WsfDraw draw(*SIMULATION);
   aObjectPtr->DebugDrawBounds(draw);
}

// =================================================================================================
//! zone.Draw()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, Draw, 1, "void", "WsfDraw")
{
   WsfDraw* drawPtr = (WsfDraw*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->Draw(*drawPtr);
}

// =================================================================================================
//! zone.DebugDrawZone()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DrawRelative, 3, "void", "WsfDraw, WsfGeoPoint, double")
{
   WsfDraw*     drawPtr = (WsfDraw*)aVarArgs[0].GetPointer()->GetAppObject();
   WsfGeoPoint* eyePtr  = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
   double       eyeWCS[3];
   eyePtr->GetLocationWCS(eyeWCS);
   double look = aVarArgs[2].GetDouble();
   aObjectPtr->DrawRelative(*drawPtr, eyeWCS, look);
}

// =================================================================================================
//! zone.DebugDrawCentroid()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DrawCentroid, 1, "void", "WsfDraw")
{
   WsfDraw* drawPtr = (WsfDraw*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->DrawCentroid(*drawPtr);
}

// =================================================================================================
//! zone.DebugDrawBounds()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, DrawBounds, 1, "void", "WsfDraw")
{
   WsfDraw* drawPtr = (WsfDraw*)aVarArgs[0].GetPointer()->GetAppObject();
   aObjectPtr->DrawBounds(*drawPtr);
}

// =================================================================================================
//! WsfGeoPoint max = zone.GetMaxPoint()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, MaxPoint, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* maxPtr = nullptr;
   if (aObjectPtr != nullptr)
   {
      double aX, aY, aAlt;
      aObjectPtr->MaxPoint(aX, aY, aAlt);
      maxPtr = new WsfGeoPoint(aX, aY, aAlt);
   }
   aReturnVal.SetPointer(new UtScriptRef(maxPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfGeoPoint min = zone.GetMinPoint()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, MinPoint, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* minPtr = nullptr;
   if (aObjectPtr != nullptr)
   {
      double aX, aY, aAlt;
      aObjectPtr->MinPoint(aX, aY, aAlt);
      minPtr = new WsfGeoPoint(aX, aY, aAlt);
   }
   aReturnVal.SetPointer(new UtScriptRef(minPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! double value = zone.ModifierValue("<modifier_name>");
//!   returns -1 if the modifier was not found.
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, ModifierValue, 1, "double", "string")
{
   std::string modifierName = aVarArgs[0].GetString();
   double      result       = aObjectPtr->ModifierValue(modifierName);
   aReturnVal.SetDouble(result);
}

// =================================================================================================
//! WsfGeoPoint location = zone.Reference()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, Reference, 0, "WsfGeoPoint", "")
{
   std::unique_ptr<WsfGeoPoint> refPoint = nullptr;
   if (aObjectPtr != nullptr)
   {
      WsfZoneDefinition* zoneDefinitionPtr = dynamic_cast<WsfZoneDefinition*>(aObjectPtr);
      if (zoneDefinitionPtr)
      {
         // Ensure the reference point is updated before getting
         zoneDefinitionPtr->UpdateReference(*SIMULATION);
      }

      // Set the reference point if the data is specified otherwise return invalid object
      double locWCS[3] = {0};
      aObjectPtr->Reference().GetLocationWCS(locWCS);
      if ((locWCS[0] != WsfZone::cUNSPECIFIED) && (locWCS[1] != WsfZone::cUNSPECIFIED) &&
          (locWCS[2] != WsfZone::cUNSPECIFIED))
      {
         refPoint = ut::make_unique<WsfGeoPoint>(locWCS);
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(refPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! double penetrationDistance = zone.PenetrationDistance(WsfGeoPoint, WsfGeoPoint);
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, PenetrationDistance, 2, "double", "WsfGeoPoint, WsfGeoPoint")
{
   WsfGeoPoint* point1Ptr = static_cast<WsfGeoPoint*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfGeoPoint* point2Ptr = static_cast<WsfGeoPoint*>(aVarArgs[1].GetPointer()->GetAppObject());
   double       lla1[3];
   double       lla2[3];
   point1Ptr->GetLocationLLA(lla1[0], lla1[1], lla1[2]);
   point2Ptr->GetLocationLLA(lla2[0], lla2[1], lla2[2]);
   aReturnVal.SetDouble(aObjectPtr->CheckIntersections(lla1, lla2));
}

// =================================================================================================
//! WsfGeoPoint location = zone.Location()
//! WsfGeoPoint location = zone.Position()
//! WsfGeoPoint location = zone.Centroid()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, Position_1, 0, "WsfGeoPoint", "")
{
   WsfGeoPoint* centroidPtr = nullptr;
   if (aObjectPtr != nullptr)
   {
      centroidPtr = new WsfGeoPoint(aObjectPtr->Centroid());
   }
   aReturnVal.SetPointer(new UtScriptRef(centroidPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! zone.PointIsInside(WsfGeoPoint,WsfGeoPoint,double,double)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, PointIsInside, 4, "bool", "WsfGeoPoint,WsfGeoPoint,double,double")
{
   bool pointInside = false;
   if (aObjectPtr != nullptr)
   {
      WsfGeoPoint* locPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
      double       locWCS[3];
      locPtr->GetLocationWCS(locWCS);

      WsfGeoPoint* eyePtr = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
      double       eyeWCS[3];
      eyePtr->GetLocationWCS(eyeWCS);

      double look      = aVarArgs[2].GetDouble();
      double rangeDown = aVarArgs[3].GetDouble();
      pointInside      = aObjectPtr->PointIsInside(SIMULATION, locWCS, eyeWCS, look, rangeDown);
   }
   aReturnVal.SetBool(pointInside);
}

// =================================================================================================
//! zone.PointIsInside(WsfPlatform,WsfGeoPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, PointIsInside_2, 2, "bool", "WsfPlatform,WsfGeoPoint")
{
   bool pointInside = false;
   if (aObjectPtr != nullptr)
   {
      double eyeWCS[3];
      double refHeading = 0.0;

      WsfGeoPoint* geoPointPtr = (WsfGeoPoint*)aVarArgs[1].GetPointer()->GetAppObject();
      double       locWCS[3];
      geoPointPtr->GetLocationWCS(locWCS);
      geoPointPtr->GetLocationWCS(eyeWCS); // in case the zone is global absolute

      // verify the zone is a relative zone, attached relative to some platform
      // if not, then ignore the platform passed in, and treat zone as global
      bool               isZoneSet         = (dynamic_cast<WsfZoneSet*>(aObjectPtr) != nullptr);
      WsfZoneDefinition* zoneDefinitionPtr = dynamic_cast<WsfZoneDefinition*>(aObjectPtr);
      if (isZoneSet ||
          ((zoneDefinitionPtr != nullptr) && (zoneDefinitionPtr->GetRelativeType() == WsfZoneDefinition::cOBSERVER)))
      {
         WsfPlatform* platformPtr = (WsfPlatform*)aVarArgs[0].GetPointer()->GetAppObject();
         platformPtr->GetLocationWCS(eyeWCS);
         double refPitch, refRoll;
         platformPtr->GetOrientationNED(refHeading, refPitch, refRoll);
      }

      pointInside = aObjectPtr->PointIsInside(SIMULATION, locWCS, eyeWCS, refHeading);
   }
   aReturnVal.SetBool(pointInside);
}

// =================================================================================================
//! zone.PointIsInside(WsfGeoPoint)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, PointIsInside_3, 1, "bool", "WsfGeoPoint")
{
   bool pointInside = false;
   if (aObjectPtr != nullptr)
   {
      // if the zone is relative, then we can't be in WCS and must return false
      WsfZoneDefinition* zoneDefPtr = dynamic_cast<WsfZoneDefinition*>(aObjectPtr);
      if ((zoneDefPtr != nullptr) && (zoneDefPtr->GetRelativeType() == WsfZoneDefinition::cOBSERVER))
      {
         bool inside = false;
         aReturnVal.SetBool(inside);
         return;
      }

      WsfGeoPoint* geoPointPtr = (WsfGeoPoint*)aVarArgs[0].GetPointer()->GetAppObject();
      double       locWCS[3];
      geoPointPtr->GetLocationWCS(locWCS);

      pointInside = aObjectPtr->PointIsInside(SIMULATION, locWCS, locWCS, 0.0);
   }
   aReturnVal.SetBool(pointInside);
}

// =================================================================================================
//! WsfZone.CreateCircular(double minRadius, double maxRadius, double startAngle, double stopAngle)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, CreateCircular, 4, "WsfZone", "double,double,double,double")
{
   std::unique_ptr<WsfZoneDefinition> zoneDefPtr(new WsfZoneDefinition());
   zoneDefPtr->SetShapeType(WsfZoneDefinition::cCIRCULAR);
   zoneDefPtr->SetRadialBounds(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
   zoneDefPtr->SetAngleBounds(aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble());
   WsfZone* zonePtr = zoneDefPtr.release();
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfZone.CreateElliptical(double latAxis, double lonAxis, double startAngle, double stopAngle)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, CreateElliptical, 4, "WsfZone", "double,double,double,double")
{
   std::unique_ptr<WsfZoneDefinition> zoneDefPtr(new WsfZoneDefinition());
   zoneDefPtr->SetShapeType(WsfZoneDefinition::cELLIPTICAL);
   zoneDefPtr->SetEllipticalAxes(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
   zoneDefPtr->SetAngleBounds(aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble());
   WsfZone* zonePtr = zoneDefPtr.release();
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfZone.CreateSpherical(double minRadius, double maxRadius, double startAngle, double stopAngle)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, CreateSpherical, 4, "WsfZone", "double,double,double,double")
{
   std::unique_ptr<WsfZoneDefinition> zoneDefPtr(new WsfZoneDefinition());
   zoneDefPtr->SetShapeType(WsfZoneDefinition::cSPHERICAL);
   zoneDefPtr->SetRadialBounds(aVarArgs[0].GetDouble(), aVarArgs[1].GetDouble());
   zoneDefPtr->SetAngleBounds(aVarArgs[2].GetDouble(), aVarArgs[3].GetDouble());
   WsfZone* zonePtr = zoneDefPtr.release();
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfZone.CreatePolygonal(Array<WsfGeoPoint>)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, CreatePolygonalAbsolute, 1, "WsfZone", "Array<WsfGeoPoint>")
{
   std::vector<UtScriptData>* scriptPointListPtr =
      static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<WsfGeoPoint>            wsfPointList; // convert points to WSF classes (not script data)
   std::vector<UtScriptData>::iterator it;
   for (it = scriptPointListPtr->begin(); it != scriptPointListPtr->end(); ++it)
   {
      wsfPointList.push_back(*((WsfGeoPoint*)it->GetPointer()->GetAppObject()));
   }

   std::unique_ptr<WsfZoneDefinition> zoneDefPtr(new WsfZoneDefinition());
   zoneDefPtr->SetShapeType(WsfZoneDefinition::cPOLYGONAL);
   zoneDefPtr->SetPolyPoints(wsfPointList);

   WsfZone* zonePtr = zoneDefPtr.release();
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfZone.CreatePolygonal(WsfPlatform, Array<Vec3>)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, CreatePolygonalRelative, 2, "WsfZone", "WsfPlatform, Array<Vec3>")
{
   // WsfPlatform* platformPtr = (WsfPlatform*)(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<UtScriptData>* scriptPointListPtr =
      static_cast<std::vector<UtScriptData>*>(aVarArgs[1].GetPointer()->GetAppObject());
   std::vector<UtVec3d>                wsfPointList; // convert points to WSF classes (not script data)
   std::vector<UtScriptData>::iterator it;
   for (it = scriptPointListPtr->begin(); it != scriptPointListPtr->end(); ++it)
   {
      wsfPointList.push_back(*((UtVec3d*)it->GetPointer()->GetAppObject()));
   }

   std::unique_ptr<WsfZoneDefinition> zoneDefPtr(new WsfZoneDefinition());
   zoneDefPtr->SetShapeType(WsfZoneDefinition::cPOLYGONAL);
   // zoneDefPtr->SetRelativeType(WsfZoneDefinition::cOBSERVER);
   zoneDefPtr->SetPolyPointsRelative(wsfPointList); // sets the relative type

   WsfZone* zonePtr = zoneDefPtr.release();
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
//! WsfZone.FindZone(string)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, FindZone, 1, "WsfZone", "string")
{
   std::string zoneName = aVarArgs[0].GetString();
   WsfZone*    zonePtr  = (WsfZone*)WsfScriptContext::GetSCENARIO(aContext)->FindType("zone", zoneName);
   aReturnVal.SetPointer(new UtScriptRef(zonePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

// =================================================================================================
//! WsfZone.SetPolyPoints(Array<WsfGeoPoint>)
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, SetPolyPoints, 1, "void", "Array<WsfGeoPoint>")
{
   std::vector<UtScriptData>* scriptPointListPtr =
      static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<WsfGeoPoint>            wsfPointList; // convert points to WSF classes (not script data)
   std::vector<UtScriptData>::iterator it;
   for (it = scriptPointListPtr->begin(); it != scriptPointListPtr->end(); ++it)
   {
      wsfPointList.push_back(*((WsfGeoPoint*)it->GetPointer()->GetAppObject()));
   }

   // Make this is a zone definition not a zone set
   WsfZoneDefinition* zoneDefinitionPtr = dynamic_cast<WsfZoneDefinition*>(aObjectPtr);
   if (zoneDefinitionPtr != nullptr)
   {
      zoneDefinitionPtr->SetPolyPoints(wsfPointList);
   }
   else
   {
      ut::log::warning() << "Cannot set points on zone.";
   }
}

// =================================================================================================
//! Array<WsfGeoPoint> WsfZone.PolyPoints()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, PolyPoints, 0, "Array<WsfGeoPoint>", "")
{
   bool               hasReturn(false);
   WsfZoneDefinition* zonePtr = dynamic_cast<WsfZoneDefinition*>(aObjectPtr);
   if (zonePtr != nullptr)
   {
      if (zonePtr->GetShapeType() == WsfZoneDefinition::cPOLYGONAL)
      {
         std::vector<WsfGeoPoint> points;
         zonePtr->GetPoints(points);
         UtScriptDataList* returnListPtr    = new UtScriptDataList(points.size());
         UtScriptClass*    geoPointClassPtr = aContext.GetTypes()->GetClass("WsfGeoPoint");
         for (size_t i = 0; i < points.size(); ++i)
         {
            double lat, lon, alt;
            points[i].GetLocationLLA(lat, lon, alt);
            WsfGeoPoint* geoPointPtr = new WsfGeoPoint(lat, lon, alt);
            (*returnListPtr)[i].SetPointer(new UtScriptRef(geoPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE));
         }
         aReturnVal.SetPointer(new UtScriptRef(returnListPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
         hasReturn = true;
      }
   }
   if (!hasReturn)
   {
      aReturnVal.SetPointer(new UtScriptRef(nullptr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
   }
}

// =================================================================================================
//! WsfZone.GetFillColor()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, GetFillColor, 0, "Color", "")
{
   ut::optional<UtColor> fillColor{aObjectPtr->GetFillColor()};
   if (fillColor.has_value())
   {
      aReturnVal.SetPointer(UtScriptColor::Create(fillColor.value()));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr));
   }
}

// =================================================================================================
//! WsfZone.GetLineColor()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, GetLineColor, 0, "Color", "")
{
   ut::optional<UtColor> lineColor{aObjectPtr->GetLineColor()};
   if (lineColor.has_value())
   {
      aReturnVal.SetPointer(UtScriptColor::Create(lineColor.value()));
   }
   else
   {
      aReturnVal.SetPointer(UtScriptRef::Ref(nullptr, aReturnClassPtr));
   }
}

// =================================================================================================
//! WsfZone.SetFillColor()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, SetFillColor, 1, "void", "Color")
{
   aObjectPtr->SetFillColor(*aVarArgs[0].GetPointer()->GetAppObject<UtColor>());
   WsfObserver::ZoneFillColorChanged(SIMULATION)(*aObjectPtr);
}

// =================================================================================================
//! WsfZone.SetLineColor()
UT_DEFINE_SCRIPT_METHOD(WsfScriptZoneClass, WsfZone, SetLineColor, 1, "void", "Color")
{
   aObjectPtr->SetLineColor(*aVarArgs[0].GetPointer()->GetAppObject<UtColor>());
   WsfObserver::ZoneLineColorChanged(SIMULATION)(*aObjectPtr);
}

UT_DEFINE_AUX_DATA_SCRIPT_METHODS(WsfScriptZoneClass, WsfZone)
