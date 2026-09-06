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

#include "WsfOpticalAttenuation.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <list>
#include <sstream>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtPath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfOpticalPath.hpp"
#include "WsfScenario.hpp"
#include "WsfTabularAttenuation.hpp"

// The actual internal compact attenuation tables are maintained in a separate file in order to avoid
// cluttering the code with thousands of lines of tables.
#include "WsfOpticalAttenuationTables.hpp"

namespace
{
// A helper function to compute the attenuation coefficient from the range and straight-line equation coefficients
inline double ComputeCoefficient(double aSlantRange, double aA, double aB)
{
   double alpha = aA; // Assume no dependence on range
   if (aB != 0.0)     // There is a range dependence...
   {
      double logAlpha = aA + (aB * log(aSlantRange));
      alpha           = exp(logAlpha);
   }
   return alpha;
}

// A helper function to compute the transmittance from range and straight-line equation coefficients
inline double ComputeTransmittance(double aSlantRange, double aA, double aB)
{
   double alpha = ComputeCoefficient(aSlantRange, aA, aB);
   return exp(-alpha * aSlantRange);
}

double GetCoefficient(const WsfOpticalAttenuation::Table* aTablePtr, size_t aAltIndex, double aSlantRange)
{
   const auto& segmentRefs = aTablePtr->mSegmentRefs;
   const auto& segments    = aTablePtr->mSegments;

   // Find the segment that includes the range of interest.
   size_t i = segmentRefs[aAltIndex].mIndex;
   while (aSlantRange > segments[i].mRangeLimit)
   {
      ++i;
   }
   return ComputeCoefficient(aSlantRange, segments[i].mA, segments[i].mB);
}

double RangeToAltitude(double aBegAltitude, double aElevationAngle, double aEndAltitude, double aEffectiveEarthRadius)
{
   double Re              = aEffectiveEarthRadius;
   double H1              = aBegAltitude + Re;
   double C1              = H1 * sin(aElevationAngle);
   double C2              = C1 * C1 - H1 * H1;
   double H2              = aEndAltitude + Re;
   double rangeToAltitude = -C1 + sqrt(C2 + (H2 * H2));
   return rangeToAltitude;
}
} // namespace

// =================================================================================================
WsfOpticalAttenuation::WsfOpticalAttenuation(const WsfScenario& aScenario)
   : WsfEM_Attenuation()
   , mAlgorithm(cAT_NONE)
   , mAtmosphereModel(2)
   , mHazeModel(1)
   , mCloudModel(-1)
   , mAdjustmentFactor(1.0)
   , mTablePtr(nullptr)
   , mExternalTablePtr(nullptr)
   , mSimpleAttenuation(0.0)
   , mAtmosphere(aScenario.GetAtmosphere())
{
}

// =================================================================================================
//! Create an instance that behaves like the old embedded model in WsfEOIR_Sensor and WsfIRST_Sensor.
WsfOpticalAttenuation::WsfOpticalAttenuation(const WsfScenario&           aScenario,
                                             double                       aAtmosphericAttenuation,
                                             UtAtmosphere::AtmosphereType aAtmosphereType)
   : WsfEM_Attenuation()
   , mAlgorithm(cAT_SIMPLE)
   , mAtmosphereModel(2)
   , mHazeModel(1)
   , mCloudModel(-1)
   , mAdjustmentFactor(1.0)
   , mTablePtr(nullptr)
   , mExternalTablePtr(nullptr)
   , mSimpleAttenuation(aAtmosphericAttenuation)
   , mAtmosphere(aScenario.GetAtmosphere(), aAtmosphereType)
{
}

// =================================================================================================
// virtual
WsfEM_Attenuation* WsfOpticalAttenuation::Clone() const
{
   return new WsfOpticalAttenuation(*this);
}

// =================================================================================================
namespace
{
struct ObjectFactory
{
   WsfScenario* mScenarioPtr;
   ObjectFactory(WsfScenario& aScenario) { mScenarioPtr = &aScenario; }
   WsfEM_Attenuation* operator()(const std::string& aTypeName)
   {
      WsfEM_Attenuation* instancePtr = nullptr;
      if ((aTypeName == "WSF_OPTICAL_ATTENUATION") || (aTypeName == "optical"))
      {
         instancePtr = new WsfOpticalAttenuation(*mScenarioPtr);
      }
      return instancePtr;
   }
};
} // namespace

// =================================================================================================
std::function<WsfEM_Attenuation*(const std::string&)> WsfOpticalAttenuation::GetObjectFactory(WsfScenario& aScenario)
{
   return ObjectFactory(aScenario);
}

// =================================================================================================
// virtual
bool WsfOpticalAttenuation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   bool ok = WsfEM_Attenuation::Initialize(aXmtrRcvrPtr);
   if (mAlgorithm == cAT_COMPACT)
   {
      // Everything should already be established...
   }
   else if (mAlgorithm == cAT_SIMPLE)
   {
      mExternalTablePtr = nullptr;
      mTablePtr         = nullptr;
   }
   else
   {
      mAlgorithm = cAT_COMPACT;
      ok         = SelectInternalTable(aXmtrRcvrPtr);
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfOpticalAttenuation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "atmosphere_model")
   {
      aInput.ReadValue(mAtmosphereModel);
      aInput.ValueInClosedRange(mAtmosphereModel, 1, 6);
   }
   else if (command == "haze_model")
   {
      aInput.ReadValue(mHazeModel);
      aInput.ValueInClosedRange(mHazeModel, 1, 10);
   }
   else if (command == "cloud_model") // NO_DOC - FUTURE
   {
      aInput.ReadValue(mCloudModel);
      aInput.ValueInClosedRange(mCloudModel, 0, 19);
      if (mCloudModel > 11)
      {
         aInput.ValueInClosedRange(mCloudModel, 18, 19);
      }
   }
   else if (command == "adjustment_factor")
   {
      aInput.ReadValue(mAdjustmentFactor);
      aInput.ValueGreater(mAdjustmentFactor, 0.0);
   }
   else if (command == "internal_table")
   {
      std::string tableName;
      aInput.ReadValue(tableName);
      mTablePtr = FindInternalTable(tableName);
      if (mTablePtr == nullptr)
      {
         throw UtInput::BadValue(aInput, "Unknown internal table");
      }
      mAlgorithm        = cAT_COMPACT;
      mExternalTablePtr = nullptr;
   }
   else if (command == "external_table")
   {
      std::string tableFileName;
      aInput.ReadValue(tableFileName);
      tableFileName = aInput.LocateFile(tableFileName);
      LoadExternalTable(aInput, tableFileName);
      mAlgorithm = cAT_COMPACT;
   }
   else if (command == "spectral_data_conversion")
   {
      // Convert the results of a MODTRAN run into either a file that can be read by this model or
      // on that be used as an embedded model.
      SpectralDataConversion(aInput);
   }
   else if (command == "optical_path_conversion")
   {
      // Convert an optical path coefficient type to our format.
      OpticalPathConversion(aInput);
   }
   else if (command == "query") // NO_DOC - TESTING
   {
      ProcessQueryCommand(aInput);
   }

   // Parameters for the legacy model from WSF_EOIR_SENSOR/WSF_IRST_SENSOR.
   else if (command == "atmospheric_attenuation")
   {
      std::string per;
      std::string units;
      aInput.ReadValue(mSimpleAttenuation);
      aInput.ValueGreater(mSimpleAttenuation, 0.0);
      aInput.ReadValue(per);
      aInput.StringEqual(per, "per");
      aInput.ReadValue(units);
      mSimpleAttenuation /= UtInput::ConvertValueFrom(1.0, units, UtInput::cLENGTH);
      mAlgorithm = cAT_SIMPLE;
   }
   else
   {
      myCommand = WsfEM_Attenuation::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
double WsfOpticalAttenuation::ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                                       WsfEnvironment&             aEnvironment,
                                                       WsfEM_Interaction::Geometry aGeometry)
{
   double atten = 1.0;
   switch (mAlgorithm)
   {
   case cAT_COMPACT:
   {
      double altitude;
      double elevationAngle;
      double slantRange;
      double Re = UtSphericalEarth::cEARTH_RADIUS * aInteraction.mEarthRadiusScale;
      GetRangeElevationAltitude(aInteraction, aGeometry, slantRange, elevationAngle, altitude);
      atten = ComputeCompactAttenuation(altitude, elevationAngle, slantRange, Re, mTablePtr);
   }
   break;
   case cAT_SIMPLE:
   {
      double altitude1;
      double altitude2;
      double groundRange;
      GetAltitudesAndGroundRange(aInteraction, aGeometry, altitude1, altitude2, groundRange);
      atten = ComputeSimpleAttenuation(altitude1, altitude2, groundRange);
   }
   break;
   default:
      break;
   }
   return (atten * mAdjustmentFactor);
}

// =================================================================================================
// private
double WsfOpticalAttenuation::ComputeSimpleAttenuation(double aAltitude1, double aAltitude2, double aGroundRange)
{
   double attenuationPerMeter = mSimpleAttenuation;

   // Determine the simplistic transmittance along a path.
   //
   // attenuationPerMeter The attenuation per meter at sea level.
   // aAltitude1           The altitude MSL of the first  object (meters)
   // aAltitude2           The altitude MSL of the second object (meters)
   // aGroundRange         The ground range between the objects (meters)

   // Sort the heights so that the first point is lower than the second point.

   double zMin = std::max(std::min(aAltitude1, aAltitude2), 0.0);
   double zMax = std::max(std::max(aAltitude1, aAltitude2), 0.0);

   // Determine the approximate elevation angle of the path (assumes flat Earth).

   double dz       = zMax - zMin;
   double dg       = aGroundRange;
   double ds       = sqrt(dg * dg + dz * dz);
   double sinTheta = 0.0;
   if (ds > 1.0)
   {
      sinTheta = dz / ds;
   }

   // Integrate through the layers.

   double              rhoSeaLevel   = mAtmosphere.Density(0);
   static const double cDELTA_Z      = 1000.0;
   double              z2            = zMin;
   double              rho2          = mAtmosphere.Density(z2);
   double              transmittance = 1.0;
   bool                done          = false;
   while (!done)
   {
      // Calculate the altitude at the top of the current layer and get the density.

      double z1   = z2;
      double rho1 = rho2;
      z2          = z1 + cDELTA_Z;
      if (z2 >= zMax)
      {
         z2   = zMax;
         done = true;
      }
      rho2 = mAtmosphere.Density(z2);

      // Calculate the path length in the current layer.

      dz = z2 - z1;
      ds = aGroundRange; // Assume horizontal path...
      if (sinTheta != 0.0)
      {
         ds = dz / sinTheta;
      }

      // Update the transmittance based on the rate of extinction in the current layer.

      double rhoAvg = 0.5 * (rho1 + rho2);
      double c      = attenuationPerMeter * (rhoAvg / rhoSeaLevel);
      transmittance *= exp(-c * ds);
   }
   return transmittance;
}

// =================================================================================================
// private
double WsfOpticalAttenuation::ComputeCompactAttenuation(double       aAltitude,
                                                        double       aElevationAngle,
                                                        double       aSlantRange,
                                                        double       aEffectiveEarthRadius,
                                                        const Table* aTablePtr)
{
   // Process the trivial case.
   if (aSlantRange < 1.0)
   {
      return 1.0;
   }
   double beginAltitude = std::max(aAltitude, 0.0);
   double slantRange    = aSlantRange;

   double Re = aEffectiveEarthRadius;
   // The length of the observers position vector. Negative MSL values are clamped to 0 (essentially shift the path up).
   double H1 = beginAltitude + Re;
   // The angle between the sides formed by the observers position vector and the line-of-sight vector.
   double theta = aElevationAngle + UtMath::cPI_OVER_2;

   // Use the law of cosines to compute the final altitude.
   double H2_sq = (H1 * H1) + (aSlantRange * aSlantRange) - (2.0 * H1 * aSlantRange * cos(theta));
   // I don't think this should ever be negative, but just in case...
   H2_sq = std::max(H2_sq, 0.0);

   double endAltitude = sqrt(H2_sq) - Re;
   if (endAltitude < 1.0)
   {
      endAltitude = 1.0;
      double H2   = endAltitude + Re;

      // Recompute the slant range for the adjusted altitude.
      //
      // H2^2 = H1^2 + R^2 - 2*H1*R*cos(theta) (law of cosines)
      // R^2 - 2*H1*cos(theta)*R + (H1^2 - H2^2) (quadratic equation in R)
      double a    = 1.0;
      double b    = -2.0 * H1 * cos(theta);
      double c    = (H1 * H1) - (H2 * H2);
      double temp = (b * b) - (4.0 * a * c);
      temp        = sqrt(std::max(temp, 0.0));
      // The answer is the smallest positive value
      double root1 = (-b + temp) / (2.0 * a);
      double root2 = (-b - temp) / (2.0 * a);
      if (root1 <= 0.0)
      {
         slantRange = root2;
      }
      else if (root2 <= 0.0)
      {
         slantRange = root1;
      }
      else
      {
         slantRange = std::min(root1, root2);
      }
   }

   // The 'variable alpha' (alpha means attenuation coefficient) relies on needed to know the total range in the
   // atmosphere (where MODTRAN defines the top-of-atmosphere to be 100 km). For paths that are purely in the
   // atmosphere this is just the total slant range, but for paths that start or end in space the part in space
   // should be omitted. Furthermore, for very long paths the range should be truncated.
   //
   // Note that this does NOT change the range over which attenuation is calculated. It simply supplies a different
   // range involved in the calculation of 'variable alpha'.

   static const double cALTITUDE_TOA = 100000.0;   // The nominal altitude of the top-of-atmosphere
   double              airSlantRange = slantRange; // assume path is purely endatmospheric.

   double sumAlphaR = 0.0; // sum(alpha * R) through the layers
   if (aElevationAngle < 0.0)
   {
      // Path is initially downward (descending through altitude layers). Note that a downward path can become an
      // upward path if the path is long enough or the angle is shallow enough. In such a case the location at which
      // the transition occurs is computed and the downward portion and upward portions are computed.

      // First determine the point the range and altitude along the line-of-sight where path would change from an
      // descending to an ascending path. If the supplied range is beyond that point then the path must be processed
      // in two segments - one that goes from that point back to the starting point and one that goes from that point
      // forward to the end point.
      //
      // Note that this point occurs where the path becomes tangent to the Earth's surface.
      double sinTheta         = sin(theta);
      double H2               = H1 * sinTheta;
      double switchAltitude   = H2 - Re;
      double switchSlantRange = sqrt((H1 * H1) - (H2 * H2));

      // Limit the range processed as the descending segment to the minimum of the input value
      // and the range at which the ascending segment begins.
      if (slantRange <= switchSlantRange)
      {
         switchSlantRange = slantRange;
         switchAltitude   = endAltitude;
      }
      else if (switchAltitude < 1.0)
      {
         // The altitude at which the ascending segment starts is actually below ground. This can lead to A LOT of
         // attenuation on paths that graze the surface. Because the input geometries probably don't use spherical
         // coordinates, it is possible that negative values may be calculated when in fact it isn't real. In these
         // cases we will just set the altitude to 1, which effectively shifts the path upwards.
         switchAltitude = 1.0;
      }

      // If the switch altitude is above the top of the atmosphere then there is no attenuation
      if (switchAltitude >= cALTITUDE_TOA)
      {
         return 1.0;
      }

      // The descending segment is processed starting at the switch point and going back to the starting point.
      // First we have to compute the elevation angle from the end of the descending segment back to the starting point.
      H2              = switchAltitude + Re;
      double cosAlpha = ((H1 * H1) - (H2 * H2) - (switchSlantRange * switchSlantRange)) / (-2.0 * H2 * switchSlantRange);
      cosAlpha        = UtMath::Limit(cosAlpha, 1.0);
      double elevationAngle = acos(cosAlpha) - UtMath::cPI_OVER_2;

      // Top-of-Atmosphere on the reverse of the descending path...
      if (slantRange <= switchSlantRange)
      {
         //  Only a descending segment. Process it in reverse (so it is ascending)
         if (aAltitude >= cALTITUDE_TOA) // End point is above atmosphere
         {
            double rangeToTOA = RangeToAltitude(switchAltitude, elevationAngle, cALTITUDE_TOA, Re);
            airSlantRange     = std::min(switchSlantRange, rangeToTOA);
         }
         ComputePathAttenuation(sumAlphaR, switchAltitude, elevationAngle, switchSlantRange, aAltitude, airSlantRange, Re, aTablePtr);
      }
      else
      {
         // Process the descending segment in reverse (so it is ascending)
         if ((aAltitude >= cALTITUDE_TOA) || (endAltitude >= cALTITUDE_TOA))
         {
            // One or both endpoints is in space. Adjust path length in atmosphere.
            double rangeToTOA       = RangeToAltitude(switchAltitude, elevationAngle, cALTITUDE_TOA, Re);
            double slantRangeInAir1 = std::min(switchSlantRange, rangeToTOA);
            rangeToTOA              = RangeToAltitude(switchAltitude, 0.0, cALTITUDE_TOA, Re);
            double slantRangeInAir2 = std::min(slantRange - switchSlantRange, rangeToTOA);
            airSlantRange           = slantRangeInAir1 + slantRangeInAir2;
         }
         ComputePathAttenuation(sumAlphaR, switchAltitude, elevationAngle, switchSlantRange, aAltitude, airSlantRange, Re, aTablePtr);
         // Process the ascending segment. Note that the elevation angle is always zero because the path is tangent
         // to the Earth's surface at the starting point of the segment.
         slantRange -= switchSlantRange; // Subtract of what's already been processed
         ComputePathAttenuation(sumAlphaR, switchAltitude, 0.0, slantRange, endAltitude, airSlantRange, Re, aTablePtr);
      }
   }
   else
   {
      // Process a path that is ONLY ascending.
      if (beginAltitude >= cALTITUDE_TOA)
      {
         return 1.0; // path is completely above the atmosphere.
      }
      if (endAltitude >= cALTITUDE_TOA)
      {
         // end point is above the top-of-atmosphere so limit the 'slant range in air' to where space is entered.
         double rangeToTOA = RangeToAltitude(beginAltitude, aElevationAngle, cALTITUDE_TOA, Re);
         airSlantRange     = std::min(slantRange, rangeToTOA);
      }
      ComputePathAttenuation(sumAlphaR, beginAltitude, aElevationAngle, slantRange, endAltitude, airSlantRange, Re, aTablePtr);
   }

   double transmittance = exp(-sumAlphaR);
   return transmittance;
}

// =================================================================================================
//! Computes the exponent for transmittance for the ascending path.
//! @note This routine assumes the path is ascending (elevation angle >= 0). The caller is responsible
//!       for calling this routine with a reverse path if the path is descending.
// private
void WsfOpticalAttenuation::ComputePathAttenuation(double&      aSumAlphaR,
                                                   double       aBegAltitude,
                                                   double       aElevationAngle,
                                                   double       aSlantRange,
                                                   double       aEndAltitude,
                                                   double       aAirSlantRange,
                                                   double       aEffectiveEarthRadius,
                                                   const Table* aTablePtr)
{
   double      sumAlphaR   = aSumAlphaR;
   const auto& segmentRefs = aTablePtr->mSegmentRefs;
   auto        out         = ut::log::debug();
   if (DebugEnabled())
   {
      out << "ComputePathAttenuation:";
      out.AddNote() << "Altitude: " << aBegAltitude;
      out.AddNote() << "Elevation Angle: " << aElevationAngle * UtMath::cDEG_PER_RAD << " deg";
      out.AddNote() << "Slant Range: " << aSlantRange;
      out.AddNote() << "End Altitude: " << aEndAltitude;
      out.AddNote() << "Air Slant Range: " << aAirSlantRange;
   }

   // Find the bounding indices of the interval such that the starting altitude is lowerAlt <= altitude < upperAlt.
   // If the altitude is below the lowest altitude then the first interval will be returned. If is greater than
   // or equal to the highest altitude then the last interval will be returned.

   size_t lowerIndex = 0;
   size_t upperIndex = 1;
   while ((segmentRefs[upperIndex].mAltitude >= 0.0) && (aBegAltitude >= segmentRefs[upperIndex].mAltitude))
   {
      lowerIndex = upperIndex;
      ++upperIndex;
   }
   if (DebugEnabled())
   {
      auto startNote = out.AddNote() << "Start:";
      startNote.AddNote() << "Index: (" << lowerIndex << ", " << upperIndex << ")";
      startNote.AddNote() << "Alt: (" << segmentRefs[lowerIndex].mAltitude << ", " << segmentRefs[upperIndex].mAltitude
                          << ")";
   }

   // Calculate the constants to be used in solving for the slant range where a particular altitude is crossed.
   double Re              = aEffectiveEarthRadius;
   double H1              = aBegAltitude + Re;
   double C1              = H1 * sin(aElevationAngle);
   double C2              = C1 * C1 - H1 * H1;
   double lowerSlantRange = 0.0;
   double upperSlantRange = 0.0;

   // If the line of sight starts below the bottom of the table, propagate to the bottom of the table.
   //
   // The attenuation values for the lowest table entry are used for the portion of the path that is below the table.

   if (aBegAltitude < segmentRefs[0].mAltitude)
   {
      double H2              = segmentRefs[0].mAltitude + Re;
      upperSlantRange        = -C1 + sqrt(C2 + (H2 * H2)); // Range to first altitude in table
      double layerSlantRange = std::min(upperSlantRange, aSlantRange) - lowerSlantRange;
      double alpha           = GetCoefficient(aTablePtr, 0, aAirSlantRange);
      sumAlphaR += (alpha * layerSlantRange);
      if (DebugEnabled())
      {
         auto InitPointBelowTable = out.AddNote() << "Initial point below table:";
         InitPointBelowTable.AddNote() << "Index: " << lowerIndex;
         InitPointBelowTable.AddNote() << "Alt: " << segmentRefs[0].mAltitude;
         InitPointBelowTable.AddNote() << "R: (" << lowerSlantRange << "," << upperSlantRange << ")";
         InitPointBelowTable.AddNote() << "r: " << layerSlantRange;
         InitPointBelowTable.AddNote() << "Alpha: " << alpha;
         InitPointBelowTable.AddNote() << "T: " << exp(-sumAlphaR);
      }
      lowerSlantRange = upperSlantRange; // Set up for entry into main loop
   }

   // Iterate through the layers
   while ((segmentRefs[upperIndex].mAltitude >= 0.0) && (upperSlantRange < aSlantRange))
   {
      // Solve to get the slant range to the point when the LOS vector crosses the upper altitude,
      // and compute the slant range within the layer.
      double H2              = segmentRefs[upperIndex].mAltitude + Re;
      upperSlantRange        = -C1 + sqrt(C2 + (H2 * H2));
      double layerSlantRange = std::min(upperSlantRange, aSlantRange) - lowerSlantRange;

      // Get the attenuation coefficient at the layer boundaries.
      //
      // For constant alpha segments (no range dependence), it does not matter what range is passed.
      // For variable alpha one wants to pass the TOTAL range and NOT just the length in the layer.
      // This seems weird, but it really does work. Trust me...
      double lowerAlpha = GetCoefficient(aTablePtr, lowerIndex, aAirSlantRange);
      double upperAlpha = GetCoefficient(aTablePtr, upperIndex, aAirSlantRange);

      // Determine the interpolation factors for the segment
      double f0 = 0.0;
      if (aBegAltitude > segmentRefs[lowerIndex].mAltitude)
      {
         f0 = (aBegAltitude - segmentRefs[lowerIndex].mAltitude) /
              (segmentRefs[upperIndex].mAltitude - segmentRefs[lowerIndex].mAltitude);
      }
      double f1 = 1.0;
      if (aEndAltitude < segmentRefs[upperIndex].mAltitude)
      {
         f1 = (aEndAltitude - segmentRefs[lowerIndex].mAltitude) /
              (segmentRefs[upperIndex].mAltitude - segmentRefs[lowerIndex].mAltitude);
      }

      // The interpolated alpha's, a0 and a1 correspond the alpha's at the start and end of the
      // path within the layer. The effective alpha for the layer will be the average of these two.
      // This simplifies as follows:
      //
      // a0 = lowerAlpha + f0 * (upperAlpha - lowerAlpha);
      // a1 = lowerAlpha + f1 * (upperAlpha - lowerAlpha);
      // alpha = 0.5 * (a0 + a1)
      //       = 0.5 * ((lowerAlpha + (f0 * (upperAlpha - lowerAlpha))) + (lowerAlpha + (f1 * (upperAlpha -
      //       lowerAlpha)))) = 0.5 * ((2.0 * lowerAlpha) + ((f0 + f1) * (upperAlpha - lowerAlpha)))
      double alpha = 0.5 * ((2.0 * lowerAlpha) + ((f0 + f1) * (upperAlpha - lowerAlpha)));

      sumAlphaR += (alpha * layerSlantRange);
      if (DebugEnabled())
      {
         auto logNoteIndex = out.AddNote() << "Index (" << lowerIndex << "," << upperIndex << "):";
         logNoteIndex.AddNote() << "Alt: [" << segmentRefs[lowerIndex].mAltitude << ", "
                                << segmentRefs[upperIndex].mAltitude << ")";
         logNoteIndex.AddNote() << "R: (" << lowerSlantRange << "," << upperSlantRange << ")";
         logNoteIndex.AddNote() << "f0: " << f0;
         logNoteIndex.AddNote() << "f1: " << f1;
         logNoteIndex.AddNote() << "r: " << layerSlantRange;
         logNoteIndex.AddNote() << "Alpha: " << alpha;
         logNoteIndex.AddNote() << "T: " << exp(-alpha * layerSlantRange);
         logNoteIndex.AddNote() << "cum: " << exp(-sumAlphaR);
      }
      lowerIndex      = upperIndex;
      lowerSlantRange = upperSlantRange;
      ++upperIndex;
   }

   // Any part of the path that extends above the table is simply ignored as it is assumed the attenuation
   // is trivial beyond that point (the standard scripts will generate data to 90000 meters).
   //
   // For debug purposes we will compute and display...
   if (DebugEnabled() && (upperSlantRange < aSlantRange))
   {
      // NOTE: At this point lowerIndex refers to the last valid table entry and lowerSlantRange
      //       is the accumulated range to that point.
      double layerSlantRange   = aSlantRange - lowerSlantRange;
      double alpha             = GetCoefficient(aTablePtr, lowerIndex, aAirSlantRange);
      auto   logNoteFinalPoint = out.AddNote() << "Final point above table:";
      logNoteFinalPoint.AddNote() << "Index: " << lowerIndex;
      logNoteFinalPoint.AddNote() << "Alt: " << segmentRefs[lowerIndex].mAltitude;
      logNoteFinalPoint.AddNote() << "R: (" << lowerSlantRange << "," << aSlantRange << ")";
      logNoteFinalPoint.AddNote() << "r: " << layerSlantRange;
      logNoteFinalPoint.AddNote() << "Alpha: " << alpha;
      logNoteFinalPoint.AddNote() << "T: " << exp(-(sumAlphaR + (alpha * layerSlantRange)));
   }
   aSumAlphaR = sumAlphaR;
}

// =================================================================================================
// private
const WsfOpticalAttenuationTables::Table* WsfOpticalAttenuation::FindInternalTable(const std::string& aTableName) const
{
   const Table* tablePtr = nullptr;
   size_t       i        = 0;
   while (WsfOpticalAttenuationTables::sTableRefs[i].mTablePtr != nullptr)
   {
      if (aTableName == WsfOpticalAttenuationTables::sTableRefs[i].mTableNamePtr)
      {
         tablePtr = WsfOpticalAttenuationTables::sTableRefs[i].mTablePtr;
         break;
      }
      ++i;
   }
   return tablePtr;
}

// =================================================================================================
// private
bool WsfOpticalAttenuation::SelectInternalTable(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   double wavelength = aXmtrRcvrPtr->GetWavelength();

   // Compute the bandwidth in units of length...
   double bandwidth       = aXmtrRcvrPtr->GetBandwidth(); // in Hz, not length...
   double frequency       = aXmtrRcvrPtr->GetFrequency();
   double lowerFrequency  = frequency - 0.5 * bandwidth;
   double upperFrequency  = frequency + 0.5 * bandwidth;
   double lowerWavelength = UtMath::cLIGHT_SPEED / upperFrequency;
   double upperWavelength = UtMath::cLIGHT_SPEED / lowerFrequency;
   double bandwidth_nm    = (upperWavelength - lowerWavelength) * 1.0E+9;

   std::string baseName;
   if (bandwidth_nm < 10.0) // Monochromatic (i.e.: laser)
   {
      int                wavelength_nm = static_cast<int>((wavelength * 1.0E+9) + 0.01);
      std::ostringstream oss;
      oss << "mono_" << wavelength_nm << "nm";
      baseName = oss.str();
   }
   else if ((wavelength >= 3.0E-6) && (wavelength <= 5.0E-6))
   {
      baseName = "mwir";
   }
   else if ((wavelength >= 8.0E-6) && (wavelength <= 12.0E-6))
   {
      baseName = "lwir";
   }
   else if ((wavelength >= 380.0E-12) && (wavelength <= 760.0E-12))
   {
      baseName = "visual";
   }
   else
   {
      return false;
   }

   std::string tableName(baseName);
   {
      std::ostringstream oss;
      oss << "_a" << mAtmosphereModel << "_h" << mHazeModel;
      tableName = tableName + oss.str();
   }

   if (mCloudModel > 0)
   {
      std::ostringstream oss;
      oss << "_c" << mCloudModel;
      tableName = tableName + oss.str();
   }

   mTablePtr = FindInternalTable(tableName);
   if (mTablePtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find attenuation table.";
      out.AddNote() << "Table Name: " << tableName;
   }
   return (mTablePtr != nullptr);
}

// =================================================================================================
// private
void WsfOpticalAttenuation::ProcessQueryCommand(UtInput& aInput)
{
   UtInputBlock   inputBlock(aInput);
   UtTable::Curve sensorResponseCurve; // Sensor response
   std::string    command;
   std::string    spectralDataFileName;
   double         altitude             = -1.0;
   double         elevationAngle       = -1.0;
   double         slantRange           = -1.0;
   double         wavelength           = -1.0;
   int            atmosphereModel      = 2;
   int            hazeModel            = 1;
   double         effectiveEarthRadius = UtSphericalEarth::cEARTH_RADIUS;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "altitude") // NO_DOC - TESTING
      {
         aInput.ReadValueOfType(altitude, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(altitude, 0.0);
      }
      else if (command == "elevation_angle") // NO_DOC - TESTING
      {
         aInput.ReadValueOfType(elevationAngle, UtInput::cANGLE);
         aInput.ValueInClosedRange(elevationAngle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
      }
      else if (command == "slant_range") // NO_DOC - TESTING
      {
         aInput.ReadValueOfType(slantRange, UtInput::cLENGTH);
         aInput.ValueGreaterOrEqual(slantRange, 0.0);
      }
      else if (command == "wavelength") // NO_DOC - TESTING
      {
         aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
         aInput.ValueGreater(wavelength, 0.0);
      }
      else if (command == "atmosphere_model") // NO_DOC - TESTING
      {
         aInput.ReadValue(atmosphereModel);
         aInput.ValueInClosedRange(atmosphereModel, 1, 6);
      }
      else if (command == "haze_model") // NO_DOC - TESTING
      {
         aInput.ReadValue(hazeModel);
         aInput.ValueInClosedRange(hazeModel, 1, 10);
      }
      else if (command == "earth_radius_multiplier") // NO_DOC - TESTING
      {
         double earthRadiusMultiplier;
         aInput.ReadValue(earthRadiusMultiplier);
         aInput.ValueGreater(earthRadiusMultiplier, 0.0);
         effectiveEarthRadius = UtSphericalEarth::cEARTH_RADIUS * earthRadiusMultiplier;
      }
      else if (command == "spectral_data") // NO_DOC - TESTING
      {
         aInput.ReadValue(spectralDataFileName);
         spectralDataFileName = aInput.LocateFile(spectralDataFileName);
      }
      else if (command == "sensor_response_curve") // NO_DOC - TESTING
      {
         sensorResponseCurve.ProcessInput(inputBlock.GetInput(),
                                          UtInput::cLENGTH,
                                          "",
                                          UtTable::ValueGT(0.0),
                                          UtInput::cNON_DIMENSIONAL,
                                          "",
                                          UtTable::ValueGE_LE(0.0, 1.0));
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   double expectedTransmittance = -1.0;

   // If a spectral data file was given then read and integrate the results...
   if (!spectralDataFileName.empty())
   {
      std::ifstream dataFile(spectralDataFileName.c_str());
      if (!dataFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + spectralDataFileName);
      }

      std::vector<double> wavenumbers;
      std::vector<double> transmittances;
      std::vector<double> sensorResponseVector;

      // First three lines are identification lines
      std::string headers[3];
      WsfTabularAttenuation::ReadSpectralHeaders(dataFile, headers);

      while (true)
      {
         double tempAltitude;
         double tempElevation;
         double tempRange;
         bool   eof =
            WsfTabularAttenuation::ReadSpectralData(dataFile, tempAltitude, tempElevation, tempRange, wavenumbers, transmittances);
         if (eof)
         {
            break;
         }

         if (expectedTransmittance >= 0.0)
         {
            throw UtInput::BadValue(aInput, "Spectral Data Error: file can contain only one case");
         }

         WsfTabularAttenuation::BuildResponseVector(sensorResponseCurve, wavenumbers, sensorResponseVector);
         expectedTransmittance =
            WsfTabularAttenuation::ComputeAverageTransmittance(wavenumbers, transmittances, sensorResponseVector);
         altitude       = tempAltitude;
         elevationAngle = tempElevation * UtMath::cRAD_PER_DEG;
         slantRange     = tempRange;
      }
   }

   if ((altitude < 0.0) || (slantRange <= 0.0))
   {
      throw UtInput::BadValue(aInput, "geometry conditions not specified");
   }

   double transmittance    = 0.0;
   double op_transmittance = -1.0;
   switch (mAlgorithm)
   {
   case cAT_COMPACT:
      transmittance = ComputeCompactAttenuation(altitude, elevationAngle, slantRange, effectiveEarthRadius, mTablePtr);
      if (wavelength > 0.0)
      {
         WsfOpticalPath opticalPath(WsfOpticalPathCoefficientTypes::Get(WsfScenario::FromInput(aInput)),
                                    wavelength,
                                    atmosphereModel,
                                    hazeModel);
         double         targetAlt = altitude + slantRange * sin(elevationAngle);
         op_transmittance         = opticalPath.GetAttenuation(slantRange, targetAlt, altitude);
      }
      break;
   case cAT_SIMPLE:
      break;
   default:
      throw UtInput::BadValue(aInput, "An atmosphere has not been configured");
   }

   auto out = ut::log::info() << "Processed query command:";
   out.AddNote() << "Altitude: " << altitude;
   out.AddNote() << "Elevation: " << elevationAngle * UtMath::cDEG_PER_RAD << " deg";
   out.AddNote() << "Range:" << slantRange;
   out.AddNote() << "Transmittance: " << transmittance;
   double error = 0.0;
   if (expectedTransmittance > 0.0)
   {
      error = fabs(transmittance - expectedTransmittance);
      out.AddNote() << "Expected: " << expectedTransmittance;
      out.AddNote() << "Error:" << error;
   }
   if (op_transmittance >= 0.0)
   {
      out.AddNote() << "op_transmittance:" << op_transmittance;
   }
   if (error > 0.001)
   {
      auto fail = ut::log::info("test_fail") << "-FAIL- Transmittance test:";
      fail.AddNote() << "Expected: " << expectedTransmittance;
      fail.AddNote() << "Actual: " << transmittance;
   }
}

// =================================================================================================
//! Loads a compact table from a file created by SpectralDataConvert.
void WsfOpticalAttenuation::LoadExternalTable(UtInput& aInput, const std::string& aTableFileName)
{
   mExternalTablePtr = nullptr;
   mTablePtr         = nullptr;

   std::vector<SegmentRef> segmentRefs;
   std::vector<Segment>    segments;

   UtInput input;
   input.PushInput(ut::make_unique<UtInputFile>(aTableFileName));

   std::string command;
   input.ReadCommand(command);
   if (command != "compact_table")
   {
      throw UtInput::UnknownCommand(input);
   }

   UtInputBlock inputBlock(input);

   float lastAltitude = -1;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "altitude")
      {
         SegmentRef segmentRef(0.0, 0);
         inputBlock.GetInput().ReadValueOfType(segmentRef.mAltitude, UtInput::cLENGTH);
         inputBlock.GetInput().ValueGreaterOrEqual(segmentRef.mAltitude, 0.0F);
         inputBlock.GetInput().ValueGreater(segmentRef.mAltitude, lastAltitude);
         lastAltitude      = segmentRef.mAltitude;
         segmentRef.mIndex = (unsigned int)segments.size();
         segmentRefs.push_back(segmentRef);

         UtInputBlock altitudeBlock(inputBlock.GetInput());
         float        lastRange = -1.0;
         while (altitudeBlock.ReadCommand(command))
         {
            if (command == "range_limit")
            {
               Segment segment;
               altitudeBlock.GetInput().ReadValueOfType(segment.mRangeLimit, UtInput::cLENGTH);
               altitudeBlock.GetInput().ValueGreater(segment.mRangeLimit, 0.0F);
               altitudeBlock.GetInput().ValueGreater(segment.mRangeLimit, lastRange);
               altitudeBlock.GetInput().ReadValue(segment.mA);
               altitudeBlock.GetInput().ReadValue(segment.mB);
               segments.push_back(segment);
            }
            else
            {
               throw UtInput::UnknownCommand(altitudeBlock.GetInput());
            }
         }
         if (segments.size() == segmentRef.mIndex)
         {
            throw UtInput::BadValue(inputBlock.GetInput(), "Empty 'altitude' block");
         }
         // Force the last segment to extend to infinity
         segments.back().mRangeLimit = std::numeric_limits<float>::max();
      }
      else
      {
         throw UtInput::UnknownCommand(inputBlock.GetInput());
      }
   }
   if (segmentRefs.empty())
   {
      throw UtInput::BadValue(aInput, "Empty table");
   }

   // Build the compact table
   segmentRefs.emplace_back(-1.0F, static_cast<unsigned int>(segments.size()));

   auto segmentRefsPtr = ut::make_unique<SegmentRef[]>(segmentRefs.size());
   std::copy(std::begin(segmentRefs), std::end(segmentRefs), segmentRefsPtr.get());
   auto segmentsPtr = ut::make_unique<Segment[]>(segments.size());
   std::copy(std::begin(segments), std::end(segments), segmentsPtr.get());


   mExternalTablePtr = std::make_shared<TableX>(segmentRefsPtr.release(), segmentsPtr.release());
   mTablePtr         = mExternalTablePtr.get();
}

// =================================================================================================
//! Convert the atmospheric coefficients used by WsfOpticalPath to the form used by this model.
void WsfOpticalAttenuation::OpticalPathConversion(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);
   double       wavelength      = 0.0;
   unsigned int atmosphereModel = 2;
   unsigned int hazeModel       = 1;
   std::string  codeFileName;
   std::string  tableFileName;
   std::string  command;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "wavelength")
      {
         aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
         aInput.ValueGreater(wavelength, 0.0);
      }
      else if (command == "atmosphere_model")
      {
         aInput.ReadValue(atmosphereModel);
         aInput.ValueInClosedRange(atmosphereModel, 1U, 6U);
      }
      else if (command == "haze_model")
      {
         aInput.ReadValue(hazeModel);
         aInput.ValueInClosedRange(hazeModel, 1U, 10U);
      }
      else if (command == "code_output")
      {
         aInput.ReadValue(codeFileName);
      }
      else if (command == "table_output")
      {
         aInput.ReadValue(tableFileName);
      }
      else
      {
         throw UtInput::UnknownCommand(inputBlock.GetInput());
      }
   }
   if ((wavelength <= 0.0) || (atmosphereModel < 1) || (hazeModel < 1))
   {
      throw UtInput::BadValue(aInput, "wavelength, atmosphere_model and haze_model must be specified");
   }

   if (codeFileName.empty() && tableFileName.empty())
   {
      throw UtInput::BadValue(aInput, "No output specifications provided");
   }

   WsfOpticalPathCoefficientTypes&          types(WsfOpticalPathCoefficientTypes::Get(WsfScenario::FromInput(aInput)));
   const WsfOpticalPath::Coefficients&      baseCoefs = types.GetCoefficients(wavelength, atmosphereModel, hazeModel);
   const WsfOpticalPath::LaserCoefficients& laserCoefs = dynamic_cast<const WsfOpticalPath::LaserCoefficients&>(baseCoefs);

   { // RAII block
      auto out = ut::log::info() << "Beginning conversion of optical path coefficients.";
      out.AddNote() << "wavelength: " << wavelength * 1.0E-9 << " nanometers";
      out.AddNote() << "atmosphere_model: " << atmosphereModel;
      out.AddNote() << "haze_model: " << hazeModel;
   }

   std::string tableName;
   {
      std::ostringstream oss;
      int                wavelength_nm = static_cast<int>(wavelength * 1.0e+9);
      oss << "opc_" << wavelength_nm << "nm_a" << atmosphereModel << "_h" << hazeModel;
      tableName = oss.str();
   }

   std::ofstream codeFile;
   if (!codeFileName.empty())
   {
      codeFile.open(codeFileName.c_str());
      if (!codeFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + codeFileName);
      }
      auto out = ut::log::info() << "Code output:";
      out.AddNote() << "File: " << codeFileName;
   }
   std::ofstream tableFile;
   if (!tableFileName.empty())
   {
      tableFile.open(tableFileName.c_str());
      if (!tableFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + tableFileName);
      }
      auto out = ut::log::info() << "Table output:";
      out.AddNote() << "File: " << tableFileName;
   }

   if (codeFile.is_open())
   {
      codeFile << "   namespace " << tableName << "\n";
      codeFile << "   {\n";
      codeFile << "      const Segment sSegments[] = {\n";
   }

   if (tableFile.is_open())
   {
      tableFile << "compact_table\n";
   }

   std::ios::fmtflags oldFlags;
   std::streamsize    oldPrecision;
   std::string        separator  = " ";
   size_t             pointCount = laserCoefs.mAltitude.size();
   for (size_t i = 0; i < pointCount; ++i)
   {
      double altitude = laserCoefs.mAltitude[i];
      double alpha    = laserCoefs.mAbs[i] + laserCoefs.mScat[i];
      double tauBeg   = exp(-alpha * 100.0);
      double tauEnd   = exp(-alpha * 300000.0);

      if (codeFile.is_open())
      {
         oldFlags     = codeFile.flags();
         oldPrecision = codeFile.precision();
         codeFile << "         " << separator << "Segment(";
         codeFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
         codeFile << "FLT_MAX";
         codeFile.setf(std::ios_base::scientific, std::ios_base::floatfield);
         codeFile << ", " << std::setprecision(8) << alpha << 'F';
         codeFile << ", 0.0F";
         codeFile << ")";
         // codeFile.setf(ios_base::fixed, ios_base::floatfield);
         // codeFile << "   // Alt=" << setprecision(0) << altitude;
         // codeFile << " T=" << setprecision(6) << tauBeg;
         // codeFile << "->" << setprecision(6) << tauEnd;
         codeFile << "\n";
         codeFile.flags(oldFlags);
         codeFile.precision(oldPrecision);
      }
      if (tableFile.is_open())
      {
         tableFile << "   altitude " << altitude << " meters\n";
         oldFlags     = tableFile.flags();
         oldPrecision = tableFile.precision();
         tableFile << "      range_limit 10000000 meters";
         tableFile.setf(std::ios_base::scientific, std::ios_base::floatfield);
         tableFile << " " << std::setprecision(8) << alpha;
         tableFile << " 0.0";
         tableFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
         tableFile << "  // T=" << std::setprecision(6) << tauBeg;
         tableFile << "->" << std::setprecision(6) << tauEnd;
         tableFile << "\n";
         tableFile.flags(oldFlags);
         tableFile.precision(oldPrecision);
         tableFile << "   end_altitude\n";
      }
      separator = ",";
   }

   // Finish up the output...

   if (codeFile.is_open())
   {
      codeFile << "      };\n"; // Finshes the table of segments
      codeFile << "\n";
      codeFile << "      const SegmentRef sSegmentRefs[] = {\n";
      codeFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
      for (size_t i = 0; i < pointCount; ++i)
      {
         codeFile << "         SegmentRef(" << std::setw(8) << std::setprecision(1) << laserCoefs.mAltitude[i] << "F, "
                  << i << "),\n";
      }
      codeFile << "         SegmentRef(    -1.0F, " << pointCount << ")\n"; // Terminating entry in the table
      codeFile << "      };\n";                                             // Finishes the table of altitude pointers.
      codeFile << "\n";
      codeFile << "      const Table sTable(sSegmentRefs, sSegments);\n";
      codeFile << "   }\n"; // Closes 'namespace' for this table.
      codeFile.close();
   }

   if (tableFile.is_open())
   {
      tableFile << "end_compact_table\n";
      tableFile.close();
      // Load the file for a possible subsequent 'query' command. This also validates the table was created properly.
      LoadExternalTable(aInput, tableFileName);
      mAlgorithm = cAT_COMPACT;
   }
   ut::log::info() << "Conversion complete.";
}

// =================================================================================================
//! Convert MODTRAN raw transmittance data to the condensed form used by this model.
// private
void WsfOpticalAttenuation::SpectralDataConversion(UtInput& aInput)
{
   UtInputBlock   inputBlock(aInput);
   UtTable::Curve sensorResponseCurve; // Sensor response
   std::string    command;
   std::string    spectralDataFileName;
   std::string    codeFileName;
   std::string    plotFileName;
   std::string    tableFileName;
   int            maximumSegmentCount  = 5;
   double         maximumAbsoluteError = 0.0001;
   double         maximumRelativeError = 0.005;
   while (inputBlock.ReadCommand(command))
   {
      if (command == "spectral_data")
      {
         aInput.ReadValue(spectralDataFileName);
         spectralDataFileName = aInput.LocateFile(spectralDataFileName);
      }
      else if (command == "sensor_response_curve")
      {
         sensorResponseCurve.ProcessInput(inputBlock.GetInput(),
                                          UtInput::cLENGTH,
                                          "",
                                          UtTable::ValueGT(0.0),
                                          UtInput::cNON_DIMENSIONAL,
                                          "",
                                          UtTable::ValueGE_LE(0.0, 1.0));
      }
      else if (command == "code_output")
      {
         aInput.ReadValue(codeFileName);
      }
      else if (command == "table_output")
      {
         aInput.ReadValue(tableFileName);
      }
      else if (command == "plot_output")
      {
         aInput.ReadValue(plotFileName);
      }
      else if (command == "maximum_segment_count")
      {
         aInput.ReadValue(maximumSegmentCount);
         aInput.ValueGreater(maximumSegmentCount, 0);
      }
      else if (command == "maximum_absolute_error")
      {
         aInput.ReadValue(maximumAbsoluteError);
         aInput.ValueGreater(maximumAbsoluteError, 0.0);
      }
      else if (command == "maximum_relative_error")
      {
         aInput.ReadValue(maximumRelativeError);
         aInput.ValueGreater(maximumRelativeError, 0.0);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (codeFileName.empty() && tableFileName.empty() && plotFileName.empty())
   {
      throw UtInput::BadValue(aInput, "No output specifications provided");
   }

   if (spectralDataFileName.empty())
   {
      throw UtInput::BadValue(aInput, "spectral_data must be provided");
   }
   std::ifstream cptFile;
   cptFile.open(spectralDataFileName.c_str());
   if (!cptFile.is_open())
   {
      throw UtInput::BadValue(aInput, "Unable to open " + spectralDataFileName);
   }

   { // RAII block
      auto out = ut::log::info() << "Beginning conversion.";
      out.AddNote() << "File: " << spectralDataFileName;
   }

   std::ofstream codeFile;
   if (!codeFileName.empty())
   {
      codeFile.open(codeFileName.c_str());
      if (!codeFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + codeFileName);
      }
      auto out = ut::log::info() << "Code output:";
      out.AddNote() << "File: " << codeFileName;
   }
   std::ofstream tableFile;
   if (!tableFileName.empty())
   {
      tableFile.open(tableFileName.c_str());
      if (!tableFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + tableFileName);
      }
      auto out = ut::log::info() << "Table output:";
      out.AddNote() << "File: " << tableFileName;
   }
   std::ofstream plotFile;
   if (!plotFileName.empty())
   {
      plotFile.open(plotFileName.c_str());
      if (!plotFile.is_open())
      {
         throw UtInput::BadValue(aInput, "Unable to open " + plotFileName);
      }
      auto out = ut::log::info() << "Plot output:";
      out.AddNote() << "File: " << plotFileName;
   }

   // First three lines are identification lines
   std::string headers[3];
   WsfTabularAttenuation::ReadSpectralHeaders(cptFile, headers);

   codeFile << "\n";
   for (size_t i = 0; i < 3; ++i)
   {
      if (tableFile.is_open())
      {
         tableFile << "# " << headers[i] << "\n";
      }
      if (plotFile.is_open())
      {
         plotFile << "# " << headers[i] << "\n";
      }
      if (!headers[i].empty())
      {
         if (codeFile.is_open())
         {
            codeFile << "   // " << headers[i] << "\n";
         }
      }
   }

   if (codeFile.is_open())
   {
      UtPath      cptPathName(spectralDataFileName);
      std::string tableName(cptPathName.GetFileName());
      size_t      tempLen = tableName.length();
      if ((tempLen > 8) && (tableName.substr(tempLen - 8) == "_cpt.plt"))
      {
         tableName = tableName.substr(0, tempLen - 8);
      }
      codeFile << "   namespace " << tableName << "\n";
      codeFile << "   {\n";
      codeFile << "      const Segment sSegments[] = {\n";
   }

   if (tableFile.is_open())
   {
      tableFile << "compact_table\n";
   }

   std::vector<double>     cptWavenumbers;
   std::vector<double>     cptTransmittances;
   std::vector<double>     altitudeValues;
   std::vector<double>     rangeValues;
   std::vector<double>     transmittanceValues;
   std::vector<double>     sensorResponseVector;
   std::vector<SegmentRef> segmentRefs;
   std::vector<Segment>    segments;
   bool                    monochromatic = false;

   double currentAltitude = -1.0;
   double currentRange    = -1.0;

   while (true)
   {
      double altitude;
      double elevationAngle;
      double range;
      bool   cptEOF =
         WsfTabularAttenuation::ReadSpectralData(cptFile, altitude, elevationAngle, range, cptWavenumbers, cptTransmittances);
      if (cptEOF)
      {
         break;
      }
      if (elevationAngle != 0.0)
      {
         throw UtInput::BadValue(aInput, "Spectral Data Error: All elevationAngle values must be zero");
      }
      if (altitude < currentAltitude)
      {
         throw UtInput::BadValue(aInput, "Spectral Data Error: Altitudes must ascend");
      }

      monochromatic = (cptWavenumbers.size() <= 2);
      if (altitude > currentAltitude)
      {
         // Finish the previous altitude
         if (!rangeValues.empty())
         {
            ProcessAltitude(codeFile,
                            tableFile,
                            plotFile,
                            currentAltitude,
                            rangeValues,
                            transmittanceValues,
                            monochromatic,
                            segmentRefs,
                            segments,
                            maximumAbsoluteError,
                            maximumRelativeError,
                            maximumSegmentCount);
         }
         altitudeValues.push_back(altitude);
         currentAltitude = altitude;
         currentRange    = -1.0;
         rangeValues.clear();
         transmittanceValues.clear();
      }
      if (range <= currentRange)
      {
         throw UtInput::BadValue(aInput, "Spectral Data Error: Ranges must ascend monotonically");
      }
      currentRange = range;

      // Build the sensor response as a function of wavenumber.
      if (sensorResponseVector.empty())
      {
         WsfTabularAttenuation::BuildResponseVector(sensorResponseCurve, cptWavenumbers, sensorResponseVector);
      }
      double transmittance =
         WsfTabularAttenuation::ComputeAverageTransmittance(cptWavenumbers, cptTransmittances, sensorResponseVector);
      rangeValues.push_back(currentRange);
      transmittanceValues.push_back(transmittance);
   }
   if (!rangeValues.empty())
   {
      ProcessAltitude(codeFile,
                      tableFile,
                      plotFile,
                      currentAltitude,
                      rangeValues,
                      transmittanceValues,
                      monochromatic,
                      segmentRefs,
                      segments,
                      maximumAbsoluteError,
                      maximumRelativeError,
                      maximumSegmentCount);
   }

   if (plotFile.is_open())
   {
      plotFile.close();
   }

   if (codeFile.is_open())
   {
      codeFile << "      };\n"; // Finshes the table of segments
      codeFile << "\n";
      codeFile << "      const SegmentRef sSegmentRefs[] = {\n";
      codeFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
      for (auto& segmentRef : segmentRefs)
      {
         codeFile << "         SegmentRef(" << std::setw(10) << std::setprecision(1) << segmentRef.mAltitude << "F, "
                  << segmentRef.mIndex << "),\n";
      }
      codeFile << "         SegmentRef(      -1.0F, " << segments.size() << ")\n"; // Terminating entry in the table
      codeFile << "      };\n"; // Finishes the table of altitude pointers.
      codeFile << "\n";
      codeFile << "      const Table sTable(sSegmentRefs, sSegments);\n";
      codeFile << "   }\n"; // Closes 'namespace' for this table.
      codeFile.close();
   }

   if (tableFile.is_open())
   {
      tableFile << "end_compact_table\n";
      tableFile.close();
      // Load the file for a possible subsequent 'test' command. This also validates the table was created properly.
      LoadExternalTable(aInput, tableFileName);
      mAlgorithm = cAT_COMPACT;
   }
   ut::log::info() << "Conversion complete.";
}

// =================================================================================================
// private
void WsfOpticalAttenuation::ProcessAltitude(std::ofstream&             aCodeFile,
                                            std::ofstream&             aTableFile,
                                            std::ofstream&             aPlotFile,
                                            double                     aAltitude,
                                            const std::vector<double>& aRangeValues,
                                            const std::vector<double>& aTransmittanceValues,
                                            bool                       aMonochromatic,
                                            std::vector<SegmentRef>&   aSegmentRefs,
                                            std::vector<Segment>&      aSegments,
                                            double                     aMaximumAbsoluteError,
                                            double                     aMaximumRelativeError,
                                            int                        aMaximumSegmentCount) const
{
   { // RAII block
      auto out = ut::log::info() << "WsfOpticalAttenuation::ProcessAltitude():";
      out.AddNote() << "Altitude: " << aAltitude;
   }

   // NOTE: 'alpha' refers to the 'attenuation coefficient' and 'tau' is the 'transmittance'.

   // Compute the x and y values to be used in the curve fit.
   size_t              pointCount = aRangeValues.size();
   std::vector<double> alphaValues(pointCount);
   // std::vector<double> rangeValues(pointCount);
   std::vector<double> lnAlphaValues(pointCount);
   std::vector<double> lnRangeValues(pointCount);

   // Attempting to use transmittance values to recover alpha can lead to problems when the value of
   // the transmittance is too small. MODTRAN reports transmittance to 8 decimal places (1.0E-8).
   // When the transmittance is very small the unknown digits past that point can greatly affectly the
   // computation of alpha. For instance, when the transmittance is reported to be 1.0E-8, it really could
   // be 1.4999...E-8 or 0.5000...E-8 (compilers typically round output before printing it). This leads to
   // an uncertainty of +/- 5 in the third digit of alpha. Transmittance values of 1.0E-7 have an uncertainty
   // of +/-5 in the fourth digit of alpha, and similarly values of 1.0E-6 lead to uncertainty of +/- 5 in
   // in the fifth digit.
   //
   // It ends up that we really want to have five good digits in alpha to lead to the determination of the
   // constant alpha or curve fit that produces an an acceptable error. Therefore we will accept reject all
   // transmittance values below 1E-5.
   //
   // Raw transmittance values less than the following value are considered to imprecise.

   double minimumTransmittanceAllowed = 1.0E-6;

   double lastGoodAlpha    = 0.0; // The last known alpha of a suitable precision
   bool   useLastGoodAlpha = false;
   for (size_t i = 0; i < pointCount; ++i)
   {
      double range = aRangeValues[i];
      // rangeValues[i] = range;
      lnRangeValues[i] = log(range);
      double tau       = aTransmittanceValues[i];

      // MODTRAN may report a transmittance of zero or one, which really shouldn't happen. MODTRAN reports the
      // values to 8 places, so the answer probably had some non-zero components in the 9 places and beyond.
      tau          = UtMath::Limit(tau, 0.000000004, 0.999999996);
      double alpha = -(log(tau) / range);

      // As long as the reported transmittance is non-zero, capture the computed alpha. This becomes the alpha
      // once the transmittance is reported as zero. This is necessary because once it becomes zero we really
      // don't know the actual transmittance because it only has 8 digits.

      if (useLastGoodAlpha)
      {
         // Signal was previously extinguished, so continue with the same alpha.
         alpha = lastGoodAlpha;
      }
      else if (aTransmittanceValues[i] < minimumTransmittanceAllowed)
      {
         // Transmittance has too much potential for error to be useful. Use the last good computed of alpha.
         if (lastGoodAlpha == 0.0)
         {
            // This probably shouldn't occur as it means the first sample was inadequate.
            // Simply use the currently computed alpha for this and subsequent points.
            lastGoodAlpha = alpha;
         }
         alpha            = lastGoodAlpha;
         useLastGoodAlpha = true;
      }
      else
      {
         // Capture this alpha as the one to be used if the transmittance subsequently becomes zero.
         lastGoodAlpha = alpha;
      }
      alphaValues[i]   = alpha;
      lnAlphaValues[i] = log(alpha);
      // cout << "Alt=" << aAltitude
      //      << " R=" << aRangeValues[i] << " tau=" << aTransmittanceValues[i]
      //      << " alpha=" << alphaValues[i] << " ln(alpha)=" << lnAlphaValues[i] << endl;
   }

   // If monochromatic light first try to generate segments with a constant alpha.
   // If this generates fewer than 4 segments then use it.
   std::vector<Segment> segments;
   if (aMonochromatic)
   {
      GenerateSegments(aRangeValues,
                       aTransmittanceValues,
                       alphaValues,
                       lnRangeValues,
                       lnAlphaValues,
                       aMaximumAbsoluteError,
                       aMaximumRelativeError,
                       aMaximumSegmentCount,
                       true,
                       segments);
      if (segments.size() > 4) // Not good enough
      {
         segments.clear();
      }
   }

   // If not monochromatic light or if constant alpha segments could not be generated for monochromatic light,
   // generate the segments allowing variable alpha.
   if (segments.empty())
   {
      GenerateSegments(aRangeValues,
                       aTransmittanceValues,
                       alphaValues,
                       lnRangeValues,
                       lnAlphaValues,
                       aMaximumAbsoluteError,
                       aMaximumRelativeError,
                       aMaximumSegmentCount,
                       false,
                       segments);
   }

   // Dump the resulting segments to the output files.

   aSegmentRefs.emplace_back(static_cast<float>(aAltitude), static_cast<unsigned int>(aSegments.size()));
   if (aTableFile.is_open())
   {
      aTableFile << "   altitude " << aAltitude << " meters\n";
   }

   std::ios::fmtflags oldFlags;
   std::streamsize    oldPrecision;
   size_t             endIndex = 0;
   for (auto& segment : segments)
   {
      double rangeLimit = segment.mRangeLimit; // negative if last segment
      double a          = segment.mA;
      double b          = segment.mB;

      double realRangeLimit = (rangeLimit >= 0.0) ? rangeLimit : FLT_MAX;

      // Compute the maximum absolute error in the segment
      double maxAbsError = 0.0;
      double maxRelError = 0.0;
      size_t begIndex    = endIndex; // This segment starts where the last one ended.
      while ((endIndex < pointCount) && (aRangeValues[endIndex] <= realRangeLimit))
      {
         double range  = aRangeValues[endIndex];
         double tau    = aTransmittanceValues[endIndex];
         double tauMod = ComputeTransmittance(range, a, b);
         if (aPlotFile.is_open())
         {
            oldFlags     = aPlotFile.flags();
            oldPrecision = aPlotFile.precision();
            // clang-format off
            aPlotFile << std::setprecision(0) << aAltitude
                      << ' ' << std::setprecision(0) << range
                      << ' ' << std::setprecision(7) << tau
                      << ' ' << std::setprecision(7) << tauMod
                      << ' ' << std::setprecision(7) << tau - tauMod
                      << ' ' << std::setprecision(2) << 100.0 * ((tau - tauMod) / tau)
                      << std::endl;
            // clang-format on
            aPlotFile.flags(oldFlags);
            aPlotFile.precision(oldPrecision);
         }
         double absError = fabs(tau - tauMod);
         maxAbsError     = std::max(maxAbsError, absError);
         double relError = absError / std::max(tau, 1.0E-8);
         maxRelError     = std::max(maxAbsError, relError);
         ++endIndex;
      }
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "Maximum absolute error in segment computation:";
         out.AddNote() << "PASS [" << begIndex << ", " << endIndex - 1 << "]";
         out.AddNote() << "Range: " << aRangeValues[endIndex - 1];
         out.AddNote() << "Max Absolute Error: " << maxAbsError;
         out.AddNote() << "Max Relative Error: " << maxRelError;
      }

      std::string separator = ",";
      if (aSegments.empty())
      {
         separator = " ";
      }
      aSegments.push_back(segment);

      double tauBeg = ComputeTransmittance(aRangeValues[begIndex], a, b);
      double tauEnd = ComputeTransmittance(aRangeValues[endIndex - 1], a, b);
      if (aCodeFile.is_open())
      {
         oldFlags     = aCodeFile.flags();
         oldPrecision = aCodeFile.precision();
         aCodeFile << "         " << separator << "Segment(";
         aCodeFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
         if (rangeLimit >= 0.0F)
         {
            aCodeFile << std::setw(10) << std::setprecision(1) << rangeLimit << 'F';
         }
         else
         {
            aCodeFile << "    FLT_MAX"; // Last segment
         }
         aCodeFile.setf(std::ios_base::scientific, std::ios_base::floatfield);
         aCodeFile << ", " << std::setprecision(8) << a << 'F';
         aCodeFile << ", " << std::setprecision(8) << b << 'F';
         aCodeFile << ")";
         aCodeFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
         // aCodeFile << "   // Alt=" << setprecision(0) << aAltitude;
         // aCodeFile << " T=" << setprecision(6) << tauBeg;
         // aCodeFile << "->" << setprecision(6) << tauEnd;
         // aCodeFile << " +/- " << maxAbsError;
         aCodeFile << "\n";
         aCodeFile.flags(oldFlags);
         aCodeFile.precision(oldPrecision);
      }
      if (aTableFile.is_open())
      {
         oldFlags     = aTableFile.flags();
         oldPrecision = aTableFile.precision();
         if (rangeLimit >= 0.0)
         {
            aTableFile << "      range_limit " << std::setw(8) << std::setprecision(0) << rangeLimit << " meters";
         }
         else
         {
            aTableFile << "      range_limit 10000000 meters"; // Last segment
         }
         aTableFile.setf(std::ios_base::scientific, std::ios_base::floatfield);
         aTableFile << " " << std::setprecision(8) << a;
         aTableFile << " " << std::setprecision(8) << b;
         aTableFile.setf(std::ios_base::fixed, std::ios_base::floatfield);
         aTableFile << "  // T=" << std::setprecision(6) << tauBeg;
         aTableFile << "->" << std::setprecision(6) << tauEnd;
         aTableFile << " +/- " << maxAbsError;
         aTableFile << "\n";
         aTableFile.flags(oldFlags);
         aTableFile.precision(oldPrecision);
      }
   }

   // Close the declaration for this altitude
   if (aTableFile.is_open())
   {
      aTableFile << "   end_altitude\n";
   }
   if (aPlotFile.is_open())
   {
      aPlotFile << "\n";
   }
}

// =================================================================================================
void WsfOpticalAttenuation::GenerateSegments(const std::vector<double>& aRangeValues,
                                             const std::vector<double>& aTransmittanceValues,
                                             const std::vector<double>& aAlphaValues,
                                             const std::vector<double>& aLnRangeValues,
                                             const std::vector<double>& aLnAlphaValues,
                                             double                     aMaximumAbsoluteError,
                                             double                     aMaximumRelativeError,
                                             int                        aMaximumSegmentCount,
                                             bool                       aOnlyUseConstantAlpha,
                                             std::vector<Segment>&      aSegments) const
{
   auto   out          = ut::log::debug() << "WsfOpticalAttenuation::GenerateSegments() Debug Report:";
   size_t pointCount   = aRangeValues.size();
   size_t begIndex     = 0;
   size_t endIndex     = pointCount;
   int    segmentCount = 0;
   while (begIndex < pointCount)
   {
      double a;
      double b;
      double siga;
      double sigb;
      double chisq;

      // First try to see if constant alpha will work using the alpha vs. range linear fit.
      // If the fit is close to a straight line then constant alpha is a possibility.
      size_t errIndex = 0;
      StraightLineFit(aRangeValues, aAlphaValues, begIndex, endIndex - begIndex, a, b, siga, sigb, chisq);
      if (DebugEnabled())
      {
         auto note = out.AddNote() << "Straight line fit(CON-FIT):";
         note.AddNote() << "A: " << a;
         note.AddNote() << "B: " << b;
         note.AddNote() << "Sig A: " << siga;
         note.AddNote() << "Sig B: " << sigb;
         note.AddNote() << "Chi Sq: " << chisq;
      }
      if (fabs(b) < 1.0E-6)
      {
         // Close to zero slope. Compute the average alpha using the estimated value at each of the sample ranges.
         double alphaSum = 0.0;
         for (size_t i = begIndex; i < endIndex; ++i)
         {
            double alpha = a + (b * aRangeValues[i]);
            alphaSum += alpha;
         }
         double constantAlpha = alphaSum / (endIndex - begIndex);

         // Assuming an alpha equal to the average value, check to see if it is acceptable.
         a        = constantAlpha;
         b        = 0.0;
         errIndex = TestStraightLineFit(aRangeValues,
                                        aTransmittanceValues,
                                        begIndex,
                                        endIndex,
                                        a,
                                        b,
                                        aMaximumAbsoluteError,
                                        aMaximumRelativeError);
      }
      if ((!aOnlyUseConstantAlpha) && (errIndex < endIndex))
      {
         // Constant alpha was not good enough. Try with variable alpha.
         StraightLineFit(aLnRangeValues, aLnAlphaValues, begIndex, endIndex - begIndex, a, b, siga, sigb, chisq);
         if (DebugEnabled())
         {
            auto note = out.AddNote() << "Straight line fit(VAR-FIT):";
            note.AddNote() << "A: " << a;
            note.AddNote() << "B: " << b;
            note.AddNote() << "Sig A: " << siga;
            note.AddNote() << "Sig B: " << sigb;
            note.AddNote() << "Chi Sq: " << chisq;
         }
         errIndex = TestStraightLineFit(aRangeValues,
                                        aTransmittanceValues,
                                        begIndex,
                                        endIndex,
                                        a,
                                        b,
                                        aMaximumAbsoluteError,
                                        aMaximumRelativeError);
      }

      if ((segmentCount + 1) >= aMaximumSegmentCount)
      {
         // The segment attempting to be constructed will cause the segment count to reach the
         // allowed limit. Therefore it is accepted regardless of the maximum error encountered.
         endIndex = pointCount;
      }
      else if (errIndex < endIndex)
      {
         // A range was found where the computed transmittance differed from the actual transmittance
         // by more than allowed. A couple of things may be done here:
         //
         // 1) Shorten the segment and try again.
         // 2) If the error location was close to the end of the list then simply ignore the error
         //    a consider the remainder to be part of this segment. (See notes below about this).

         if (DebugEnabled())
         {
            auto note = out.AddNote() << "Transmittance value differs from the actual value by more than allowed.";
            note.AddNote() << "FAIL [" << begIndex << ", " << endIndex - 1 << "]:";
            note.AddNote() << "Index: " << errIndex;
            note.AddNote() << "Range: " << aRangeValues[errIndex];
         }

         // The segment must be accepted if the length has been reduced to 2 points
         bool forcedToAccept = ((endIndex - begIndex) == 2);

         if ((errIndex + 10) > pointCount) // Error location is close to the end of the list
         {
            endIndex       = pointCount; // Ignore the error and accept the tail end
            forcedToAccept = true;
            if (DebugEnabled())
            {
               out.AddNote() << "Error ignored because is close to the end of the list.";
            }
         }
         if (!forcedToAccept)
         {
            endIndex = endIndex - 1;
            continue; // Try again with a shorter segment
         }
      }

      // A good segment has been found, or we've been forced to accept the current segment. If the end of
      // the segment is close to the end of the list such that the next segment would be trivial, accept
      // the remainder as part of the current segment.
      //
      // NOTE: accepting the tail will result in larger errors in the tail, but that is typically
      //       not a problem because the tail represents longer ranges and:
      //
      //       a) At lower altitudes the signal is almost totally attenuated.
      //       b) At higher altitudes there is very little attenuation.

      if ((endIndex < pointCount) &&      // End of current segment is not the end of the list ....
          ((endIndex + 10) > pointCount)) // ... but it is close to the end of the list
      {
         endIndex = pointCount;
         if (DebugEnabled())
         {
            out.AddNote() << "End point is close to end of list. Moving to end of list.";
         }
      }

      // Set the range limit in the last segment to a negative value as a flag for caller.
      double rangeLimit = aRangeValues[endIndex - 1];
      if (endIndex >= pointCount)
      {
         rangeLimit = -rangeLimit;
      }
      aSegments.emplace_back(static_cast<float>(rangeLimit), static_cast<float>(a), static_cast<float>(b));
      ++segmentCount;

      // Finished processing this segment.
      // Advance to the next segment, which starts with the end point of this segment.

      if (endIndex >= pointCount) // Current segment is the last segment
      {
         begIndex = pointCount; // Forces termination of the loop
      }
      else if ((endIndex - begIndex) > 1) // Current segment was non-trivial
      {
         begIndex = endIndex - 1; // Next segment starts with the endpoint of the current segment ...
      }
      endIndex = pointCount;
   }
}
// =================================================================================================
// Test to see if a given straight line fit is acceptable.
size_t WsfOpticalAttenuation::TestStraightLineFit(const std::vector<double>& aRangeValues,
                                                  const std::vector<double>& aTransmittanceValues,
                                                  size_t                     aBeg,
                                                  size_t                     aEnd,
                                                  double                     aA,
                                                  double                     aB,
                                                  double                     aMaximumAbsoluteError,
                                                  double                     aMaximumRelativeError) const
{
   // Find the first computed transmittance that differs from the its actual value by more than the
   // allowed amount. If no such value was found then the current segment is complete.

   size_t endIndex = aEnd; // Assume the fit is good
   for (size_t i = aBeg; i < aEnd; ++i)
   {
      double range    = aRangeValues[i];
      double tau      = aTransmittanceValues[i];
      double tauMod   = ComputeTransmittance(range, aA, aB);
      double absError = fabs(tau - tauMod);
      double relError = absError / std::max(tau, 1.0E-8);
      if ((absError > aMaximumAbsoluteError) && (relError > aMaximumRelativeError))
      {
         endIndex = i;
         if (DebugEnabled())
         {
            auto out =
               ut::log::debug() << "Transmittance differs from its actual value by more than the allowed amount.";
            out.AddNote() << "FAIL [" << aBeg << ", " << aEnd - 1 << "]:";
            out.AddNote() << "Index: " << endIndex;
            out.AddNote() << "Range: " << range;
            out.AddNote() << "Absolute Error: " << absError;
            out.AddNote() << "Relative Error: " << relError;
            out.AddNote() << "A: " << aA;
            out.AddNote() << "B: " << aB;
         }
         break;
      }
   }
   return endIndex;
}

// =================================================================================================
// Straight Line fit of a set of data points to the line y = a + bx.
// private
void WsfOpticalAttenuation::StraightLineFit(const std::vector<double>& aX,
                                            const std::vector<double>& aY,
                                            size_t                     aStart,
                                            size_t                     aCount,
                                            double&                    aA,
                                            double&                    aB,
                                            double&                    aSigA,
                                            double&                    aSigB,
                                            double&                    aChi2) const
{
   if (aCount == 1)
   {
      aA    = aY[0];
      aB    = 0.0;
      aSigA = 0.0;
      aSigB = 0.0;
      aChi2 = 0.0;
      return;
   }

   size_t begIndex = aStart;
   size_t endIndex = aStart + aCount;

   double sx = 0.0;
   double sy = 0.0;
   size_t ss = 0;
   for (size_t i = begIndex; i < endIndex; ++i)
   {
      sx += aX[i];
      sy += aY[i];
      ++ss;
   }
   double sxoss = sx / ss;
   double st2   = 0.0;
   double a     = 0.0;
   double b     = 0.0;
   for (size_t i = begIndex; i < endIndex; ++i)
   {
      double t = aX[i] - sxoss;
      st2 += t * t;
      b += t * aY[i];
   }
   b /= st2;
   a           = (sy - sx * b) / ss;
   double siga = sqrt((1.0 + (sx * sx) / (ss * st2)) / ss);
   double sigb = sqrt(1.0 / st2);
   double chi2 = 0.0;
   for (size_t i = begIndex; i < endIndex; ++i)
   {
      double t = aY[i] - a - (b * aX[i]);
      chi2 += (t * t);
   }
   double sigdat = 1.0;
   if (endIndex - begIndex > 2)
   {
      sigdat = sqrt(chi2 / (endIndex - begIndex - 2));
   }
   siga *= sigdat;
   sigb *= sigdat;
   aA    = a;
   aB    = b;
   aSigA = siga;
   aSigB = sigb;
   aChi2 = chi2;
}
