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

// ================================================================================================
// ================================================================================================
// PROGRAMMING NOTE: See the programming notes in WsfZone.cpp about the use of Initialize()!!!!
// ================================================================================================
// ================================================================================================

#include "WsfCodedZone.hpp"

#include "UtInput.hpp"
#include "UtSpan.hpp"

// =================================================================================================
//! Old form of the constructor
//! @param aLL_Points Input vector of latitude/longitude points.
WsfCodedZone::WsfCodedZone(ut::span<const Point> aLL_Points)
   : WsfZoneDefinition()
{
   SetShapeType(cPOLYGONAL);
   SetPointsAreLatLon(true);
   for (const auto& point : aLL_Points)
   {
      AddPoint(point);
   }
}

// =================================================================================================
// virtual
WsfZone* WsfCodedZone::Clone() const
{
   return new WsfCodedZone(*this);
}

// =================================================================================================
bool WsfCodedZone::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "latitude_longitude")
   {
      SetShapeType(cPOLYGONAL);
      SetPointsAreLatLon(true);

      double lat, lon;
      aInput.ReadValueOfType(lat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(lon, UtInput::cLONGITUDE);
      AddPoint(Point(lat, lon));
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
