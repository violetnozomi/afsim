// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfPolygonalFieldOfView.hpp"

#include <sstream>

#include "UtInput.hpp"
#include "UtInputString.hpp"
#include "UtMath.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfZoneDefinition.hpp"

// =================================================================================================
WsfPolygonalFieldOfView::WsfPolygonalFieldOfView()
   : WsfFieldOfView()
{
   SetType("polygonal");
   mZoneDefinition.SetShapeType(WsfZoneDefinition::cPOLYGONAL);
}

// =================================================================================================
WsfPolygonalFieldOfView::WsfPolygonalFieldOfView(const WsfPolygonalFieldOfView& aSrc)
   : WsfFieldOfView(aSrc)
   , mAzElPoints(aSrc.mAzElPoints)
   , mZoneDefinition(aSrc.mZoneDefinition)
{
   mZoneDefinition.SetShapeType(WsfZoneDefinition::cPOLYGONAL);
}

// =================================================================================================
bool WsfPolygonalFieldOfView::operator==(const WsfPolygonalFieldOfView& aRhs) const
{
   return (mAzElPoints == aRhs.mAzElPoints);
}

bool WsfPolygonalFieldOfView::operator!=(const WsfPolygonalFieldOfView& aRhs) const
{
   return !(*this == aRhs);
}

// =================================================================================================
// virtual
bool WsfPolygonalFieldOfView::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "azimuth_elevation")
   {
      double az, el;
      aInput.ReadValueOfType(az, UtInput::cANGLE);
      aInput.ReadValueOfType(el, UtInput::cANGLE);
      UtMath::AngleIsBetween(az, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      UtMath::AngleIsBetween(el, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);

      // Save the points for object comparison and access.
      mAzElPoints.emplace_back(std::make_pair(az, el));

      // Insert the point with planar projection of the az, el points at unit distance.
      // Geometrically this amounts to using the tangent of the az and el angles as the zone x,y.
      std::ostringstream oss;
      oss << "point " << tan(az) << " " << tan(el) << " m"; // At unit tangent distance from "eyepoint"
      UtInput input;
      input.PushInput(ut::make_unique<UtInputString>(oss.str()));
      try
      {
         mZoneDefinition.ProcessInput(input);
      }
      catch (const UtInput::EndOfData&)
      {
         // continue
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfPolygonalFieldOfView::Initialize(const WsfArticulatedPart& aArticulatedPart)
{
   WsfFieldOfView::Initialize(aArticulatedPart);
   bool ok = (mZoneDefinition.GetPoints().size() >= 3);

   // The following is a little weird but necessary to maintain a copy of a zone.
   // TODO mZoneDefinition.ComponentParentChanged(mAntennaPtr->GetArticulatedPart()->GetPlatform());
   static_cast<WsfZone&>(mZoneDefinition).Initialize(aArticulatedPart.GetScenario());
   return ok;
}

// =================================================================================================
// virtual
bool WsfPolygonalFieldOfView::WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
{
   // Compute the location of the az, el onto the zone.
   // This amounts to normalizing a vector to unit distance in the x direction
   // (we are looking down the x axis),
   // then using the y value as the x lookup and the -z value as the y lookup.
   double sinAz          = sin(aThisToTgtAz);
   double cosAz          = cos(aThisToTgtAz);
   double sinEl          = sin(aThisToTgtEl);
   double cosEl          = cos(aThisToTgtEl);
   double normalizeConst = 1.0 / (cosAz * cosEl); // Guaranteed not to overflow because we checked values in input.
   double xLookup        = normalizeConst * sinAz * cosEl;
   double yLookup        = normalizeConst * sinEl;
   return mZoneDefinition.WithinPolygonSides(xLookup, yLookup);
}
