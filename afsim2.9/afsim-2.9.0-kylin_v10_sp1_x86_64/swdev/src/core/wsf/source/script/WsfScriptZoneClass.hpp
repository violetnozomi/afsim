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

#ifndef WSFSCRIPTZONECLASS_HPP
#define WSFSCRIPTZONECLASS_HPP

#include "wsf_export.h"

#include "WsfScriptAuxDataUtil.hpp"
#include "WsfScriptObjectClass.hpp"

//! Define script methods for WsfZone.
class WSF_EXPORT WsfScriptZoneClass : public WsfScriptObjectClass
{
public:
   WsfScriptZoneClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Simulation management methods
   UT_DECLARE_SCRIPT_METHOD(Draw);         // draw the zone using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DrawRelative); // draw the zone relative to a given location, using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DrawCentroid); // draw the centroid using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DrawBounds);   // draw the bounding box using WsfDraw

   UT_DECLARE_SCRIPT_METHOD(DebugDrawZone);         // draw the zone using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DebugDrawZoneRelative); // draw the zone relative to a given location, using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DebugDrawCentroid);     // draw the centroid using WsfDraw
   UT_DECLARE_SCRIPT_METHOD(DebugDrawBounds);       // draw the bounding box using WsfDraw

   UT_DECLARE_SCRIPT_METHOD(MaxPoint);            // Get the max point of the bounding box
   UT_DECLARE_SCRIPT_METHOD(MinPoint);            // Get the min point of the bounding box
   UT_DECLARE_SCRIPT_METHOD(ModifierValue);       // Get the value of a modifier
   UT_DECLARE_SCRIPT_METHOD(Position_1);          // Get the position of the zone (centroid)
   UT_DECLARE_SCRIPT_METHOD(Reference);           // Get the reference point of the zone (usually the first vertex)
   UT_DECLARE_SCRIPT_METHOD(PenetrationDistance); // Get the penetration distance for a line segment
   UT_DECLARE_SCRIPT_METHOD(PointIsInside);       // Test if the given point is inside the zone
   UT_DECLARE_SCRIPT_METHOD(PointIsInside_2);     // Test if the given point is inside the zone (relative to platform)
   UT_DECLARE_SCRIPT_METHOD(PointIsInside_3); // Test if the given point is inside the zone (global zone made of geo points)

   UT_DECLARE_AUX_DATA_SCRIPT_METHODS()

   UT_DECLARE_SCRIPT_METHOD(CreateCircular);          // Create an instance of a circular zone
   UT_DECLARE_SCRIPT_METHOD(CreateElliptical);        // Create an instance of an elliptical zone
   UT_DECLARE_SCRIPT_METHOD(CreateSpherical);         // Create an instance of a spherical zone
   UT_DECLARE_SCRIPT_METHOD(CreatePolygonalAbsolute); // Create an instance of a polygonal zone, with absolute coordinates
   UT_DECLARE_SCRIPT_METHOD(CreatePolygonalRelative); // Create an instance of a polygonal zone, with relative coordinates
   UT_DECLARE_SCRIPT_METHOD(FindZone);
   UT_DECLARE_SCRIPT_METHOD(SetPolyPoints); // Set the points of the zone, makes it a polygonal zone
   UT_DECLARE_SCRIPT_METHOD(PolyPoints);

   // Zone color methods
   UT_DECLARE_SCRIPT_METHOD(GetFillColor);
   UT_DECLARE_SCRIPT_METHOD(GetLineColor);
   UT_DECLARE_SCRIPT_METHOD(SetFillColor);
   UT_DECLARE_SCRIPT_METHOD(SetLineColor);
};

#endif
