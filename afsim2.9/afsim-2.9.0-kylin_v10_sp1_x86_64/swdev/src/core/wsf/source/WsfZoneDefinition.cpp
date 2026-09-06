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

#include "WsfZoneDefinition.hpp"

#include <cassert>

#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfDraw.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfZone.hpp"

namespace
{
constexpr double cNEAR_ZERO = std::numeric_limits<double>::epsilon();

std::string ShapeToString(WsfZoneDefinition::ShapeType aShape)
{
   switch (aShape)
   {
   case WsfZoneDefinition::cNO_SHAPE:
      return "None";
   case WsfZoneDefinition::cCIRCULAR:
      return "Circle";
   case WsfZoneDefinition::cELLIPTICAL:
      return "Ellipse";
   case WsfZoneDefinition::cSPHERICAL:
      return "Sphere";
   case WsfZoneDefinition::cPOLYGONAL:
      return "Polygon";
   default:
      return "Unknown";
   }
}

} // namespace

// =================================================================================================
WsfZoneDefinition::WsfZoneDefinition()
   : WsfZone()
   , mPoints()
   , mShapeType(cNO_SHAPE)
   , mMinRadiusSq(cUNSPECIFIED)
   , mMaxRadiusSq(cUNSPECIFIED)
   , mLongitudinalAxis(cUNSPECIFIED)
   , mLateralAxis(cUNSPECIFIED)
   , mStartAngle(cUNSPECIFIED)
   , mStopAngle(cUNSPECIFIED)
   , mPointsAreLatLon(false)
   , mPointsInputIsMGRS(false)
   , mPointsInputIsPolar(false)
   , mNegative(false)
   , mRelativeTo(cOBSERVER)
   , mReferenceLat(cUNSPECIFIED)
   , mReferenceLon(cUNSPECIFIED)
   , mReferenceHeading(0.0)
   , mHasAbsoluteLatLon(false)
   , mReferencePlatformNameId(nullptr)
   , mReferencePlatformIndex(0)
   , mRefZonePtr(nullptr)
   , mZoneIndex(0)
   , mDebugEnabled(false)
{
   mReferenceWCS[0] = cUNSPECIFIED;
   mReferenceWCS[1] = cUNSPECIFIED;
   mReferenceWCS[2] = cUNSPECIFIED;
}

// =================================================================================================
WsfZoneDefinition::WsfZoneDefinition(const WsfZoneDefinition& aSrc)
   : WsfZone(aSrc)
   , mPoints(aSrc.mPoints)
   , mShapeType(aSrc.mShapeType)
   , mMinRadiusSq(aSrc.mMinRadiusSq)
   , mMaxRadiusSq(aSrc.mMaxRadiusSq)
   , mLongitudinalAxis(aSrc.mLongitudinalAxis)
   , mLateralAxis(aSrc.mLateralAxis)
   , mStartAngle(aSrc.mStartAngle)
   , mStopAngle(aSrc.mStopAngle)
   , mPointsAreLatLon(aSrc.mPointsAreLatLon)
   , mPointsInputIsMGRS(aSrc.mPointsInputIsMGRS)
   , mPointsInputIsPolar(aSrc.mPointsInputIsPolar)
   , mNegative(aSrc.mNegative)
   , mRelativeTo(aSrc.mRelativeTo)
   , mReferenceLat(aSrc.mReferenceLat)
   , mReferenceLon(aSrc.mReferenceLon)
   , mReferenceHeading(aSrc.mReferenceHeading)
   , mHasAbsoluteLatLon(aSrc.mHasAbsoluteLatLon)
   , mReferencePlatformNameId(aSrc.mReferencePlatformNameId)
   , mReferencePlatformIndex(0)
   , mRefZonePtr(aSrc.mRefZonePtr)
   , mZoneIndex(aSrc.mZoneIndex)
   , mDebugEnabled(aSrc.mDebugEnabled)
{
   mReferenceWCS[0] = aSrc.mReferenceWCS[0];
   mReferenceWCS[1] = aSrc.mReferenceWCS[1];
   mReferenceWCS[2] = aSrc.mReferenceWCS[2];
}

// =================================================================================================
// virtual
WsfZone* WsfZoneDefinition::Clone() const
{
   return new WsfZoneDefinition(*this);
}

// =================================================================================================
// virtual
bool WsfZoneDefinition::PointIsInside(WsfSimulation* aSimulationPtr,
                                      const double   aViewedPointWCS[3],
                                      const double   aEyePointWCS[3],
                                      const double   aLookHeading,
                                      const double   aDeltaDownRange)
{
   // If the zone is relative to another platform, define the reference point to be that platform.
   if (mReferencePlatformNameId != 0)
   {
      // Attempt to use the platform index from the last call (if defined).
      // As long as the platform still exists then this is the fastest way to get to the platform.
      WsfPlatform* platformPtr = nullptr;
      if (mReferencePlatformIndex != 0 && aSimulationPtr)
      {
         platformPtr = aSimulationPtr->GetPlatformByIndex(mReferencePlatformIndex);
      }

      if (platformPtr == nullptr && aSimulationPtr)
      {
         // One of the following is true:
         // 1) This is the first call OR the platform has not come into existence.
         // 2) The platform had at one time existed, but has been deleted.
         //
         // So go back and try to find the platform by name (which is slower than finding by index).

         platformPtr = aSimulationPtr->GetPlatformByName(mReferencePlatformNameId);
         if ((platformPtr == nullptr) && (mReferencePlatformIndex == 0))
         {
            // Platform does not exist, and has never existed (so no previous data exists).
            return false;
         }
      }

      // Update the reference data if the platform exists, otherwise just use the previous data.
      if (platformPtr != nullptr)
      {
         mReferencePlatformIndex = platformPtr->GetIndex();
         SetReference(*platformPtr);
      }
   }

   bool inside = PointIsInsidePrivate(aViewedPointWCS, aEyePointWCS, aLookHeading, aDeltaDownRange);
   return (mNegative ? !inside : inside);
}

// =================================================================================================
// override
double WsfZoneDefinition::CheckIntersections(const double aLLA1[3], const double aLLA2[3])
{
   double penetrationDistance = 0.0;

   if (mPointsAreLatLon)
   {
      if (mShapeType != cSPHERICAL)
      {
         penetrationDistance = ExtrudedZonePenetrationDistance(aLLA1, aLLA2);
      }
      else
      {
         penetrationDistance = SphericalZonePenetrationDistance(aLLA1, aLLA2);
      }
      if (mNegative)
      {
         UtVec3d line;
         line.Subtract(aLLA2, aLLA1);
         double end1_WCS[3];
         double end2_WCS[3];
         UtEllipsoidalEarth::ConvertLLAToECEF(aLLA1[0], aLLA1[1], aLLA1[2], end1_WCS);
         UtEllipsoidalEarth::ConvertLLAToECEF(aLLA2[0], aLLA2[1], aLLA2[2], end2_WCS);
         UtVec3d penetrationVector;
         penetrationVector.Subtract(end2_WCS, end1_WCS);
         penetrationDistance = penetrationVector.Magnitude() - penetrationDistance;
      }
   }
   return penetrationDistance;
}

// =================================================================================================
// virtual
bool WsfZoneDefinition::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "circular")
   {
      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cCIRCULAR;
         if (mPointsInputIsPolar || mPointsAreLatLon)
         {
            auto out = ut::log::warning() << "Polar or lat/lon points are not intended to be used with circular zones, "
                                             "and may modify the zone in unexpected ways.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Previous inputs conflict with selection of Zone type 'circular'.");
      }
   }
   else if (command == "elliptical")
   {
      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cELLIPTICAL;
         if (mPointsInputIsPolar || mPointsAreLatLon)
         {
            auto out = ut::log::warning() << "Polar or lat/lon points are not intended to be used with elliptical "
                                             "zones, and may modify the zone in unexpected ways.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Previous inputs conflict with selection of Zone type 'elliptical'.");
      }
   }
   else if (command == "polygonal")
   {
      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cPOLYGONAL;
         if ((mMinRadiusSq != cUNSPECIFIED) || (mMaxRadiusSq != cUNSPECIFIED) || (mStartAngle != cUNSPECIFIED) ||
             (mStopAngle != cUNSPECIFIED))
         {
            auto out = ut::log::warning() << "Radius or angle constraints are not intended to be used with polygonal "
                                             "zones, and may modify the zone in unexpected ways.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Previous inputs conflict with selection of Zone type 'polygonal'.");
      }
   }
   else if (command == "spherical")
   {
      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cSPHERICAL;
         if (mPointsInputIsPolar || mPointsAreLatLon)
         {
            auto out = ut::log::warning() << "Polar or lat/lon points are not intended to be used with spherical "
                                             "zones, and may modify the zone in unexpected ways.";
            out.AddNote() << "Location: " << aInput.GetLocation();
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Previous inputs conflict with selection of Zone type 'spherical'.");
      }
   }
   else if (command == "minimum_altitude")
   {
      aInput.ReadValueOfType(mMinAlt, UtInput::cLENGTH);

      if ((mMaxAlt != cUNSPECIFIED) && (mMinAlt >= mMaxAlt))
      {
         throw UtInput::BadValue(aInput, "Minimum altitude must be specified as less than maximum altitude.");
      }
   }
   else if (command == "maximum_altitude")
   {
      aInput.ReadValueOfType(mMaxAlt, UtInput::cLENGTH);

      if ((mMinAlt != cUNSPECIFIED) && (mMaxAlt <= mMinAlt))
      {
         throw UtInput::BadValue(aInput, "Maximum altitude must be specified greater than minimum altitude.");
      }
   }
   else if (command == "minimum_radius")
   {
      aInput.ReadValueOfType(mMinRadiusSq, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMinRadiusSq, 0.0);
      mMinRadiusSq *= mMinRadiusSq;

      if (mShapeType == cPOLYGONAL)
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is not intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }

      if ((mMaxRadiusSq != cUNSPECIFIED) && (mMaxRadiusSq <= mMinRadiusSq))
      {
         throw UtInput::BadValue(aInput, "Minimum radius must be specified smaller than maximum radius.");
      }
   }
   else if (command == "maximum_radius")
   {
      aInput.ReadValueOfType(mMaxRadiusSq, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mMaxRadiusSq, 0.0);
      mMaxRadiusSq *= mMaxRadiusSq;

      if (mShapeType == cPOLYGONAL)
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is not intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }

      if ((mMinRadiusSq != cUNSPECIFIED) && (mMinRadiusSq >= mMaxRadiusSq))
      {
         throw UtInput::BadValue(aInput, "Maximum radius must be specified greater than minimum radius.");
      }
   }
   else if (command == "longitudinal_axis")
   {
      aInput.ReadValueOfType(mLongitudinalAxis, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mLongitudinalAxis, 0.0);

      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cELLIPTICAL;
      }
      else if (mShapeType != cELLIPTICAL)
      {
         throw UtInput::BadValue(aInput, "Non-Elliptical Zone is defining a Longitudinal axis.  Not permitted.");
      }
   }
   else if (command == "lateral_axis")
   {
      aInput.ReadValueOfType(mLateralAxis, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mLateralAxis, 0.0);

      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cELLIPTICAL;
      }
      else if (mShapeType != cELLIPTICAL)
      {
         throw UtInput::BadValue(aInput, "Non-Elliptical Zone is defining a Lateral axis.  Not permitted.");
      }
   }
   else if (command == "start_angle")
   {
      aInput.ReadValueOfType(mStartAngle, UtInput::cANGLE);
      mStartAngle = UtMath::NormalizeAngle0_TwoPi(mStartAngle);
      if (mShapeType == cPOLYGONAL)
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is not intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (command == "stop_angle")
   {
      aInput.ReadValueOfType(mStopAngle, UtInput::cANGLE);
      mStopAngle = UtMath::NormalizeAngle0_TwoPi(mStopAngle);
      if (mShapeType == cPOLYGONAL)
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is not intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
      if (mStopAngle == 0)
      {
         mStopAngle = UtMath::cTWO_PI;
      }
   }
   else if (command == "lat_lon")
   {
      mPointsAreLatLon = true;
      mRelativeTo      = cINTERNAL;
      if ((mShapeType != cNO_SHAPE) && (mShapeType != cPOLYGONAL))
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is only intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (command == "mgrs")
   {
      mPointsAreLatLon   = true;
      mPointsInputIsMGRS = true;
      mRelativeTo        = cINTERNAL;
      if ((mShapeType != cNO_SHAPE) && (mShapeType != cPOLYGONAL))
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is only intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (command == "polar")
   {
      mPointsInputIsPolar = true;
      if ((mShapeType != cNO_SHAPE) && (mShapeType != cPOLYGONAL))
      {
         auto out = ut::log::warning()
                    << "Command " << command
                    << " is only intended to be used with polygonal zones, and may modify the zone in unexpected ways.";
         out.AddNote() << "Location: " << aInput.GetLocation();
      }
   }
   else if (command == "point")
   {
      if (mShapeType == cNO_SHAPE)
      {
         mShapeType = cPOLYGONAL;
      }
      else if (mShapeType != cPOLYGONAL)
      {
         throw UtInput::BadValue(aInput, "Cannot specify a 'point' in a non-'polygonal' zone");
      }

      double xOrLat;
      double yOrLon;
      if (mPointsAreLatLon)
      {
         if (mPointsInputIsMGRS)
         {
            std::string mgrsCoord;
            aInput.ReadValue(mgrsCoord);
            bool success = UtEllipsoidalEarth::ConvertMGRSToLL(mgrsCoord, xOrLat, yOrLon);
            if (!success)
            {
               throw UtInput::BadValue(aInput, "Unable to convert MGRS location: " + mgrsCoord);
            }
         }
         else // standard lat/lon input
         {
            aInput.ReadValueOfType(xOrLat, UtInput::cLATITUDE);
            aInput.ReadValueOfType(yOrLon, UtInput::cLONGITUDE);
         }

         // If a stationary Zone is specified, the reference (lat,lon) location
         // is, by default, the first (lat,lon) point specified.  Can be over-
         // ridden by config file input, if desired.  By default, the reference
         // heading is Zero (north), but can also be overridden via config file.
         if (mReferenceLat == cUNSPECIFIED)
         {
            mReferenceLat = xOrLat;
            mReferenceLon = yOrLon;
            UtEntity::ConvertLLAToWCS(mReferenceLat, mReferenceLon, 0.0, mReferenceWCS);
         }
         mHasAbsoluteLatLon = true;
      }
      else if (mPointsInputIsPolar)
      {
         double bearing;
         double range;
         aInput.ReadValueOfType(bearing, UtInput::cANGLE);
         aInput.ReadValueOfType(range, UtInput::cLENGTH);

         xOrLat = range * cos(bearing);
         yOrLon = range * sin(bearing);
      }
      else
      {
         std::string units;
         aInput.ReadValue(xOrLat);
         aInput.ReadValue(yOrLon);
         aInput.ReadValue(units);
         double multiplier = aInput.ConvertValue(1.0, units, UtInput::cLENGTH);
         xOrLat *= multiplier;
         yOrLon *= multiplier;
      }
      mPoints.emplace_back(xOrLat, yOrLon);
   }
   else if (command == "position")
   {
      aInput.ReadValueOfType(mReferenceLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mReferenceLon, UtInput::cLONGITUDE);

      UtEntity::ConvertLLAToWCS(mReferenceLat, mReferenceLon, 0.0, mReferenceWCS);

      // Since the user has just specified a reference (lat, lon),
      // the zone is obviously intending to use it...
      mRelativeTo              = cINTERNAL;
      mHasAbsoluteLatLon       = true;
      mReferencePlatformNameId = nullptr;
   }
   else if (command == "heading")
   {
      aInput.ReadValueOfType(mReferenceHeading, UtInput::cANGLE);

      // Since the user has just specified a reference heading,
      // the zone is obviously intending to use it...
      mRelativeTo              = cINTERNAL;
      mReferencePlatformNameId = nullptr;
   }
   else if (command == "references")
   {
      std::string refName;
      aInput.ReadCommand(refName);
      mRefZonePtr = dynamic_cast<WsfZoneDefinition*>(WsfScenario::FromInput(aInput).FindType("zone", refName));
      if (mRefZonePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "No zone definition named " + refName + " found to reference.");
      }
      mReferencePlatformNameId = nullptr;
   }
   else if (command == "reference_platform")
   {
      std::string referencePlatformName;
      aInput.ReadValue(referencePlatformName);
      mReferencePlatformNameId = referencePlatformName;
      mHasAbsoluteLatLon       = false;
      mRelativeTo              = cINTERNAL;
   }
   else if ((command == "sensor_modifier") || (command == "comm_modifier") || (command == "modifier"))
   {
      // xxx_modifier <category-name> <modifier-value>
      std::string categoryName;
      aInput.ReadCommand(categoryName);
      double modifierValue;
      aInput.ReadValue(modifierValue);
      mModifierList[categoryName] = modifierValue;
   }
   else if (command == "negative")
   {
      mNegative = true;
   }
   else if (command == "positive")
   {
      mNegative = false;
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = WsfZone::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
//! Update the relevant reference platform for the zone from the simulation.
//!
//! @param aSimulation A reference to the simulation object.
//! @returns 'void'.
void WsfZoneDefinition::UpdateReference(const WsfSimulation& aSimulation)
{
   // If the zone is relative to another platform, define the reference point to be that platform.
   if (!mReferencePlatformNameId.Empty())
   {
      // Clear the reference point
      mReferenceWCS[0] = cUNSPECIFIED;
      mReferenceWCS[1] = cUNSPECIFIED;
      mReferenceWCS[2] = cUNSPECIFIED;

      // Attempt to use the platform index from the last call (if defined).
      // As long as the platform still exists then this is the fastest way to get to the platform.
      WsfPlatform* platformPtr = nullptr;
      if (mReferencePlatformIndex != 0)
      {
         platformPtr = aSimulation.GetPlatformByIndex(mReferencePlatformIndex);
      }

      // if platformPtr is still nullptr, then we know GetPlatformByIndex failed
      if (platformPtr == nullptr)
      {
         // One of the following is true:
         // 1) This is the first call OR the platform has not come into existence.
         // 2) The platform had at one time existed, but has been deleted.
         //
         // So go back and try to find the platform by name (which is slower than finding by index).

         platformPtr = aSimulation.GetPlatformByName(mReferencePlatformNameId);
      }

      // Update the reference data if the platform exists, otherwise just use the previous data.
      if (platformPtr != nullptr)
      {
         mReferencePlatformIndex = platformPtr->GetIndex();
         SetReference(*platformPtr);
      }
   }
}

// =================================================================================================
//! Set the geometric reference point for the zone.
//! (There are 4 different forms of the same method.)
//!
//! @param aReference the UtEntity at a location where the zone will be placed.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetReference(UtEntity& aReference)
{
   if (mRelativeTo == cINTERNAL)
   {
      double lat;
      double lon;
      double alt;
      aReference.GetLocationLLA(lat, lon, alt);

      double heading;
      double pitch;
      double roll;
      aReference.GetOrientationNED(heading, pitch, roll);

      double newWCS[3];
      UtEntity::ConvertLLAToWCS(lat, lon, alt, newWCS);
      SetReference(lat, lon, heading, newWCS);
   }
}

// =================================================================================================
//! Set the geometric reference point for the zone.
//! (There are 4 different forms of the same method.)
//!
//! @param aReference the geopoint where the zone will be placed.
//! @param aReferenceHeading the new heading at which the zone will be oriented.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetReference(const WsfGeoPoint& aReference, double aReferenceHeading)
{
   if (mRelativeTo == cINTERNAL)
   {
      double lat = aReference.GetLat();
      double lon = aReference.GetLon();

      double newWCS[3];
      UtEntity::ConvertLLAToWCS(lat, lon, 0.0, newWCS);
      SetReference(lat, lon, aReferenceHeading, newWCS);
   }
}

// =================================================================================================
//! Set the geometric reference point for the zone.
//! (There are 4 different forms of the same method.)
//!
//! @param aReferenceWCS the world coordinates where the zone will be placed.
//! @param aReferenceHeading the new heading at which the zone will be oriented.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetReference(double aReferenceWCS[3], double aReferenceHeading)
{
   if (mRelativeTo == cINTERNAL)
   {
      double lat;
      double lon;
      double alt;
      UtEntity::ConvertWCSToLLA(aReferenceWCS, lat, lon, alt);

      // The provided WCS location may not be a zero altitude, so
      // must calculate another which is guaranteed to be at MSL = 0.
      double newWCS[3];
      UtEntity::ConvertLLAToWCS(lat, lon, 0.0, newWCS);
      SetReference(lat, lon, aReferenceHeading, newWCS);
   }
}

// =================================================================================================
//! Set the geometric reference point for the zone.
//! (There are 4 different forms of the same method.)
//!
//! @param aReferenceLat the latitude at which the zone will be placed.
//! @param aReferenceLon the longitude at which the zone will be placed.
//! @param aReferenceHeading the new heading at which the zone will be oriented.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetReference(double aReferenceLat, double aReferenceLon, double aReferenceHeading)
{
   if (mRelativeTo == cINTERNAL)
   {
      double newWCS[3];
      UtEntity::ConvertLLAToWCS(aReferenceLat, aReferenceLon, 0.0, newWCS);
      SetReference(aReferenceLat, aReferenceLon, aReferenceHeading, newWCS);
   }
}

// =================================================================================================
//! Set the shape type of the zone.
//!
//! @param aType the new enumerated shape type of the zone.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetShapeType(ShapeType aType)
{
   mIsInitialized = false;
   mShapeType     = aType;
}

// =================================================================================================
//! Set the relative type of the zone, which specifies it's relative or fixed location.
//!
//! @param aType the new enumerated relative type of the zone.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetRelativeType(RelativeToType aType)
{
   mIsInitialized = false;
   mRelativeTo    = aType;
}

// =================================================================================================
//! Set the altitude bounds of the zone shape.
//!
//! @param aMinAlt the minimum altitude limit of the zone shape.
//! @param aMaxAlt the maximum altitude limit of the zone shape.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetAltBounds(double aMinAlt, double aMaxAlt)
{
   if (aMinAlt != cUNSPECIFIED && aMaxAlt != cUNSPECIFIED && aMinAlt >= aMaxAlt)
   {
      // should we leave the min & max alone?
      // or default them to unspecified at this time?
      return;
   }

   mIsInitialized = false;
   mMinAlt        = aMinAlt;
   mMaxAlt        = aMaxAlt;
}

// =================================================================================================
//! Set the radial range bounds of the zone shape.
//!
//! @param aMinRadius the minimum radial range limit of the zone shape.
//! @param aMaxRadius the maximum radial range limit of the zone shape.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetRadialBounds(double aMinRadius, double aMaxRadius)
{
   if (aMinRadius != cUNSPECIFIED && aMaxRadius != cUNSPECIFIED && aMinRadius >= aMaxRadius)
   {
      // should we leave the min & max alone?
      // or default them to unspecified at this time?
      return;
   }

   mIsInitialized = false;
   if (aMinRadius != cUNSPECIFIED)
   {
      mMinRadiusSq = aMinRadius * aMinRadius;
   }
   else
   {
      mMinRadiusSq = cUNSPECIFIED;
   }

   if (aMaxRadius != cUNSPECIFIED)
   {
      mMaxRadiusSq = aMaxRadius * aMaxRadius;
   }
   else
   {
      mMaxRadiusSq = cUNSPECIFIED;
   }
}

// =================================================================================================
//! Set the radial angle bounds of the zone shape.
//!
//! @param aStartAngle the minimum radial angle limit of the zone shape.
//! @param aStopAngle the maximum radial angle limit of the zone shape.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetAngleBounds(double aStartAngle, double aStopAngle)
{
   mIsInitialized = false;
   if (aStartAngle != cUNSPECIFIED)
   {
      mStartAngle = UtMath::NormalizeAngle0_TwoPi(aStartAngle);
   }
   else
   {
      mStartAngle = cUNSPECIFIED;
   }

   if (aStopAngle != cUNSPECIFIED)
   {
      mStopAngle = UtMath::NormalizeAngle0_TwoPi(aStopAngle);
   }
   else
   {
      mStopAngle = cUNSPECIFIED;
   }
}

// =================================================================================================
//! Set the elliptical axes of the zone shape.
//!
//! @param aLatAxisLen the latitude axis of an elliptical zone shape.
//! @param aLonAxisLen the longitude axis of an elliptical zone shape.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetEllipticalAxes(double aLatAxisLen, double aLonAxisLen)
{
   mIsInitialized    = false;
   mLateralAxis      = std::max(0.0, aLatAxisLen);
   mLongitudinalAxis = std::max(0.0, aLonAxisLen);
}

// =================================================================================================
//! Set the points / vertices of a polygonal zone shape.
//!
//! @param aPoints the points (in clockwise order) of a polyognal zone shape.
//! @returns 'void'.
// virtual
void WsfZoneDefinition::SetPolyPoints(const std::vector<WsfGeoPoint>& aPoints)
{
   mShapeType       = cPOLYGONAL;
   mIsInitialized   = false;
   mPointsAreLatLon = true;
   mRelativeTo      = cINTERNAL;
   mPoints.clear();

   if (aPoints.empty())
   {
      mMinX = cHUGE;
      mMaxX = cSMALL;
      mMinY = cHUGE;
      mMaxY = cSMALL;
      return;
   }

   // the reference location IS used when:  mRelativeTo == cINTERNAL
   if (mReferenceLat == cUNSPECIFIED)
   {
      mReferenceLat = aPoints[0].GetLat();
      mReferenceLon = aPoints[0].GetLon();
      UtEntity::ConvertLLAToWCS(mReferenceLat, mReferenceLon, 0.0, mReferenceWCS);
   }

   std::vector<WsfGeoPoint>::const_iterator git = aPoints.begin();
   for (; git != aPoints.end(); ++git)
   {
      mPoints.emplace_back(git->GetLat(), git->GetLon());
   }

   // Set quick-and-dirty Zone check parameters (Assumes shape is polygon)
   std::vector<Point>::size_type index;
   for (index = 0; index != mPoints.size(); ++index)
   {
      double x = mPoints[index].mX;
      if (x > mMaxX)
      {
         mMaxX = x;
      }
      if (x < mMinX)
      {
         mMinX = x;
      }

      double y = mPoints[index].mY;
      if (y > mMaxY)
      {
         mMaxY = y;
      }
      if (y < mMinY)
      {
         mMinY = y;
      }
   }
}

// =================================================================================================
//! Add (:at, Lon) the points of a polygonal zone shape.
//! @param aPoint a point to be added (in clockwise order) of a polyognal zone shape.
//! @returns 'void'.
void WsfZoneDefinition::AddPoint(const Point& aPoint)
{
   mPoints.push_back(aPoint);
}

// =================================================================================================
//! Calculate the penetration distance through extruded zone types (circular, elliptical, polygonal).
//! @param aLLA1 The lat, lon, alt of the first tested point.
//! @param aLLA2 The lat, lon, alt of the second tested point.
//! @return The approximate penetration distance along the line between the two points through the zone.
double WsfZoneDefinition::ExtrudedZonePenetrationDistance(const double aLLA1[3], const double aLLA2[3])
{
   double  penetrationDistance = 0.0;
   double  minAlt              = std::min(aLLA1[2], aLLA2[2]);
   double  maxAlt              = std::max(aLLA1[2], aLLA2[2]);
   UtVec3d penetrationVector;

   if (((mMinAlt == WsfZone::cUNSPECIFIED) || (maxAlt >= mMinAlt)) &&
       ((mMaxAlt == WsfZone::cUNSPECIFIED) || (minAlt <= mMaxAlt)))
   {
      UtVec2d endPoint[2];
      endPoint[0].Set(aLLA1[0], aLLA1[1]);
      endPoint[1].Set(aLLA2[0], aLLA2[1]);

      UtVec2d isect[2];
      bool    penetrated = Penetration2D(endPoint[0], endPoint[1], isect[0], isect[1]);

      if (penetrated)
      {
         // find point along the 3d line that intersects with intersection point(s)
         // find altitude difference
         UtVec3d dl;
         dl.Subtract(aLLA2, aLLA1);
         double alt[2] = {aLLA1[2], aLLA2[2]};
         dl.Normalize();

         if (fabs(fabs(dl[2]) - 1.0) > cNEAR_ZERO) // the 2D lat/lon points are not the same
         {
            // We use either lat or lon as the index to use for computations,
            // as we need to avoid the case of delta latitude or delta longitude ~ 0.
            size_t index = 0;
            if (fabs(dl[1]) > fabs(dl[0]))
            {
               index = 1;
            }
            double alt12[2] = {aLLA1[2], aLLA2[2]};
            for (size_t i = 0; i < 2; ++i)
            {
               // Here and below, we use 't' as a parmeter to slide along a line from LLA1 to LLA2.
               // Note that the start point is always lla1 == endpoint[0].
               double t = (isect[i][index] - endPoint[0][index]) / dl[index];
               alt[i]   = aLLA1[2] + t * dl[2];
               if (((mMinAlt != WsfZone::cUNSPECIFIED) && (alt[i] < mMinAlt)) ||
                   ((mMaxAlt != WsfZone::cUNSPECIFIED) && (alt[i] > mMaxAlt)))
               {
                  if (alt12[i] == minAlt)
                  {
                     alt[i] = mMinAlt + 1.1e-6; // see WithinAltitudeBounds; check won't succeed unless the delta is added.
                  }
                  else
                  {
                     alt[i] = mMaxAlt -
                              1.1e-6; // see WithinAltitudeBounds; check won't succeed unless the delta is subtracted.
                  }

                  // Again, compute a parameter to slide along the line from LLA1 to LLA2.
                  // This time we re-compute the lat, lon for the new altitude.
                  t           = (alt[i] - aLLA1[2]) / dl[2];
                  isect[i][0] = aLLA1[0] + t * dl[0];
                  isect[i][1] = aLLA1[1] + t * dl[1];
                  double isectWCS[3];
                  UtEllipsoidalEarth::ConvertLLAToECEF(isect[i][0], isect[i][1], alt[i], isectWCS);
                  if (!PointIsInsidePrivate(isectWCS, isectWCS, 0.0, 0.0))
                  {
                     penetrated = false;
                     break;
                  }
               }
            }
         }
         else // 2D lat/lon points are the same.
         {
            // We have already verified an intersection in altitude, clamp the altitude range to the minimum possible.
            for (size_t i = 0; i < 2; ++i)
            {
               if (alt[i] == minAlt)
               {
                  alt[i] = std::max(minAlt, mMinAlt);
               }
               else // (alt[i] == maxAlt)
               {
                  alt[i] = std::min(maxAlt, mMaxAlt);
               }
            }
         }

         if (penetrated)
         {
            // Compute penetration distance using the ECEF endpoints.
            double i1_WCS[3];
            double i2_WCS[3];
            UtEllipsoidalEarth::ConvertLLAToECEF(isect[0][0], isect[0][1], alt[0], i1_WCS);
            UtEllipsoidalEarth::ConvertLLAToECEF(isect[1][0], isect[1][1], alt[1], i2_WCS);
            penetrationVector.Subtract(i2_WCS, i1_WCS);
            penetrationDistance = penetrationVector.Magnitude();
         }
      }
   }

   return penetrationDistance;
}

// =================================================================================================
//! Calculate the penetration distance through a spherical zone.
//! @param aLLA1 The lat, lon, alt of the first tested point.
//! @param aLLA2 The lat, lon, alt of the second tested point.
//! @return The approximate penetration distance along the line between the two points through the zone.
double WsfZoneDefinition::SphericalZonePenetrationDistance(const double aLLA1[3], const double aLLA2[3])
{
   double penetrationDistance = 0.0;
   double lla[2][3];
   UtVec3d::Set(lla[0], aLLA1);
   UtVec3d::Set(lla[1], aLLA2);
   double alt[2] = {aLLA1[2], aLLA2[2]};

   // Adjust the altitudes in case for any min/max entries.
   // find point along the 3d line that intersects with intersection point(s)
   UtVec3d dl;
   dl.Subtract(aLLA2, aLLA1);
   dl.Normalize();

   double minAlt = std::min(aLLA1[2], aLLA2[2]);
   double maxAlt = std::max(aLLA1[2], aLLA2[2]);

   // Only adjust if the intersect line crosses at least one of the min/max altitudes.
   if ((minAlt != maxAlt) && ((((mMinAlt != WsfZone::cUNSPECIFIED) && ((minAlt < mMinAlt) && (maxAlt > mMinAlt)))) ||
                              (((mMaxAlt != WsfZone::cUNSPECIFIED) && ((minAlt < mMaxAlt) && (maxAlt > mMaxAlt))))))
   {
      for (size_t i = 0; i < 2; ++i)
      {
         if ((alt[i] == minAlt) && (mMinAlt != WsfZone::cUNSPECIFIED))
         {
            alt[i] = mMinAlt + 1.1e-6; // see WithinAltitudeBounds; check won't succeed unless the delta is added.
         }
         else if (mMaxAlt != WsfZone::cUNSPECIFIED)
         {
            alt[i] = mMaxAlt - 1.1e-6; // see WithinAltitudeBounds; check won't succeed unless the delta is subtracted.
         }

         lla[i][2] = alt[i];

         if (fabs(fabs(dl[2]) - 1.0) > cNEAR_ZERO) // the 2D lat/lon points are not the same
         {
            // Compute a parameter to slide along the line from LLA1 to LLA2.
            // This time we re-compute the lat, lon for the new altitude.
            double t  = (alt[i] - aLLA1[2]) / dl[2];
            lla[i][0] = aLLA1[0] + t * dl[0];
            lla[i][1] = aLLA1[1] + t * dl[1];
         }
      }
   }

   // see http://www.ambrsoft.com/TrigoCalc/Sphere/SpherLineIntersection_.htm
   // Convert endpoints to WCS
   UtVec3d p0;
   UtVec3d p1;
   UtEllipsoidalEarth::ConvertLLAToECEF(lla[0][0], lla[0][1], lla[0][2], p0.GetData());
   UtEllipsoidalEarth::ConvertLLAToECEF(lla[1][0], lla[1][1], lla[1][2], p1.GetData());
   UtVec3d cen;
   UtEllipsoidalEarth::ConvertLLAToECEF(mReferenceLat, mReferenceLon, 0.0, cen.GetData()); // todo what about agl with terrain??
   UtVec3d ldiff;
   ldiff.Subtract(p1, p0);
   double  a = ldiff.DotProduct(ldiff);
   UtVec3d cdiff;
   cdiff.Subtract(p0, cen);
   double b        = 2.0 * ldiff.DotProduct(cdiff);
   double c        = cen.DotProduct(cen) + p0.DotProduct(p0) - 2.0 * p0.DotProduct(cen) - mMaxRadiusSq;
   double b2       = b * b;
   double det      = b2 - 4.0 * a * c;
   bool   p0Inside = cdiff.MagnitudeSquared() < mMaxRadiusSq;
   cdiff.Subtract(p1, cen);
   bool    p1Inside = cdiff.MagnitudeSquared() < mMaxRadiusSq;
   UtVec3d isect[2] = {p0, p1};

   if (det >= 0.0)
   {
      double sqrtDet = sqrt(det);
      double t[2]    = {(-b - sqrtDet) / (2.0 * a), (-b + sqrtDet) / (2.0 * a)};
      for (size_t i = 0; i < 2; ++i)
      {
         UtVec3d increment;
         increment.Multiply(ldiff, t[i]);
         isect[i].Add(p0, increment);
      }
      if (p0Inside)
      {
         isect[0] = p0;
      }
      if (p1Inside)
      {
         isect[1] = p1;
      }

      UtVec3d penetrationVector;
      penetrationVector.Subtract(isect[1], isect[0]);
      penetrationDistance = penetrationVector.Magnitude();
   }

   return penetrationDistance;
}

// =================================================================================================
//! Calculate the 2D penetration of a ray through an extruded zone type (circular, elliptical, polygonal).
//! @param aP0 The first point as specified in lat, lon format.
//! @param aP1 The second point as specified in lat, lon format.
//! @aI1 The first intersection point, if found.
//! @aI2 The second intersection point, if found.
//! @return Whether the shape was penetrated by the ray.
bool WsfZoneDefinition::Penetration2D(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2)
{
   bool penetrated = false;
   if (mShapeType == cPOLYGONAL)
   {
      penetrated = PenetrationIntoPolygon(aP0, aP1, aI1, aI2);
   }
   else if (mShapeType == cCIRCULAR)
   {
      penetrated = PenetrationIntoCircle(aP0, aP1, aI1, aI2);
   }
   else if (mShapeType == cELLIPTICAL)
   {
      penetrated = PenetrationIntoEllipse(aP0, aP1, aI1, aI2);
   }
   return penetrated;
}

// =================================================================================================
//! Calculate the 2D penetration of a ray through an elliptical zone type.
//! @param aP0 The first point as specified in lat, lon format.
//! @param aP1 The second point as specified in lat, lon format.
//! @aI1 The first intersection point, if found.
//! @aI2 The second intersection point, if found.
//! @return Whether the shape was penetrated by the ray.
bool WsfZoneDefinition::PenetrationIntoEllipse(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2)
{
   bool    penetrated = false;
   UtVec2d p0         = aP0;
   UtVec2d p1         = aP1;
   UtVec2d relP0      = p0;
   relP0[0] -= mReferenceLat;
   relP0[1] -= mReferenceLon;
   UtVec2d relP1 = p1;
   relP1[0] -= mReferenceLat;
   relP1[1] -= mReferenceLon;

   double heading = mReferenceHeading * UtMath::cRAD_PER_DEG;

   double a = mLateralAxis; // by convention, latitude is along the x axis.
   double b = mLongitudinalAxis;
   if (a < b)
   {
      a       = mLongitudinalAxis;
      b       = mLateralAxis;
      heading = UtMath::NormalizeAngle0_TwoPi(heading - UtMath::cPI_OVER_2);
   }

   // following http://www.ambrsoft.com/TrigoCalc/Circles2/Ellipse/EllipseLine.htm
   a                 = a / 2.0 / UtSphericalEarth::cEARTH_MEAN_RADIUS * UtMath::cDEG_PER_RAD; // semi-axes, in degrees
   b                 = b / 2.0 / UtSphericalEarth::cEARTH_MEAN_RADIUS * UtMath::cDEG_PER_RAD; // semi-axes, in degrees
   double cosLat     = cos(mReferenceLat * UtMath::cRAD_PER_DEG);
   double sinHeading = 0.0;
   double cosHeading = 1.0;

   if (heading != 0.0)
   {
      sinHeading          = sin(heading);
      cosHeading          = cos(heading);
      double cosHd_cosLat = cosHeading / cosLat;
      double sinHd_cosLat = sinHeading / cosLat;
      b *= sqrt(cosHd_cosLat * cosHd_cosLat + sinHeading * sinHeading);
      a *= sqrt(sinHd_cosLat * sinHd_cosLat + cosHeading * cosHeading);
      UtVec2d relP0t = relP0;
      UtVec2d relP1t = relP1;
      relP0[0]       = cosHeading * relP0t[0] - sinHeading * relP0t[1];
      relP0[1]       = cosHeading * relP0t[1] + sinHeading * relP0t[0];
      relP1[0]       = cosHeading * relP1t[0] - sinHeading * relP1t[1];
      relP1[1]       = cosHeading * relP1t[1] + sinHeading * relP1t[0];
   }
   else
   {
      b /= cosLat;
   }

   double a2 = a * a;
   double b2 = b * b;

   double  dx = relP1[0] - relP0[0];
   UtVec2d relI1;
   UtVec2d relI2;

   if (fabs(dx) >= cNEAR_ZERO)
   {
      double dy  = relP1[1] - relP0[1];
      double m   = dy / dx;
      double c   = relP0[1] - m * relP0[0];
      double m2  = m * m;
      double det = a2 * m2 + b2 - c * c;

      if (det >= 0.0)
      {
         penetrated     = true;
         double sqrtDet = sqrt(det);
         double denom   = a2 * m2 + b2;

         relI1 = UtVec2d((-a2 * m * c + a * b * sqrtDet) / denom, (b2 * c + a * b * m * sqrtDet) / denom);
         relI2 = UtVec2d((-a2 * m * c - a * b * sqrtDet) / denom, (b2 * c - a * b * m * sqrtDet) / denom);
      }
   }
   else // latitude is constant
   {
      double det = b2 * (1.0 - relP0[0] * relP0[0] / a2);
      if (det >= 0.0)
      {
         penetrated     = true;
         double sqrtDet = sqrt(det);
         relI1          = UtVec2d(relP0[0], -sqrtDet);
         relI2          = UtVec2d(relP0[0], sqrtDet);
      }
   }

   if (penetrated)
   {
      // Check whether either of the points are within the polygon.
      bool p0Inside = ((relP0[0] * relP0[0] / a2 + relP0[1] * relP0[1] / b2) < 1.0);
      bool p1Inside = ((relP1[0] * relP1[0] / a2 + relP1[1] * relP1[1] / b2) < 1.0);

      // If so, disambiguate which point overrides which intersection.
      if (((relP0[0] < relP1[0]) && (relI1[0] > relI2[0])) || ((relP0[0] > relP1[0]) && (relI1[0] < relI2[0])) ||
          ((relP0[1] < relP1[1]) && (relI1[1] > relI2[1])) || ((relP0[1] > relP1[1]) && (relI1[1] < relI2[1])))
      {
         // swap
         UtVec2d relI1t = relI1;
         relI1          = relI2;
         relI2          = relI1t;
      }
      if (p0Inside)
      {
         relI1 = relP0;
      }
      if (p1Inside)
      {
         relI2 = relP1;
      }

      aI1[0] = cosHeading * relI1[0] + sinHeading * relI1[1] + mReferenceLat;
      aI1[1] = cosHeading * relI1[1] - sinHeading * relI1[0] + mReferenceLon;
      aI2[0] = cosHeading * relI2[0] + sinHeading * relI2[1] + mReferenceLat;
      aI2[1] = cosHeading * relI2[1] - sinHeading * relI2[0] + mReferenceLon;
   }

   return penetrated;
}

// =================================================================================================
//! Calculate the 2D penetration of a ray through an circular zone type.
//! @param aP0 The first point as specified in lat, lon format.
//! @param aP1 The second point as specified in lat, lon format.
//! @aI1 The first intersection point, if found.
//! @aI2 The second intersection point, if found.
//! @return Whether the shape was penetrated by the ray.
bool WsfZoneDefinition::PenetrationIntoCircle(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2)
{
   bool    penetrated = false;
   double  dx         = aP1[0] - aP0[0];
   double  dy         = aP1[1] - aP0[1];
   double  dr         = sqrt(dx * dx + dy * dy);
   UtVec2d relP0      = aP0;
   UtVec2d relP1      = aP1;
   relP0[0] -= mReferenceLat;
   relP1[0] -= mReferenceLat;
   relP0[1] -= mReferenceLon;
   relP1[1] -= mReferenceLon;
   double              D                = UtVec2d::CrossProduct(relP0, relP1);
   double              D2               = D * D;
   double              dr2              = dr * dr;
   static const double M_PER_DEG_LAT_SQ = 12347654400.0;
   double              r2               = mMaxRadiusSq / M_PER_DEG_LAT_SQ;
   double              del              = r2 * dr2 - D2;
   double              cosLat           = cos(mReferenceLat * UtMath::cRAD_PER_DEG);

   if (del >= 0.0)
   {
      penetrated         = true;
      double commonTerm1 = sqrt(del);
      double sgn         = 1.0;
      if (std::signbit(dy))
      {
         sgn = -1.0;
      }
      if (UtVec2d::DotProduct(relP0, relP0) > r2)
      {
         aI1[0] = (D * dy - sgn * dx * commonTerm1) / dr2 + mReferenceLat;
         aI1[1] = (-D * dx - fabs(dy) * commonTerm1) / dr2 / cosLat + mReferenceLon;
      }
      else
      {
         aI1 = aP0;
      }
      if (UtVec2d::DotProduct(relP1, relP1) > r2)
      {
         aI2[0] = (D * dy + sgn * dx * commonTerm1) / dr2 + mReferenceLat;
         aI2[1] = (-D * dx + fabs(dy) * commonTerm1) / dr2 / cosLat + mReferenceLon;
      }
      else
      {
         aI2 = aP1;
      }
   }

   return penetrated;
}

// =================================================================================================
//! Calculate the 2D penetration of a ray through an polygonal zone type.
//! @param aP0 The first point as specified in lat, lon format.
//! @param aP1 The second point as specified in lat, lon format.
//! @aI1 The first intersection point, if found.
//! @aI2 The second intersection point, if found.
//! @return Whether the shape was penetrated by the ray.
bool WsfZoneDefinition::PenetrationIntoPolygon(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2)
{
   if (aP0 == aP1)
   {
      //  S is a single point, so test for point inclusion of P0
      if (WithinPolygonSides(aP0[0], aP0[1]))
      {
         aI1 = aP0;
         aI2 = aP1;
         return true;
      }
      else
      {
         return false;
      }
   }

   double  tE = 0.0;       // the maximum entering segment parameter
   double  tL = 1.0;       // The minimum leaving segment parameter
   UtVec2d dS = aP1 - aP0; // the segment direction vector

   // Helper method to convert a point into a UtVec2d.
   auto PointVec = [](const WsfZoneDefinition::Point& aPoint) { return UtVec2d(aPoint.mX, aPoint.mY); };

   size_t numEdges = mPoints.size();
   for (size_t edgeNum = 0; edgeNum < numEdges; ++edgeNum)
   {
      UtVec2d vertex1 = PointVec(mPoints[edgeNum]);
      UtVec2d vertex2;
      if (edgeNum == (numEdges - 1))
      {
         vertex2 = PointVec(mPoints[0]);
      }
      else
      {
         vertex2 = PointVec(mPoints[edgeNum + 1]);
      }
      UtVec2d edge  = vertex2 - vertex1;
      UtVec2d p0mVi = aP0 - vertex1;
      double  N     = UtVec2d::CrossProduct(edge, p0mVi);
      double  D     = -UtVec2d::CrossProduct(edge, dS);

      if (fabs(D) < cNEAR_ZERO) // S is parallel to the edge ei
      {
         if (N < 0) // 0 is outside the edge ei
         {
            return false; // S cannot intersect polygon
         }
         else
         {
            // S cannot enter or leave polygon across edge ei;
            // Continue to process the next edge.
            continue;
         }
      }
      double t = N / D;
      if (D < 0) // Segment S is entering polygon across edge ei.
      {
         tE = std::max(tE, t);
         if (tE > tL) // Segment S enters polygon after leaving.
         {
            return false; // S cannot intersect polygon.
         }
      }
      else // (D > 0) Segment S is leaving polygon across edge ei.
      {
         tL = std::min(tL, t);
         if (tL < tE) // Segment S leaves polygon before entering.
         {
            return false; // S cannot intersect polygon.
         }
      }
   }

   //    Output: [Note:to get here, one must have tE <= tL]
   //       there is a valid intersection of S with polygon
   aI1 = aP0 + tE * dS; // the entering point
   aI2 = aP0 + tL * dS; // the exiting point
   return true;
}

// =================================================================================================
void WsfZoneDefinition::SetPolyPointsRelative(const std::vector<UtVec3d>& aPoints)
{
   mIsInitialized   = false;
   mPointsAreLatLon = false;
   mRelativeTo      = cOBSERVER;
   mPoints.clear();

   if (aPoints.empty())
   {
      mMinX = cHUGE;
      mMaxX = cSMALL;
      mMinY = cHUGE;
      mMaxY = cSMALL;
      return;
   }

   std::vector<UtVec3d>::const_iterator vit = aPoints.begin();
   for (; vit != aPoints.end(); ++vit)
   {
      mPoints.emplace_back(vit->Get(0), vit->Get(1));
   }

   // Set quick-and-dirty Zone check parameters (Assumes shape is polygon)
   std::vector<Point>::size_type index;
   for (index = 0; index != mPoints.size(); ++index)
   {
      double x = mPoints[index].mX;
      if (x > mMaxX)
      {
         mMaxX = x;
      }
      if (x < mMinX)
      {
         mMinX = x;
      }

      double y = mPoints[index].mY;
      if (y > mMaxY)
      {
         mMaxY = y;
      }
      if (y < mMinY)
      {
         mMinY = y;
      }
   }
}

// =================================================================================================
//! return the geopoint of the reference point
// virtual
WsfGeoPoint WsfZoneDefinition::Reference()
{
   return WsfGeoPoint(mReferenceWCS);
}

// =================================================================================================
//! Returns true if the zone points represent (Lat, Lon), false otherwise.  Only if true is returned,
//! argument values represent the lower left and upper right corners of the zone extrema.
bool WsfZoneDefinition::GetLatLonExtrema(double& aSouthernLat, double& aWesternLon, double& aNorthernLat, double& aEasternLon) const
{
   if (mPointsAreLatLon)
   {
      if (mShapeType == cPOLYGONAL)
      {
         // Must have polygon shape for this function to make any sense.
         std::vector<WsfZone::Point>::const_iterator ptsIter = mPoints.begin();

         aSouthernLat = 90.0;
         aWesternLon  = 180.0;
         aNorthernLat = -90.0;
         aEasternLon  = -180.0;

         while (ptsIter != mPoints.end())
         {
            if (ptsIter->mX < aSouthernLat)
            {
               aSouthernLat = ptsIter->mX;
            }

            if (ptsIter->mY < aWesternLon)
            {
               aWesternLon = ptsIter->mY;
            }

            if (ptsIter->mX > aNorthernLat)
            {
               aNorthernLat = ptsIter->mX;
            }

            if (ptsIter->mY > aEasternLon)
            {
               aEasternLon = ptsIter->mY;
            }

            ++ptsIter;
         }

         return true;
      }
      else if ((mShapeType == cCIRCULAR) || (mShapeType == cSPHERICAL))
      {
         double notUsedValue;
         double dist = sqrt(mMaxRadiusSq);
         UtSphericalEarth::ExtrapolateGreatCirclePosition(mReferenceLat, mReferenceLon, 90.0, dist, notUsedValue, aEasternLon);
         UtSphericalEarth::ExtrapolateGreatCirclePosition(mReferenceLat, mReferenceLon, -90.0, dist, notUsedValue, aWesternLon);
         UtSphericalEarth::ExtrapolateGreatCirclePosition(mReferenceLat, mReferenceLon, 0.0, dist, aNorthernLat, notUsedValue);
         UtSphericalEarth::ExtrapolateGreatCirclePosition(mReferenceLat, mReferenceLon, 180.0, dist, aSouthernLat, notUsedValue);
         return false;
      }
      else if (mShapeType == cELLIPTICAL)
      {
         // If there is a need for this one, will have to figure out how to get the extrema.
         return false;
      }
   }

   return false;
}

// =================================================================================================
double WsfZoneDefinition::GetRadius() const
{
   return sqrt(mMaxRadiusSq);
}

// =================================================================================================
double WsfZoneDefinition::GetMinRadius() const
{
   return sqrt(mMinRadiusSq);
}

// =================================================================================================
double WsfZoneDefinition::GetReferenceHeading() const
{
   return mReferenceHeading;
}

// =================================================================================================
double WsfZoneDefinition::GetLatAxisLength() const
{
   return mLateralAxis;
}

// =================================================================================================
double WsfZoneDefinition::GetLonAxisLength() const
{
   return mLongitudinalAxis;
}

// =================================================================================================
void WsfZoneDefinition::GetAngleBounds(double& aStartAngle, double& aStopAngle) const
{
   if (mStartAngle == cUNSPECIFIED)
   {
      aStartAngle = 0.0;
   }
   else
   {
      aStartAngle = mStartAngle;
   }

   if (mStopAngle == cUNSPECIFIED)
   {
      aStopAngle = UtMath::cTWO_PI;
   }
   else
   {
      aStopAngle = mStopAngle;
   }
}

// =================================================================================================
void WsfZoneDefinition::GetAltBounds(double& aMinAlt, double& aMaxAlt) const
{
   aMinAlt = mMinAlt;
   aMaxAlt = mMaxAlt;
}

// =================================================================================================
WsfPlatform* WsfZoneDefinition::GetReferencePlatform(WsfSimulation* aSimulationPtr) const
{
   WsfPlatform* platformPtr = nullptr;
   if (mReferencePlatformIndex != 0 && aSimulationPtr)
   {
      platformPtr = aSimulationPtr->GetPlatformByIndex(mReferencePlatformIndex);
   }

   if (platformPtr == nullptr && aSimulationPtr)
   {
      // One of the following is true:
      // 1) This is the first call OR the platform has not come into existence.
      // 2) The platform had at one time existed, but has been deleted.
      //
      // So go back and try to find the platform by name (which is slower than finding by index).

      platformPtr = aSimulationPtr->GetPlatformByName(mReferencePlatformNameId);
   }
   return platformPtr;
}

// =================================================================================================
//! returns an array of geopoints that make up the vertices of the zone
//! ***** currently doesn't support altitude
void WsfZoneDefinition::GetPoints(std::vector<WsfGeoPoint>& aPoints) const
{
   size_t numPoints = mPoints.size();

   aPoints.resize(numPoints);
   for (size_t i = 0; i < numPoints; ++i)
   {
      aPoints[i].SetLocationLLA(mPoints[i].mX, mPoints[i].mY, 0);
   }
}

// =================================================================================================
//! draw the zone using WsfDraw
// virtual
void WsfZoneDefinition::DebugDrawZone(WsfDraw& aDraw)
{
   aDraw.SetColor(0.75, 1.0, 1.0);
   aDraw.SetLineSize(3);
   aDraw.SetDuration(5.0);

   Draw(aDraw);
}

// =================================================================================================
void WsfZoneDefinition::DebugDrawZoneRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading)
{
   aDraw.SetColor(0.75, 1.0, 1.0);
   aDraw.SetLineSize(3);
   aDraw.SetDuration(5.0);

   DrawRelative(aDraw, aEyePointWCS, aLookHeading);
}

// =================================================================================================
//! draw the centroid using WsfDraw in an x
// virtual
void WsfZoneDefinition::DebugDrawCentroid(WsfDraw& aDraw)
{
   aDraw.SetColor(0.0, 0.0, 0.0);
   aDraw.SetLineSize(1);
   aDraw.SetDuration(5.0);

   DrawCentroid(aDraw);
}

// =================================================================================================
//! draw the bounding box using WsfDraw
// virtual
void WsfZoneDefinition::DebugDrawBounds(WsfDraw& aDraw)
{
   aDraw.SetColor(1.0, 1.0, 0.0);
   aDraw.SetLineSize(3);
   aDraw.SetDuration(5.0);

   DrawBounds(aDraw);
}

// =================================================================================================
//! Draw the zone using WsfDraw
//! @param aDraw The draw object to be written to
// virtual
void WsfZoneDefinition::Draw(WsfDraw& aDraw)
{
   if (mShapeType != cPOLYGONAL || mPoints.empty()) // must be a polygon (change this & cover all shapes in the future???)
   {
      return;
   }

   aDraw.BeginLines();
   size_t numPoints = mPoints.size();
   double alt       = 0.0;
   if (mMinAlt != cUNSPECIFIED)
   {
      alt = mMinAlt;
   }

   // Takes in local coordinates and outputs the absolute LLA coordinates for the point
   auto GetRelativeLLA = [this](double aPoint[3])
   {
      double transECEF[3][3];
      double refECEF[3];

      // Convert local relative coordinates to latitude and longitude
      UtEllipsoidalEarth::ComputeNEDTransform(mReferenceLat, mReferenceLon, 0, transECEF, refECEF);
      UtEllipsoidalEarth::ConvertLocalToLLA(refECEF, transECEF, aPoint, aPoint[0], aPoint[1], aPoint[2]);

      // Rotate the latitude and longitude to match the reference heading
      if (mReferenceHeading != 0)
      {
         double rotLat = (aPoint[0] - mReferenceLat) * cos(mReferenceHeading) -
                         (aPoint[1] - mReferenceLon) * sin(mReferenceHeading) + mReferenceLat;
         double rotLon = (aPoint[0] - mReferenceLat) * sin(mReferenceHeading) +
                         (aPoint[1] - mReferenceLon) * cos(mReferenceHeading) + mReferenceLon;
         aPoint[0] = rotLat;
         aPoint[1] = rotLon;
      }
   };

   double point[3] = {mPoints[0].mX, mPoints[0].mY, 0};

   // If point is not LLA, this and all other points must be converted before being used.
   if (!mPointsAreLatLon)
   {
      // Get most recent reference values through simulation.
      UpdateReference(aDraw.GetDrawManager()->GetSimulation());
      GetRelativeLLA(point);
   }

   for (size_t i = 0; i < numPoints; ++i)
   {
      size_t loopIdx      = (i + 1) % numPoints;
      double nextPoint[3] = {mPoints[loopIdx].mX, mPoints[loopIdx].mY, 0};

      if (!mPointsAreLatLon)
      {
         GetRelativeLLA(nextPoint);
      }

      aDraw.VertexLLA(point[0], point[1], alt);
      aDraw.VertexLLA(nextPoint[0], nextPoint[1], alt);

      if (mMaxAlt != cUNSPECIFIED)
      {
         aDraw.VertexLLA(point[0], point[1], mMaxAlt);
         aDraw.VertexLLA(nextPoint[0], nextPoint[1], mMaxAlt);

         aDraw.VertexLLA(point[0], point[1], alt);
         aDraw.VertexLLA(point[0], point[1], mMaxAlt);

         aDraw.VertexLLA(nextPoint[0], nextPoint[1], alt);
         aDraw.VertexLLA(nextPoint[0], nextPoint[1], mMaxAlt);
      }
      point[0] = nextPoint[0];
      point[1] = nextPoint[1];
   }
   aDraw.End();
}

// =================================================================================================
void WsfZoneDefinition::DrawRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading)
{
   // must be a relative zone
   // must be relative coordinates (not geo or world coordinates)
   if (mRelativeTo == cINTERNAL || mPointsAreLatLon == true)
   {
      return Draw(aDraw);
   }

   if (mShapeType != cPOLYGONAL || // must be a polygon (change this & cover all shapes in the future???)
       mPoints.empty())            // must have at lease 1 point
   {
      return;
   }

   // convert the relative coordinates (ECS) into world coordinates (WCS), for drawing
   UtEntity uteRef; // reference entity used to make the conversions
   uteRef.SetLocationWCS(aEyePointWCS);
   uteRef.SetOrientationNED(aLookHeading, 0.0, 0.0);
   double locECS[3] = {0, 0, 0}; // stores the relative coordinates of the zone point
   double locWCS[3] = {0, 0, 0}; // where the world coordinates of the zone point are put

   aDraw.BeginLines();
   size_t numPoints = mPoints.size();
   for (size_t i = 0; i < numPoints; ++i)
   {
      size_t loopIdx = (i + 1) % numPoints;

      locECS[0] = mPoints[i].mX;
      locECS[1] = mPoints[i].mY;
      uteRef.ConvertECSToWCS(locECS, locWCS);
      aDraw.VertexWCS(locWCS);

      locECS[0] = mPoints[loopIdx].mX;
      locECS[1] = mPoints[loopIdx].mY;
      uteRef.ConvertECSToWCS(locECS, locWCS);
      aDraw.VertexWCS(locWCS);
   }
   aDraw.End();
}

// =================================================================================================
//! draw the centroid using WsfDraw in an x
// virtual
void WsfZoneDefinition::DrawCentroid(WsfDraw& aDraw)
{
   if (mShapeType != cPOLYGONAL)
   {
      return;
   }

   aDraw.BeginLines();
   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX - 0.1, mCentroid.mY + 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX + 0.1, mCentroid.mY + 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX + 0.1, mCentroid.mY - 0.1, 0.0);

   aDraw.VertexLLA(mCentroid.mX, mCentroid.mY, 0.0);
   aDraw.VertexLLA(mCentroid.mX - 0.1, mCentroid.mY - 0.1, 0.0);
   aDraw.End();
}

// =================================================================================================
//! draw the bounding box using WsfDraw
// virtual
void WsfZoneDefinition::DrawBounds(WsfDraw& aDraw)
{
   if (mShapeType != cPOLYGONAL)
   {
      return;
   }

   aDraw.BeginLines();
   aDraw.VertexLLA(mMinX, mMinY, 0.0);
   aDraw.VertexLLA(mMinX, mMaxY, 0.0);

   aDraw.VertexLLA(mMinX, mMaxY, 0.0);
   aDraw.VertexLLA(mMaxX, mMaxY, 0.0);

   aDraw.VertexLLA(mMaxX, mMaxY, 0.0);
   aDraw.VertexLLA(mMaxX, mMinY, 0.0);

   aDraw.VertexLLA(mMaxX, mMinY, 0.0);
   aDraw.VertexLLA(mMinX, mMinY, 0.0);
   aDraw.End();
}

// =================================================================================================
// Protected Methods
// =================================================================================================

// =================================================================================================
//! Initializes and validates the attributes of the zone shape.
//!
//! @returns 'true' if initialization successful.
// virtual
void WsfZoneDefinition::Initialize(const WsfScenario& aScenario)
{
   if (mIsInitialized)
   {
      return;
   }
   mIsInitialized = true;

   // If a different Zone is referenced, then copy the
   // referenced Zone attributes into this one
   // (translated and rotated as desired).
   if (mRefZonePtr != nullptr)
   {
      bool ok = true;
      if ((mShapeType != cNO_SHAPE) && (mShapeType != mRefZonePtr->mShapeType))
      {
         auto out = ut::log::warning() << "Zone Definition: Shape type conflict.";
         out.AddNote() << "Shape: " << ShapeToString(mShapeType);
         out.AddNote() << "Ref Zone Shape: " << ShapeToString(mRefZonePtr->mShapeType);

         ok = false;
      }
      else
      {
         mShapeType = mRefZonePtr->mShapeType;
      }
      if ((mMinAlt != cUNSPECIFIED) && (mMinAlt != mRefZonePtr->mMinAlt))
      {
         auto out = ut::log::warning() << "Zone Definition: Minimum altitude conflict.";
         out.AddNote() << "Min Alt: " << mMinAlt;
         out.AddNote() << "Ref Zone Min Alt: " << mRefZonePtr->mMinAlt;

         ok = false;
      }
      else
      {
         mMinAlt = mRefZonePtr->mMinAlt;
      }

      if ((mMaxAlt != cUNSPECIFIED) && (mMaxAlt != mRefZonePtr->mMaxAlt))
      {
         auto out = ut::log::warning() << "Zone Definition: Maximum altitude conflict.";
         out.AddNote() << "Max Alt: " << mMaxAlt;
         out.AddNote() << "Ref Zone Max Alt: " << mRefZonePtr->mMaxAlt;
         ok = false;
      }
      else
      {
         mMaxAlt = mRefZonePtr->mMaxAlt;
      }
      if ((mMinRadiusSq != cUNSPECIFIED) && (mMinRadiusSq != mRefZonePtr->mMinRadiusSq))
      {
         auto out = ut::log::warning() << "Zone Definition: Minimum radius conflict.";
         out.AddNote() << "Min Radius: " << std::sqrt(mMinRadiusSq);
         out.AddNote() << "Ref Zone Min Radius: " << std::sqrt(mRefZonePtr->mMinRadiusSq);
         ok = false;
      }
      else
      {
         mMinRadiusSq = mRefZonePtr->mMinRadiusSq;
      }

      if ((mMaxRadiusSq != cUNSPECIFIED) && (mMaxRadiusSq != mRefZonePtr->mMaxRadiusSq))
      {
         auto out = ut::log::warning() << "Zone Definition: Maximum radius conflict.";
         out.AddNote() << "Max Radius: " << std::sqrt(mMaxRadiusSq);
         out.AddNote() << "Ref Zone Max Radius: " << std::sqrt(mRefZonePtr->mMaxRadiusSq);
         ok = false;
      }
      else
      {
         mMaxRadiusSq = mRefZonePtr->mMaxRadiusSq;
      }
      if ((mLongitudinalAxis != cUNSPECIFIED) && (mLongitudinalAxis != mRefZonePtr->mLongitudinalAxis))
      {
         auto out = ut::log::warning() << "Zone Definition: Longitudinal axis conflict.";
         out.AddNote() << "Long Axis: " << mLongitudinalAxis;
         out.AddNote() << "Ref Zone Long Axis: " << mRefZonePtr->mLongitudinalAxis;
         ok = false;
      }
      else
      {
         mLongitudinalAxis = mRefZonePtr->mLongitudinalAxis;
      }

      if ((mLateralAxis != cUNSPECIFIED) && (mLateralAxis != mRefZonePtr->mLateralAxis))
      {
         auto out = ut::log::warning() << "Zone Definition: Lateral axis conflict.";
         out.AddNote() << "Lat Axis: " << mLateralAxis;
         out.AddNote() << "Ref Zone Lat Axis: " << mRefZonePtr->mLateralAxis;
         ok = false;
      }
      else
      {
         mLateralAxis = mRefZonePtr->mLateralAxis;
      }

      if ((mStartAngle != cUNSPECIFIED) && (mStartAngle != mRefZonePtr->mStartAngle))
      {
         auto out = ut::log::warning() << "Zone Definition: Start angle conflict.";
         out.AddNote() << "Start Angle: " << mStartAngle;
         out.AddNote() << "Ref Zone Start Angle: " << mRefZonePtr->mStartAngle;
         ok = false;
      }
      else
      {
         mStartAngle = mRefZonePtr->mStartAngle;
      }

      if ((mStopAngle != cUNSPECIFIED) && (mStopAngle != mRefZonePtr->mStopAngle))
      {
         auto out = ut::log::warning() << "Zone Definition: Stop angle conflict.";
         out.AddNote() << "Stop Angle: " << mStopAngle;
         out.AddNote() << "Ref Zone Stop Angle: " << mRefZonePtr->mStopAngle;
         ok = false;
      }
      else
      {
         mStopAngle = mRefZonePtr->mStopAngle;
      }

      if (!mPoints.empty())
      {
         ut::log::warning() << "Zone Definition: Ignoring polygonal specification.";
         ok = false;
         mPoints.clear();
      }
      mPointsAreLatLon = false;

      if (mRefZonePtr->mPointsAreLatLon)
      {
         // This is a special case.  Here we need to turn the (lat,lon) points into
         // (X,Y) offsets from the "other" Zone's reference position and heading.
         // (Will later transform them into a new location...)
         UtEntity uteRef;
         uteRef.SetLocationWCS(mRefZonePtr->mReferenceWCS);
         uteRef.SetOrientationNED(mRefZonePtr->mReferenceHeading, 0.0, 0.0);

         for (auto& pnt : mRefZonePtr->mPoints)
         {
            // These values are in utE's ECS frame...
            double   offsetECS[3];
            UtEntity point;
            point.SetLocationLLA(pnt.mX, // lat
                                 pnt.mY, // lon
                                 0.0);   // alt
            uteRef.GetRelativeLocationNED(&point, offsetECS);
            mPoints.emplace_back(offsetECS[0], offsetECS[1]);
         }
      }
      else
      {
         mPoints = mRefZonePtr->mPoints;
      }

      if ((mRelativeTo == cINTERNAL) && (mRelativeTo != mRefZonePtr->mRelativeTo))
      {
         ut::log::warning() << "Zone Definition: Zone relationship conflict.";
         ok = false;
      }
      else
      {
         mRelativeTo = mRefZonePtr->mRelativeTo;
      }

      if (mRelativeTo == cINTERNAL)
      {
         if (mReferenceWCS[0] == cUNSPECIFIED)
         {
            ut::log::warning() << "Zone Definition: No reference location.";
            ok = false;
         }
      }

      if (!ok)
      {
         auto out = ut::log::warning() << "Zone Definition: Problems encountered between zone and reference zone.";
         out.AddNote() << "Zone: " << GetName();
         out.AddNote() << "Reference Zone: " << mRefZonePtr->GetName();
         out.AddNote() << "See the preceding messages describing the problems.";
      }
   }

   // Set quick-and-dirty Zone check parameters.
   if (mShapeType == cPOLYGONAL)
   {
      std::vector<Point>::size_type index;

      for (index = 0; index != mPoints.size(); ++index)
      {
         double x = mPoints[index].mX;
         if (x > mMaxX)
         {
            mMaxX = x;
         }
         if (x < mMinX)
         {
            mMinX = x;
         }

         double y = mPoints[index].mY;
         if (y > mMaxY)
         {
            mMaxY = y;
         }
         if (y < mMinY)
         {
            mMinY = y;
         }
      }

      // only polygons are supported at this time
      CalculateArea();
      CalculateCentroid();
   }
   if (!mModifierList.empty())
   {
      if (!mPointsAreLatLon)
      {
         // I'm not sure why this one uses GetName() for the "relative zone" instead of the "zone", but that's what was
         // there previously.
         auto out = ut::log::warning() << "Modifier values for relative zone will be ignored.";
         out.AddNote() << "Relative Zone: " << GetName();
      }
      else if ((mShapeType == cPOLYGONAL) && (mReferenceHeading != 0.0))
      {
         auto out = ut::log::warning() << "'heading' input will be ignored for zone-based attenuation calculations.";
         out.AddNote() << "Zone: " << GetName();
      }
      else if ((mStartAngle != WsfZone::cUNSPECIFIED) || (mStopAngle != WsfZone::cUNSPECIFIED))
      {
         auto out = ut::log::warning()
                    << "'start_angle' and 'end_angle' input will be ignored for zone-based attenuation calculations.";
         out.AddNote() << "Zone: " << GetName();
      }
      else if (mMinRadiusSq != WsfZone::cUNSPECIFIED)
      {
         auto out =
            ut::log::warning() << "'minimum_radius' input will be ignored for zone-based attenuation calculations.";
         out.AddNote() << "Zone: " << GetName();
      }
   }
}

// =================================================================================================
// Private Methods
// =================================================================================================

// =================================================================================================
//! Calculate the area of the polygon.
//! A = 1/2 * SUM(Xi*Yi+1 - Xi+1*Yi)
// private
void WsfZoneDefinition::CalculateArea()
{
   double xyFactor = 0.0;

   size_t numPoints = mPoints.size();
   for (size_t i = 0; i < numPoints; ++i)
   {
      size_t j = (i + 1) % numPoints; // this keeps the i+1 from going out of bounds (meaning the last element is the
                                      // same as the first)

      // (Xi*Yi+1 - Xi+1*Yi)
      xyFactor += (mPoints[i].mX * mPoints[j].mY) - (mPoints[j].mX * mPoints[i].mY);
   }

   mArea = xyFactor * 0.5;
}

// =================================================================================================
//! Calculate the centroid of the polygon.
//! Cx = 1/6A * SUM((Xi + Xi+1)*(Xi*Yi+1 - Xi+1*Yi))
//! Cy = 1/6A * SUM((Yi + Yi+1)*(Xi*Yi+1 - Xi+1*Yi))
// private
void WsfZoneDefinition::CalculateCentroid()
{
   double xTotal = 0.0;
   double yTotal = 0.0;

   size_t numPoints = mPoints.size();
   for (size_t i = 0; i < numPoints; ++i)
   {
      size_t j = (i + 1) % numPoints; // this keeps the i+1 from going out of bounds (meaning the last element is the
                                      // same as the first)

      // (Xi*Yi+1 - Xi+1*Yi)
      double xyFactor = (mPoints[i].mX * mPoints[j].mY) - (mPoints[j].mX * mPoints[i].mY);

      // (Xi + Xi+1)
      double xFactor = mPoints[i].mX + mPoints[j].mX;

      // (Yi + Yi+1)
      double yFactor = mPoints[i].mY + mPoints[j].mY;

      xTotal += (xFactor * xyFactor);
      yTotal += (yFactor * xyFactor);
   }

   mCentroid.mX = (1.0 / (6.0 * mArea)) * xTotal;
   mCentroid.mY = (1.0 / (6.0 * mArea)) * yTotal;
}

// =================================================================================================
void WsfZoneDefinition::ConvertWCSToLocalFrame(const double aWCSLocation[3],
                                               const double aEyePointWCS[3],
                                               const double aLookHeading,
                                               double       aPointInMyFrame[3]) const
{
   // Convert a global viewpoint into one that makes sense locally.
   double lat;
   double lon;
   double alt; // not used

   UtEntity::ConvertWCSToLLA(aEyePointWCS, lat, lon, alt);
   UtEntity utE;
   utE.SetLocationLLA(lat, lon, 0.0);
   utE.SetOrientationNED(aLookHeading, 0.0, 0.0);
   utE.ConvertWCSToECS(aWCSLocation, aPointInMyFrame);
}

// =================================================================================================
// private
void WsfZoneDefinition::SetReference(double aReferenceLat,
                                     double aReferenceLon,
                                     double aReferenceHeading,
                                     double aReferenceWCS[3])
{
   // This method relocates a relative Zone:
   // (mRelativeTo == cINTERNAL has already been verified)

   mReferenceLat     = aReferenceLat;
   mReferenceLon     = aReferenceLon;
   mReferenceHeading = aReferenceHeading;
   UtVec3d::Set(mReferenceWCS, aReferenceWCS);
}

// =================================================================================================
// private
bool WsfZoneDefinition::WithinAngleBounds(const double aPointInMyFrame[3], double& aViewedAngle) const
{
   bool withinAngleBounds = true;
   if ((mStartAngle != cUNSPECIFIED) && (mStopAngle != cUNSPECIFIED))
   {
      aViewedAngle = atan2(aPointInMyFrame[1], aPointInMyFrame[0]);
      aViewedAngle = UtMath::NormalizeAngle0_TwoPi(aViewedAngle);
      // NOTE: If the angle range is zero, close to zero or close to 2pi then then
      // range is treated as 2pi (unlimited). This allows us to handle the case where
      // unit conversion and normalization of an angle=360 may result in 0, near 0 or
      // near 2pi.
      if (mStopAngle > mStartAngle)
      {
         // Angle range does not span the zero degree line.
         if ((mStopAngle - mStartAngle) > 1.0E-6)
         {
            if ((aViewedAngle < mStartAngle) || (aViewedAngle > mStopAngle))
            {
               withinAngleBounds = false;
            }
         }
      }
      else if (mStopAngle < mStartAngle)
      {
         // Angle range spans the 0 degree line.
         double deltaAngle = mStartAngle - mStopAngle;
         if ((deltaAngle > 1.0E-6) && (deltaAngle < UtMath::cTWO_PI - 1.0E-6))
         {
            if ((aViewedAngle < mStartAngle) && (aViewedAngle > mStopAngle))
            {
               withinAngleBounds = false;
            }
         }
      }
   }
   return withinAngleBounds;
}

// =================================================================================================
// private
bool WsfZoneDefinition::WithinAltitudeBounds(const double aViewedAlt) const
{
   if ((mMinAlt != cUNSPECIFIED) && ((aViewedAlt - mMinAlt) < -1.0e-6))
   {
      return false;
   }

   if ((mMaxAlt != cUNSPECIFIED) && ((aViewedAlt - mMaxAlt) > -1.0e-6))
   {
      return false;
   }

   return true;
}

// =================================================================================================
// private
bool WsfZoneDefinition::WithinAltitudeBounds(const double aViewedPointWCS[3],
                                             const double aEyePointWCS[3],
                                             double&      aViewedLat,
                                             double&      aViewedLon,
                                             double&      aViewedAlt,
                                             bool&        aLLAValid) const
{
   if (!UtVec3d::Equals(aViewedPointWCS, aEyePointWCS))
   {
      // If the point being viewed (i.e. the target point) is not at the eyepoint,
      // make sure the target point is the same hemisphere
      // (otherwise one is looking through the Earth). This is a coarse check and assumes the
      // horizontal check does not extend far when compared to the radius of the Earth.

      double eyePointUnitVec[3];
      double viewedPointUnitVec[3];
      double eyePointMag    = UtVec3d::Normalize(eyePointUnitVec, aEyePointWCS);
      double viewedPointMag = UtVec3d::Normalize(viewedPointUnitVec, aViewedPointWCS);
      if ((eyePointMag > 0.0) && // Should always be true, but just in case...
          (viewedPointMag > 0.0))
      {
         double cosTheta = UtVec3d::DotProduct(eyePointUnitVec, viewedPointUnitVec);
         if (cosTheta <= 0.0)
         {
            return false; // target is in the opposite hemisphere.
         }
      }
   }

   if ((mMinAlt == cUNSPECIFIED) && (mMaxAlt == cUNSPECIFIED))
   {
      return true;
   }

   if (!aLLAValid)
   {
      UtEntity::ConvertWCSToLLA(aViewedPointWCS, aViewedLat, aViewedLon, aViewedAlt);
      aLLAValid = true;
   }

   return WithinAltitudeBounds(aViewedAlt);
}

// =================================================================================================
// private
bool WsfZoneDefinition::WithinRadialBounds(const double aPointInMyFrame[3],
                                           const double aViewedPointWCS[3],
                                           const double aEyePointWCS[3],
                                           double&      aViewedRadiusSquared) const
{
   if ((mMinRadiusSq == cUNSPECIFIED) && (mMaxRadiusSq == cUNSPECIFIED))
   {
      // Don't care about min or max range... return true.
      return true;
   }

   if (mShapeType == cSPHERICAL)
   {
      // Do a 3-D test for a sphere.
      // NOTE: aPointInMyFrame cannot be used for this test because the reference point was is altitude=0
      //       rather than the altitude of the eye point. Thus, the Z coordinate of the viewed point is
      //       relative to altitude 0 rather than the altitude of the eye point.
      double relLocWCS[3];
      UtVec3d::Subtract(relLocWCS, aViewedPointWCS, aEyePointWCS);
      aViewedRadiusSquared = UtVec3d::MagnitudeSquared(relLocWCS);
   }
   else
   {
      // Do a 2-D test for a vertical cylinder.
      aViewedRadiusSquared = aPointInMyFrame[0] * aPointInMyFrame[0] + aPointInMyFrame[1] * aPointInMyFrame[1];
   }

   // First line equates to "Long Enough", then anded with "Short Enough" on 2nd line...
   return (((aViewedRadiusSquared >= mMinRadiusSq) || (mMinRadiusSq == cUNSPECIFIED)) &&
           ((aViewedRadiusSquared <= mMaxRadiusSq) || (mMaxRadiusSq == cUNSPECIFIED)));
}

// =================================================================================================
// virtual
bool WsfZoneDefinition::WithinGrossXYBounds(const double aXorLatValue, const double aYorLonValue) const
{
   // This "quick and dirty" check is a time-saver,
   // but is NOT definitive...
   return (!((aXorLatValue < mMinX) || (aXorLatValue > mMaxX) || (aYorLonValue < mMinY) || (aYorLonValue > mMaxY)));
}

// =================================================================================================
bool WsfZoneDefinition::WithinPolygonSides(const double aXorLatValue, const double aYorLonValue) const
{
   // Do a quick and dirty test first to save time...
   if (!WithinGrossXYBounds(aXorLatValue, aYorLonValue))
   {
      return false;
   }

   // Reference = SUBROUTINE REGION() Check for point within a polygon.
   // Algorithm obtained from external FORTRAN Code.  Variable names
   // and logic flow are kept (approximately) the same, to facilitate
   // comparison:
   const double bndryTol = 1.0E-20; // 1.0E-10;
   const double otherTol = 1.0E-10; // 1.0E-2;

   double x = aXorLatValue;
   double y = aYorLonValue;

   double xc1 = mPoints[0].mX;
   double yc1 = mPoints[0].mY;

   double ang1 = 0.0;
   if ((fabs(yc1 - y) > otherTol) || (fabs(xc1 - x) > otherTol))
   {
      ang1 = atan2(yc1 - y, xc1 - x);
   }

   double angN1      = ang1;
   double angTot     = 0.0;
   bool   onBoundary = false;
   bool   inRegion   = false;

   double xc2;
   double yc2;
   double ang2;

   for (std::vector<Point>::size_type index = 1; (index <= mPoints.size()) && (!onBoundary); ++index)
   {
      if (index == mPoints.size())
      {
         xc2  = mPoints[0].mX;
         yc2  = mPoints[0].mY;
         ang2 = angN1;
      }
      else
      {
         xc2  = mPoints[index].mX;
         yc2  = mPoints[index].mY;
         ang2 = 0.0;
         if ((fabs(yc2 - y) > otherTol) || (fabs(xc2 - x) > otherTol))
         {
            ang2 = atan2(yc2 - y, xc2 - x);
         }
      }

      double delta = ang2 - ang1;

      if (fabs(delta) > UtMath::cPI)
      {
         delta -= 2.0 * UtMath::cPI * UtMath::Sign(delta);
      }

      angTot += delta;

      double bndry = -(y - yc1) * (xc2 - xc1) + (x - xc1) * (yc2 - yc1);
      double dist  = sqrt((xc2 - xc1) * (xc2 - xc1) + (yc2 - yc1) * (yc2 - yc1));

      if (fabs(bndry) <= (dist * bndryTol))
      {
         if ((((xc1 <= x) && (x <= xc2)) || ((xc2 <= x) && (x <= xc1))) &&
             (((yc1 <= y) && (y <= yc2)) || ((yc2 <= y) && (y <= yc1))))
         {
            onBoundary = true;
         }
      }

      ang1 = ang2;
      xc1  = xc2;
      yc1  = yc2;
   }

   if (fabs(angTot) > 1.0)
   {
      inRegion = true;
   }
   if (onBoundary)
   {
      inRegion = true;
   }

   return inRegion;
}

// =================================================================================================
bool WsfZoneDefinition::PointIsInsidePrivate(const double aViewedPointWCS[3],
                                             const double aEyePointWCS[3],
                                             const double aLookHeading,
                                             const double aDeltaDownRange) const
{
   // Adjust the eyepoint before doing any constraint checks
   const double* eyePointWCS = (mRelativeTo == cOBSERVER) ? aEyePointWCS : mReferenceWCS;

   // Check validity of eyepoint
   if (eyePointWCS[0] == cUNSPECIFIED)
   {
      return false;
   }

   // There are four general constraints to check:
   // 1.  Altitude Bounds (if any)
   //       a. Translate WCS location into LLA.
   //       b. Compare resulting Alt to constraints.
   // 2.  Radial Bounds (if any)
   // 3.  Angular Bounds (if any)
   // 4.  Geometric, or Polygonal Bounds --
   //       a. Do a quick (gross) boundary check.  If it passes:
   //       b. Do detailed specific check:
   //             i.  Geometric Shape, or
   //             ii. Within Polygon Sides.
   // (For computational efficiency, the quicker tests are done
   //  before the slower ones, and function returns as soon as
   //  any test fails.)

   double vpLat;
   double vpLon;
   double vpAlt;
   bool   LLAValid = false;

   // The altitude test does not involve any transformation
   // into a local frame, so it is relatively simple and quick:
   if (!WithinAltitudeBounds(aViewedPointWCS, eyePointWCS, vpLat, vpLon, vpAlt, LLAValid))
   {
      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Zone-point altitude check failure.";
         out.AddNote() << "Min Alt: " << mMinAlt << " m";
         out.AddNote() << "Max Alt: " << mMaxAlt << " m";
         out.AddNote() << "Point Alt: " << vpAlt << " m";
      }
      return false;
   }

   double localPos[3];
   if (mRelativeTo == cOBSERVER)
   {
      ConvertWCSToLocalFrame(aViewedPointWCS, eyePointWCS, aLookHeading, localPos);
   }
   else // if (mRelativeTo == cINTERNAL)
   {
      ConvertWCSToLocalFrame(aViewedPointWCS, eyePointWCS, mReferenceHeading, localPos);
   }

   // Now adjust for sliding Zone away from the eyepoint:
   localPos[0] += aDeltaDownRange;

   double vpRadSq;
   if (!WithinRadialBounds(localPos, aViewedPointWCS, eyePointWCS, vpRadSq))
   {
      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Zone-point radial check failure.";
         out.AddNote() << "Min Radius: " << std::sqrt(mMinRadiusSq) << " m";
         out.AddNote() << "Max Radius: " << std::sqrt(mMaxRadiusSq) << " m";
         out.AddNote() << "Point Radius: " << std::sqrt(vpRadSq) << " m";
      }

      return false;
   }

   double vpAngle;
   if (!WithinAngleBounds(localPos, vpAngle))
   {
      if (mDebugEnabled)
      {
         auto out = ut::log::debug() << "Zone-point angle check failure.";
         out.AddNote() << "Start Angle: " << mStartAngle * UtMath::cDEG_PER_RAD;
         out.AddNote() << "Stop Angle: " << mStopAngle * UtMath::cDEG_PER_RAD;
         out.AddNote() << "Point Angle: " << vpAngle * UtMath::cDEG_PER_RAD;
      }
      return false;
   }

   // Having passed the previous tests, we have
   // several miscellaneous possibilities left:

   // If the shape type was either circular or spherical,
   // and we already passed the radius test, there is no
   // further check to be made, so return true:

   if ((mShapeType == cCIRCULAR) || (mShapeType == cSPHERICAL))
   {
      return true;
   }

   // If the shape is elliptical, do a local frame test:
   if (mShapeType == cELLIPTICAL)
   {
      double k = (((localPos[0] * localPos[0]) / (mLongitudinalAxis * mLongitudinalAxis)) +
                  ((localPos[1] * localPos[1]) / (mLateralAxis * mLateralAxis)));

      // The 0.25 comes from the fact that Longitudinal and Lateral axes
      // are full values, not semi-major and semi-minor, per the standard
      // formula for an ellipse:  1 = x^2/a^2 + y^2/b^2
      return k <= 0.25;
   }

   // The only other option left is a polygonal boundary:
   if (mPoints.empty())
   {
      // No polygon specified, so we have met all the criteria that was set.
      return true;
   }

   if (mPointsAreLatLon)
   {
      if (!LLAValid)
      {
         UtEntity::ConvertWCSToLLA(aViewedPointWCS, vpLat, vpLon, vpAlt);
      }
      return WithinPolygonSides(vpLat, vpLon);
   }
   else
   {
      return WithinPolygonSides(localPos[0], localPos[1]);
   }
}

// =================================================================================================
void WsfZoneDefinition::VerticesIn(double aMinX, double aMinY, double aMaxX, double aMaxY, std::vector<Point>& aPointsVector) const
{
   const std::vector<WsfZone::Point>& pts = GetPoints();
   aPointsVector.clear();
   for (unsigned int i = 0; i != pts.size(); ++i)
   {
      if (((pts[i].mX >= aMinX) && (pts[i].mX <= aMaxX)) && ((pts[i].mY >= aMinY) && (pts[i].mY <= aMaxY)))
      {
         aPointsVector.push_back(pts[i]);
      }
   }
}

ut::optional<UtColor> WsfZoneDefinition::GetFillColor() const
{
   ut::optional<UtColor> fillColor = WsfZone::GetFillColor();
   if (!fillColor && mRefZonePtr)
   {
      fillColor = mRefZonePtr->GetFillColor();
   }

   return fillColor;
}

ut::optional<UtColor> WsfZoneDefinition::GetLineColor() const
{
   ut::optional<UtColor> lineColor = WsfZone::GetLineColor();
   if (!lineColor && mRefZonePtr)
   {
      lineColor = mRefZonePtr->GetLineColor();
   }

   return lineColor;
}
