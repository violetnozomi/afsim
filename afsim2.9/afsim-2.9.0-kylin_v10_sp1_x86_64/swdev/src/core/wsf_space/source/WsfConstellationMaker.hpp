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

#ifndef WSFCONSTELLATIONMAKER_HPP
#define WSFCONSTELLATIONMAKER_HPP

#include <memory>

class UtCalendar;
#include "UtPath.hpp"
class UtScript;
#include "UtUnitTypes.hpp"

class WsfConstellation;
#include "WsfConstellationOptions.hpp"
class WsfPlatform;
class WsfScriptContext;
class WsfSimulation;

//! This object can be used to create constellations.
//!
//! This object is used to create WsfConstellation objects, or to write an input
//! file that when included in a scenario would create a constellation. There
//! are three broad possibilities for the design of the constellation: a General
//! a Walker Delta, or a Walker Star. Because a few parameters of the design have
//! some interrelationships, this object is only constructed through static methods
//! that allow for those relationships to hold. The basic parameters of created
//! instances of this object are immutable. The remaining parameters of the
//! constellation design are set with member methods. Once the constellation design
//! is completed, this object is then used to actually create the constellation in
//! one of the supported forms.
class WsfConstellationMaker
{
public:
   static std::unique_ptr<WsfConstellationMaker> CreateWalkerDelta(unsigned int aNumTotalSats,
                                                                   unsigned int aNumPlanes,
                                                                   unsigned int aWalkerF);
   static std::unique_ptr<WsfConstellationMaker> CreateWalkerStar(unsigned int aNumTotalSats,
                                                                  unsigned int aNumPlanes,
                                                                  unsigned int aWalkerF);
   static std::unique_ptr<WsfConstellationMaker> CreateGeneral(unsigned int aNumPlanes,
                                                               unsigned int aNumSatsPerPlane,
                                                               UtAngleValue aAnomalyAlias,
                                                               UtAngleValue aRAAN_Range);

   void              WriteToFile() const;
   WsfConstellation* Create(double            aCreationTime,
                            WsfSimulation&    aSimulation,
                            WsfScriptContext& aContext,
                            UtScript*         aFilterScriptPtr,
                            UtScript*         aSetupScriptPtr) const;
   bool              DetectConjunction() const;

   //! Return if the constellation design is fully specified.
   bool IsSetup() const { return mOptions.IsSetup(); }

   //! Validate the constellation design, returning any deficiencies.
   std::string Validate() const { return mOptions.ValidateSetup(); }

   //! Return if the constellation has a Walker Delta design.
   bool IsWalkerDelta() const { return mOptions.IsWalkerDelta(); }

   //! Return if the constellation has a Walker Star design.
   bool IsWalkerStar() const { return mOptions.IsWalkerStar(); }

   //! Return if the constellation has a general design.
   bool IsGeneral() const { return mOptions.IsGeneral(); }

   //! Return the total number of members of constellations produced by this object.
   unsigned int GetNumTotalSats() const { return mOptions.GetTotalSats(); }

   //! Return the number of orbital planes of constellations produced by this object.
   unsigned int GetNumPlanes() const { return mOptions.GetNumPlanes(); }

   //! Return the number of members per orbital plane of constellations produced by this object.
   unsigned int GetSatsPerPlane() const { return mOptions.GetSatsPerPlane(); }

   // Return the Walker inter-plane phasing parameter angle of constellations produced by this object.
   unsigned int GetWalkerF() const { return mOptions.GetWalkerF(); }

   //! Return the inter-plane phasing angle of constellations produced by this object.
   const UtAngleValue& GetAnomalyAlias() const { return mOptions.GetAnomalyAlias(); }

   //! Return the range in RAAN over which the orbital planes are distributed for constellations produced by this object.
   const UtAngleValue& GetRAAN_Range() const { return mOptions.GetRAAN_Range(); }

   //! Get the semi-major axis of the orbits for constellations produced by this object.
   const UtLengthValue& GetSemiMajorAxis() const { return mOptions.GetSemiMajorAxis(); }

   //! Get the circular altitude of the orbits for constellations produced by this object.
   const UtLengthValue& GetCircularAltitude() const { return mOptions.GetCircularAltitude(); }

   //! Get the period of orbits for constellations produced by this object.
   const UtTimeValue& GetOrbitalPeriod() const { return mOptions.GetOrbitalPeriod(); }

   //! Get the number of revolutions per day of orbits for constellations produced by this object.
   double GetRevolutionsPerDay() const { return mOptions.GetRevolutionsPerDay(); }

   //! Get the inclination of orbits for constellations produced by this object.
   const UtAngleValue& GetInclination() const { return mOptions.GetInclination(); }

   //! Get the RAAN of the zeroth orbital plane for constellations produced by this object.
   const UtAngleValue& GetInitialRAAN() const { return mOptions.GetInitialRAAN(); }

   //! Get the true anomaly of the zeroth member of the zeroth plane for constellations produced by this object.
   const UtAngleValue& GetInitialAnomaly() const { return mOptions.GetInitialAnomaly(); }

   //! Get the name of constellations produced by this object.
   const std::string& GetConstellationName() const { return mOptions.GetConstellationName(); }

   //! Get the type of platforms for constellations produced by this object.
   const std::string& GetPlatformType() const { return mOptions.GetPlatformType(); }

   //! Get the base path to any files produced by this object.
   const UtPath& GetBasePath() const { return mOptions.GetBasePath(); }

   //! Set the semi-major axis of the orbits for constellations produced by this object.
   void SetSemiMajorAxis(const UtLengthValue& aSemiMajorAxis) { mOptions.SetSemiMajorAxis(aSemiMajorAxis); }

   //! Set the circular altitude of the orbits for constellations produced by this object.
   void SetCircularAltitude(const UtLengthValue& aCircularAltitude) { mOptions.SetCircularAltitude(aCircularAltitude); }

   //! Set the period of the orbits for constellations produces by this object.
   void SetOrbitalPeriod(const UtTimeValue& aOrbitalPeriod) { mOptions.SetOrbitalPeriod(aOrbitalPeriod); }

   //! Set the number of revolutions per day for orbits for constellations produced by this object.
   void SetRevolutionsPerDay(double aRevolutionsPerDay) { mOptions.SetRevolutionsPerDay(aRevolutionsPerDay); }

   //! Set the inclination of the orbits in constellations produced by this object.
   void SetInclination(const UtAngleValue& aInclination) { mOptions.SetInclination(aInclination); }

   //! Set the RAAN of the zeroth orbital plane for constellations produced by this object.
   void SetInitialRAAN(const UtAngleValue& aInitialRAAN) { mOptions.SetInitialRAAN(aInitialRAAN); }

   //! Set the true anomaly of the zeroth member of the zeroth plane for constellations produced by this object.
   void SetInitialAnomaly(const UtAngleValue& aInitialAnomaly) { mOptions.SetInitialAnomaly(aInitialAnomaly); }

   //! Set the name of constellations produced by this object.
   void SetConstellationName(const std::string& aName) { mOptions.SetConstellationName(aName); }

   //! Set the type of platform for members of the constellation produced by this object.
   void SetPlatformType(const std::string& aPlatformType) { mOptions.SetPlatformType(aPlatformType); }

   //! Set the base path to any files generated by this object.
   void SetBasePath(const UtPath& aBasePath) { mOptions.SetBasePath(aBasePath); }

private:
   explicit WsfConstellationMaker(const WsfConstellationOptions& aOptions)
      : mOptions{aOptions}
   {
   }

   std::string GetGeneratedName() const;
   void        CreateContainingFolder() const;

   WsfPlatform*      CheckTypeExistence(WsfSimulation& aSimulation) const;
   void              CheckNameCollisions(const WsfSimulation& aSimulation) const;
   bool              FilterMember(WsfScriptContext& aContext,
                                  UtScript*         aFilterScriptPtr,
                                  double            aSimTime,
                                  unsigned int      aPlane,
                                  unsigned int      aSatellite) const;
   WsfConstellation* AddMembersToSimulation(double            aCreationTime,
                                            WsfSimulation&    aSimulation,
                                            WsfScriptContext& aContext,
                                            UtScript*         aFilterScriptPtr,
                                            WsfPlatform*      aPlatformType) const;
   void SetupMemberElements(WsfPlatform* aMemberPtr, unsigned int aPlane, unsigned int aSat, const UtCalendar& aEpoch) const;
   void SetupMembers(WsfScriptContext& aContext,
                     UtScript*         aFilterScriptPtr,
                     double            aSimTime,
                     WsfConstellation* aConstellationPtr) const;

   WsfConstellationOptions mOptions;
};

#endif // WSFCONSTELLATIONMAKER_HPP
