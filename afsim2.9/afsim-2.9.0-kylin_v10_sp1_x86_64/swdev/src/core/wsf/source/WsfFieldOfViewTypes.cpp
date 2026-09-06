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

#include "WsfFieldOfViewTypes.hpp"

#include "UtException.hpp"
#include "UtMemory.hpp"
#include "WsfCircularFieldOfView.hpp"
#include "WsfEquatorialFieldOfView.hpp"
#include "WsfFieldOfView.hpp"
#include "WsfPolygonalFieldOfView.hpp"
#include "WsfRectangularFieldOfView.hpp"

// =================================================================================================
//! Factory method to create a new instance of a field-of-view type.
//! @param aType A string indicating the type of field of view.
//!              Currently this must be one of "rectangular", "circular", "polygonal", or "equatorial".
//! @return The new field-of-view object.
std::unique_ptr<WsfFieldOfView> WsfFieldOfViewTypes::Create(WsfStringId aType)
{
   if (aType == "rectangular")
   {
      return ut::make_unique<WsfRectangularFieldOfView>();
   }
   if (aType == "circular")
   {
      return ut::make_unique<WsfCircularFieldOfView>();
   }
   if (aType == "polygonal")
   {
      return ut::make_unique<WsfPolygonalFieldOfView>();
   }
   if (aType == "equatorial")
   {
      return ut::make_unique<WsfEquatorialFieldOfView>();
   }
   throw UtException("Field of view type " + aType + " does not exist.");
}
