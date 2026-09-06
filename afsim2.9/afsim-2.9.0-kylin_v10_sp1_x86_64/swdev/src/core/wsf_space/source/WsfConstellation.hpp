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

#ifndef WSFCONSTELLATION_HPP
#define WSFCONSTELLATION_HPP

#include <string>

#include "UtUnitTypes.hpp"

class WsfPlatform;

#include "WsfConstellationOptions.hpp"
class WsfConstellationManager;

class WsfConstellation
{
public:
   WsfConstellation(const WsfConstellationManager& aManager, const WsfConstellationOptions& aOptions);

   //! Return if this constellation was created with a general design.
   bool IsGeneral() const { return mOptions.IsGeneral(); }

   //! Return if this constellation was created with a Walker Delta design.
   bool IsWalkerDelta() const { return mOptions.IsWalkerDelta(); }

   //! Return if this constellation was created with a Walker Star design.
   bool IsWalkerStar() const { return mOptions.IsWalkerStar(); }

   //! Return the total number of members in the design of this constellation.
   //!
   //! This returns the number of members in the design of this constellation,
   //! not the current number of member platforms that exist in the simulation.
   //!
   //! \return - The number of members in the design of this constellation.
   unsigned int GetTotalSats() const { return mOptions.GetTotalSats(); }

   //! Return the number of orbital planes in the design of this constellation.
   unsigned int GetNumPlanes() const { return mOptions.GetNumPlanes(); }

   //! Return the number of members per orbital plane in the design of this constellation.
   unsigned int GetSatsPerPlane() const { return mOptions.GetSatsPerPlane(); }

   //! Return the Walker inter-plane phasing parameter.
   //!
   //! The return value is only meaningful when this constellation has a
   //! Walker Delta or Walker Star design. Constellations with a general
   //! design will always return 0 in this method.
   //!
   //! \return - The Walker inter-plane phasing parameter, or 0 for general
   //!           constellations.
   unsigned int GetWalkerF() const { return mOptions.GetWalkerF(); }

   //! Return the inter-plane phasing angle for the design of this constellation.
   UtAngleValue GetAnomalyAlias() const { return mOptions.GetAnomalyAlias(); }

   //! Return the range in RAAN for the orbital planes for this constellation.
   UtAngleValue GetRAAN_Range() const { return mOptions.GetRAAN_Range(); }

   //! Return the constellation's name.
   std::string GetName() const { return mOptions.GetConstellationName(); }

   //! Return the platform type of the members of this constellation.
   std::string GetPlatformType() const { return mOptions.GetPlatformType(); }

   //! Return the semi-major axis of the orbits in this constellation.
   UtLengthValue GetSemiMajorAxis() const { return mOptions.GetSemiMajorAxis(); }

   //! Return the circular altitude of the orbits in this constellation.
   UtLengthValue GetCircularAltitude() const { return mOptions.GetCircularAltitude(); }

   //! Return the orbital period of the orbits in this constellation.
   UtTimeValue GetOrbitalPeriod() const { return mOptions.GetOrbitalPeriod(); }

   //! Return the number of revolutions per day of the orbits in this constellation.
   double GetRevolutionsPerDay() const { return mOptions.GetRevolutionsPerDay(); }

   //! Return the inclination of the orbits in this constellation.
   UtAngleValue GetInclination() const { return mOptions.GetInclination(); }

   //! Return the RAAN of the zeroth orbital plane for this constellation.
   UtAngleValue GetInitialRAAN() const { return mOptions.GetInitialRAAN(); }

   //! Return the initial true anomaly of the zeroth member of the zeroth plane in this constellation.
   UtAngleValue GetInitialAnomaly() const { return mOptions.GetInitialAnomaly(); }

   std::string  GetMemberName(unsigned int aPlane, unsigned int aMember) const;
   WsfPlatform* GetMember(unsigned int aPlane, unsigned int aMember) const;

private:
   WsfConstellationOptions        mOptions;
   const WsfConstellationManager& mManager;
};

#endif // WSFCONSTELLATION_HPP
