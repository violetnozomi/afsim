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

#include "WsfFieldOfView.hpp"

#include <sstream>

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtVec3.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Util.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
// virtual
//! Process input from an input object.
//! @param aInput The input source.
//! @return Whether the current input commands were processed in this method.
bool WsfFieldOfView::ProcessInput(UtInput& aInput)
{
   return false;
}

// =================================================================================================
// virtual
//! Provide initialization of the field of view object.
//! @param aArticulatedPart The articulated part on which the field of view is being defined.
//! @return Whether the initialization was successful.
bool WsfFieldOfView::Initialize(const WsfArticulatedPart& aArticulatedPart)
{
   mArticulatedPartPtr = &aArticulatedPart;
   return true;
}

// =================================================================================================
//! Compute azimuth and elevation of the target and determine if it is within the field of view
//  of the articulated part.
//!
//! This method computes the azimuth and elevation angles for a target wrt to the articulated part.
//! The 'true' values are computed using the actual geometric coordinates.  The 'apparent' values
//! are those computed using the 'Earth Radius Multiplier', which simulates the effects of atmospheric
//! refraction.
//!
//! @param aArticulatedPartPtr      [input] The source articulated part.
//! @param aTgtLocWCS               [input] A target WCS position.
//! @param aEarthRadiusMultiplier   [input] A scale factor to be applied to the Earth's radius to
//! simulate the effect of atmospheric refraction.
//! @param aTrueThisToTgtAz         [output] The true (geometric) azimuth of the target with respect
//! to the antenna coordinate system (radians).
//! @param aTrueThisToTgtEl         [output] The true (geometric) elevation of the target with respect
//! to the antenna coordinate system (radians).
//! @param aApparentThisToTgtLocWCS [output] The WCS vector from the antenna to the apparent
//! position of the target (meters).
//! @param aApparentThisToTgtAz     [output] The azimuth of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentThisToTgtEl     [output] The elevation of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentTgtToThisLocWCS [output] The WCS vector from the target to the apparent
//! position of the antenna (meters).
//!
//! @returns 'true' if within the field of view or 'false' if not.
//!
//! @note Sorry this has so many arguments, but various routine need these values and they are
//! too expensive to recompute.
bool WsfFieldOfView::WithinFieldOfView(WsfArticulatedPart* aArticulatedPartPtr,
                                       double              aTgtLocWCS[3],
                                       double              aEarthRadiusMultiplier,
                                       double&             aTrueThisToTgtAz,
                                       double&             aTrueThisToTgtEl,
                                       double              aApparentThisToTgtLocWCS[3],
                                       double&             aApparentThisToTgtAz,
                                       double&             aApparentThisToTgtEl,
                                       double              aApparentTgtToThisLocWCS[3])
{
   bool withinFieldOfView = false;
   bool useGeometricView  = true;

   double thisLocWCS[3];
   double thisToTgtLocWCS[3];

   // Get the WCS position vectors of this and the target and the relative position vector from this to the target.
   aArticulatedPartPtr->GetLocationWCS(thisLocWCS);
   UtVec3d::Subtract(thisToTgtLocWCS, aTgtLocWCS, thisLocWCS);

   // Compute the true (geometric) azimuth and elevation of the target with respect to this.
   aArticulatedPartPtr->ComputeAspect(thisToTgtLocWCS, aTrueThisToTgtAz, aTrueThisToTgtEl);

   if (aEarthRadiusMultiplier != 1.0)
   {
      // Get the altitude at the two points.
      double lat;
      double lon;
      double thisAlt;
      double tgtAlt;
      aArticulatedPartPtr->GetLocationLLA(lat, lon, thisAlt);
      UtEntity::ConvertWCSToLLA(aTgtLocWCS, lat, lon, tgtAlt);

      // Account for atmospheric refraction by determining the unit vectors that point to the 'apparent' locations.
      double apparentThisLocWCS[3];
      double apparentTgtLocWCS[3];
      bool   useApparentView = WsfEM_Util::ComputeApparentPosition(aEarthRadiusMultiplier,
                                                                 thisLocWCS,
                                                                 aTgtLocWCS,
                                                                 thisAlt,
                                                                 tgtAlt,
                                                                 apparentThisLocWCS,
                                                                 apparentTgtLocWCS);
      if (useApparentView)
      {
         useGeometricView = false;
         UtVec3d::Subtract(aApparentThisToTgtLocWCS, apparentTgtLocWCS, thisLocWCS);
         aArticulatedPartPtr->ComputeAspect(aApparentThisToTgtLocWCS, aApparentThisToTgtAz, aApparentThisToTgtEl);
         if (WithinFieldOfView(aApparentThisToTgtAz, aApparentThisToTgtEl))
         {
            // Compute the WCS unit vector that points from the target to this apparent position (needed for signature lookups).
            withinFieldOfView = true;
            UtVec3d::Subtract(aApparentTgtToThisLocWCS, apparentThisLocWCS, aTgtLocWCS);
            UtVec3d::Normalize(aApparentTgtToThisLocWCS);
         }
      }
   }

   // If the view wasn't computed and checked using the refracted value
   // then check the view using the geometric values.

   if (useGeometricView)
   {
      if (WithinFieldOfView(aTrueThisToTgtAz, aTrueThisToTgtEl))
      {
         withinFieldOfView = true;

         // The apparent view is the same as the geometric view.
         UtVec3d::Set(aApparentThisToTgtLocWCS, thisToTgtLocWCS);
         aApparentThisToTgtAz = aTrueThisToTgtAz;
         aApparentThisToTgtEl = aTrueThisToTgtEl;

         // Compute the WCS unit vector that points from the target to the apparent position
         // of this (needed for signature lookups).
         UtVec3d::Multiply(aApparentTgtToThisLocWCS, thisToTgtLocWCS, -1.0);
         UtVec3d::Normalize(aApparentTgtToThisLocWCS);
      }
   }

   return withinFieldOfView;
}

// =================================================================================================
//! Compute azimuth and elevation of the target and determine if it is within the field of view
//  of the antenna.
//!
//! This method computes the azimuth and elevation angles for a target wrt to the antenna.
//! The 'true' values are computed using the actual geometric coordinates.  The 'apparent' values
//! are those computed using the 'Earth Radius Multiplier', which simulates the effects of atmospheric
//! refraction.
//!
//! @param aAntennaPtr              [input] A source antenna.
//! @param aTgtLocWCS               [input] A target WCS position.
//! @param aEarthRadiusMultiplier   [input] A scale factor to be applied to the Earth's radius to
//! simulate the effect of atmospheric refraction.
//! @param aTrueThisToTgtAz         [output] The true (geometric) azimuth of the target with respect
//! to the antenna coordinate system (radians).
//! @param aTrueThisToTgtEl         [output] The true (geometric) elevation of the target with respect
//! to the antenna coordinate system (radians).
//! @param aApparentThisToTgtLocWCS [output] The WCS vector from the antenna to the apparent
//! position of the target (meters).
//! @param aApparentThisToTgtAz     [output] The azimuth of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentThisToTgtEl     [output] The elevation of the apparent target with respect to
//! the antenna coordinate system (radians).
//! @param aApparentTgtToThisLocWCS [output] The WCS vector from the target to the apparent
//! position of the antenna (meters).
//!
//! @returns 'true' if within the field of view or 'false' if not.
//!
//! @note Sorry this has so many arguments, but various routine need these values and they are
//! too expensive to recompute.
bool WsfFieldOfView::WithinFieldOfView(WsfEM_Antenna* aAntennaPtr,
                                       double         aTgtLocWCS[3],
                                       double         aEarthRadiusMultiplier,
                                       double&        aTrueThisToTgtAz,
                                       double&        aTrueThisToTgtEl,
                                       double         aApparentThisToTgtLocWCS[3],
                                       double&        aApparentThisToTgtAz,
                                       double&        aApparentThisToTgtEl,
                                       double         aApparentTgtToThisLocWCS[3])
{
   bool withinFieldOfView = false;
   bool useGeometricView  = true;

   double thisLocWCS[3];
   double thisToTgtLocWCS[3];

   // Get the WCS position vectors of this and the target and the relative position vector from this to the target.
   aAntennaPtr->GetLocationWCS(thisLocWCS);
   UtVec3d::Subtract(thisToTgtLocWCS, aTgtLocWCS, thisLocWCS);

   // Compute the true (geometric) azimuth and elevation of the target with respect to this.
   aAntennaPtr->ComputeAspect(thisToTgtLocWCS, aTrueThisToTgtAz, aTrueThisToTgtEl);

   if (aEarthRadiusMultiplier != 1.0)
   {
      // Get the altitude at the two points.
      double lat;
      double lon;
      double thisAlt;
      double tgtAlt;
      aAntennaPtr->GetLocationLLA(lat, lon, thisAlt);
      UtEntity::ConvertWCSToLLA(aTgtLocWCS, lat, lon, tgtAlt);

      // Account for atmospheric refraction by determining the unit vectors that point to the 'apparent' locations.
      double apparentThisLocWCS[3];
      double apparentTgtLocWCS[3];
      bool   useApparentView = WsfEM_Util::ComputeApparentPosition(aEarthRadiusMultiplier,
                                                                 thisLocWCS,
                                                                 aTgtLocWCS,
                                                                 thisAlt,
                                                                 tgtAlt,
                                                                 apparentThisLocWCS,
                                                                 apparentTgtLocWCS);
      if (useApparentView)
      {
         useGeometricView = false;
         UtVec3d::Subtract(aApparentThisToTgtLocWCS, apparentTgtLocWCS, thisLocWCS);
         aAntennaPtr->ComputeAspect(aApparentThisToTgtLocWCS, aApparentThisToTgtAz, aApparentThisToTgtEl);
         if (WithinFieldOfView(aApparentThisToTgtAz, aApparentThisToTgtEl))
         {
            // Compute the WCS unit vector that points from the target to this apparent position (needed for signature lookups).
            withinFieldOfView = true;
            UtVec3d::Subtract(aApparentTgtToThisLocWCS, apparentThisLocWCS, aTgtLocWCS);
            UtVec3d::Normalize(aApparentTgtToThisLocWCS);
         }
      }
   }

   // If the view wasn't computed and checked using the refracted value
   // then check the view using the geometric values.

   if (useGeometricView)
   {
      if (WithinFieldOfView(aTrueThisToTgtAz, aTrueThisToTgtEl))
      {
         withinFieldOfView = true;

         // The apparent view is the same as the geometric view.
         UtVec3d::Set(aApparentThisToTgtLocWCS, thisToTgtLocWCS);
         aApparentThisToTgtAz = aTrueThisToTgtAz;
         aApparentThisToTgtEl = aTrueThisToTgtEl;

         // Compute the WCS unit vector that points from the target to the apparent position
         // of this (needed for signature lookups).
         UtVec3d::Multiply(aApparentTgtToThisLocWCS, thisToTgtLocWCS, -1.0);
         UtVec3d::Normalize(aApparentTgtToThisLocWCS);
      }
   }

   return withinFieldOfView;
}
