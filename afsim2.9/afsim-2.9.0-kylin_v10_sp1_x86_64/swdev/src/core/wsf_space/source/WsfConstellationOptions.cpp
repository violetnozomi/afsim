// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfConstellationOptions.hpp"

#include <iostream>
#include <sstream>

#include "UtLog.hpp"

namespace
{

template<typename Value>
void WriteField(const char* aKeyword, const Value& aValue, std::ostream& aStream, bool aIsLast = false)
{
   aStream << "    \"" << aKeyword << "\": \"" << aValue << "\"";
   if (!aIsLast)
   {
      aStream << ",\n";
   }
}

} // namespace

//! Create a set of options for a Walker Delta constellation.
//!
//! This method will return options that correspond to a Walker Delta constellation.
//! If @p aNumTotalSats is not evenly divisible by @p aNumPlanes, or if @p aWalkerF
//! is not in the allowed range, then this will return an invalid set of options,
//! and will print out error messages indicating the nature of the failure.
//!
//! \param aNumTotalSats - The total number of members of the constellation.
//! \param aNumPlanes    - The number of orbital planes for the constellation.
//! \param aWalkerF      - The inter-plane phasing parameter for the constellation.
//! \returns - The specified constellation options, or an Invalid set of options for
//!            inputs out of their allowed range.
WsfConstellationOptions WsfConstellationOptions::CreateWalkerDelta(unsigned int aNumTotalSats,
                                                                   unsigned int aNumPlanes,
                                                                   unsigned int aWalkerF)
{
   if (ValidWalkerInputs(aNumTotalSats, aNumPlanes, aWalkerF))
   {
      return WsfConstellationOptions{ConstellationType::cWALKER_DELTA,
                                     aNumTotalSats,
                                     aNumPlanes,
                                     aNumTotalSats / aNumPlanes,
                                     aWalkerF,
                                     AnomalyAliasFromWalkerF(aWalkerF, aNumTotalSats),
                                     UtAngleValue{360.0, UtUnitAngle::cDEGREES}};
   }
   else
   {
      return WsfConstellationOptions{};
   }
}

//! Create a set of options for a Walker Star constellation.
//!
//! This method will return options that correspond to a Walker Star constellation.
//! If @p aNumTotalSats is not evenly divisible by @p aNumPlanes, or if @p aWalkerF
//! is not in the allowed range, then this will return an invalid set of options,
//! and will print out error messages indicating the nature of the failure.
//!
//! \param aNumTotalSats - The total number of members of the constellation.
//! \param aNumPlanes    - The number of orbital planes for the constellation.
//! \param aWalkerF      - The inter-plane phasing parameter for the constellation.
//! \returns - The specified constellation options, or an Invalid set of options for
//!            inputs out of their allowed range.
WsfConstellationOptions WsfConstellationOptions::CreateWalkerStar(unsigned int aNumTotalSats,
                                                                  unsigned int aNumPlanes,
                                                                  unsigned int aWalkerF)
{
   if (ValidWalkerInputs(aNumTotalSats, aNumPlanes, aWalkerF))
   {
      return WsfConstellationOptions{ConstellationType::cWALKER_STAR,
                                     aNumTotalSats,
                                     aNumPlanes,
                                     aNumTotalSats / aNumPlanes,
                                     aWalkerF,
                                     AnomalyAliasFromWalkerF(aWalkerF, aNumTotalSats),
                                     UtAngleValue{180.0, UtUnitAngle::cDEGREES}};
   }
   else
   {
      return WsfConstellationOptions{};
   }
}

//! Create a set of options for General constellation.
WsfConstellationOptions WsfConstellationOptions::CreateGeneral(unsigned int        aNumPlanes,
                                                               unsigned int        aNumSatsPerPlane,
                                                               const UtAngleValue& aAnomalyAlias,
                                                               const UtAngleValue& aRAAN_Range)
{
   return WsfConstellationOptions{ConstellationType::cGENERAL,
                                  aNumPlanes * aNumSatsPerPlane,
                                  aNumPlanes,
                                  aNumSatsPerPlane,
                                  0,
                                  aAnomalyAlias,
                                  aRAAN_Range};
}

//! Return if this object has been fully set up.
bool WsfConstellationOptions::IsSetup() const
{
   return mConstellationType != ConstellationType::cINVALID && ValidateSetup().empty();
}

//! Validate the setup of this object.
//!
//! This method examines the current state of this object and reports on
//! any deficiencies that would keep this object from being used as the options
//! to create a constellation.
//!
//! \returns - A string containing messages for any deficiencies that need to be corrected.
std::string WsfConstellationOptions::ValidateSetup() const
{
   std::stringstream retval{};
   if (mConstellationType == ConstellationType::cINVALID)
   {
      retval << "Invalid constellation type.\n";
   }
   else
   {
      if (mConstellationName.empty())
      {
         retval << "Must provide a name for the constellation.\n";
      }
      if (mPlatformType.empty())
      {
         retval << "Must provide a platform type for the constellation.\n";
      }
      if (mOrbitSize.GetSemiMajorAxis() == 0.0)
      {
         retval << "Orbit size is not defined.\n";
      }
      if (mInclination < 0.0 || mInclination.GetAsUnit(UtUnitAngle::cDEGREES) > 180.0)
      {
         retval << "Inclination must be in the range [0.0, 180.0] degrees.\n";
      }
      if (mInitialRAAN < 0.0 || mInitialRAAN.GetAsUnit(UtUnitAngle::cDEGREES) > 360.0)
      {
         retval << "Initial RAAN must be in the range [0.0, 360.0] degrees.\n";
      }
      if (mInitialAnomaly < 0.0 || mInitialAnomaly.GetAsUnit(UtUnitAngle::cDEGREES) >= 360.0)
      {
         retval << "Initial Anomaly must be in the range [0.0, 360.0) degrees.\n";
      }
   }
   return retval.str();
}

//! Write these options into a AFSIM input comment block in the provided @p aStream.
void WsfConstellationOptions::OutputOptions(std::ostream& aStream) const
{
   aStream << cHEADER_START_MARKER;
   aStream << "{\n";
   WriteField(cANOMALY_ALIAS, mAnomalyAlias, aStream);
   WriteField(cBASE_NAME, mConstellationName, aStream);
   WriteField(cINCLINATION, mInclination, aStream);
   WriteField(cINITIAL_ANOMALY, mInitialAnomaly, aStream);
   WriteField(cINITIAL_RAAN, mInitialRAAN, aStream);
   WriteField(cNUM_PLANES, mNumPlanes, aStream);
   WriteField(cGENERATION_PATH, mBasePath.GetFileName(), aStream);
   WriteField(cPLATFORM_TYPE, mPlatformType, aStream);
   WriteField(cRAAN_RANGE, mRAAN_Range, aStream);
   if (mOrbitSize.SpecifiedAsSemiMajorAxis())
   {
      WriteField(cSEMI_MAJOR_AXIS, mOrbitSize.GetSemiMajorAxis(), aStream);
   }
   else if (mOrbitSize.SpecifiedAsCircularAltitude())
   {
      WriteField(cALTITUDE, mOrbitSize.GetCircularAltitude(), aStream);
   }
   else if (mOrbitSize.SpecifiedAsOrbitalPeriod())
   {
      WriteField(cPERIOD, mOrbitSize.GetOrbitalPeriod(), aStream);
   }
   else if (mOrbitSize.SpecifiedAsRevsPerDay())
   {
      WriteField(cREVS_PER_DAY, mOrbitSize.GetRevolutionsPerDay(), aStream);
   }
   WriteField(cSATS_PER_PLANE, mSatsPerPlane, aStream, true);
   aStream << "\n}\n*/\n";
}

//! Return the name of files that would be generated for this constellation design.
std::string WsfConstellationOptions::GetGeneratedFilePrefix() const
{
   return GetBasePath().GetFileName() + "/" + GetConstellationName();
}

//! Return the name a platform would have for the given position.
std::string WsfConstellationOptions::GetMemberName(unsigned int aPlane, unsigned int aSatellite) const
{
   return GetConstellationName() + "_" + std::to_string(aPlane) + "_" + std::to_string(aSatellite);
}

//! Return the RAAN of the members of the given @p aPlane.
UtAngleValue WsfConstellationOptions::GetMemberRAAN(unsigned int aPlane) const
{
   double raan = GetInitialRAAN().GetAsUnit(UtUnitAngle::cDEGREES) +
                 GetRAAN_Range().GetAsUnit(UtUnitAngle::cDEGREES) / GetNumPlanes() * aPlane;
   raan = UtMath::NormalizeAngle0_360(raan);
   UtAngleValue retval{raan, UtUnitAngle::cDEGREES};
   return retval;
}

//! Return the true anomaly for the given @p aSatellite in the given @p aPlane.
UtAngleValue WsfConstellationOptions::GetMemberAnomaly(unsigned int aPlane, unsigned int aSatellite) const
{
   double anom = GetInitialAnomaly().GetAsUnit(UtUnitAngle::cDEGREES) + 360.0 / GetSatsPerPlane() * aSatellite +
                 GetAnomalyAlias().GetAsUnit(UtUnitAngle::cDEGREES) * aPlane;
   anom = UtMath::NormalizeAngle0_360(anom);
   UtAngleValue retval{anom, UtUnitAngle::cDEGREES};
   return retval;
}

//! Return true if the given options can form a valid Walker design.
bool WsfConstellationOptions::ValidWalkerInputs(unsigned int aNumTotalSats, unsigned int aNumPlanes, unsigned int aWalkerF)
{
   bool retval{true};
   if (aNumTotalSats % aNumPlanes != 0)
   {
      auto logger =
         ut::log::error() << "The total number of satellites must be evenly divisible by the number of planes.";
      logger.AddNote() << "Number of Satellites: " << aNumTotalSats;
      logger.AddNote() << "Number of Planes: " << aNumPlanes;
      retval = false;
   }
   if (aWalkerF >= aNumPlanes)
   {
      auto logger = ut::log::error() << "The f parameter must be less than the number of planes.";
      logger.AddNote() << "f: " << aWalkerF;
      logger.AddNote() << "Number of Planes: " << aNumPlanes;
      retval = false;
   }
   return retval;
}

//! Compute the inter-plane phasing angle from the walker inter-plane phasing parameter.
UtAngleValue WsfConstellationOptions::AnomalyAliasFromWalkerF(unsigned int aWalkerF, unsigned int aNumSatsTotal)
{
   return UtAngleValue{(360.0 / aNumSatsTotal) * aWalkerF, UtUnitAngle::cDEGREES};
}
