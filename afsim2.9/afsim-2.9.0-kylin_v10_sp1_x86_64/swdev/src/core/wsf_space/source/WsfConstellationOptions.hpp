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

#ifndef WSFCONSTELLATIONOPTIONS_HPP
#define WSFCONSTELLATIONOPTIONS_HPP

#include "wsf_space_export.h"

#include <ostream>
#include <string>

#include "UtPath.hpp"
#include "UtUnitTypes.hpp"
#include "WsfSpaceOrbitSize.hpp"

//! This class represents the options that for a constellation as created
//! by the constellation maker.
class WSF_SPACE_EXPORT WsfConstellationOptions
{
public:
   WsfConstellationOptions(const WsfConstellationOptions& aOther) = default;

   WsfConstellationOptions& operator=(const WsfConstellationOptions& aOther) = default;

   static WsfConstellationOptions CreateWalkerDelta(unsigned int aNumTotalSats,
                                                    unsigned int aNumPlanes,
                                                    unsigned int aWalkerF);
   static WsfConstellationOptions CreateWalkerStar(unsigned int aNumTotalSats, unsigned int aNumPlanes, unsigned int aWalkerF);
   static WsfConstellationOptions CreateGeneral(unsigned int        aNumPlanes,
                                                unsigned int        aNumSatsPerPlane,
                                                const UtAngleValue& aAnomalyAlias,
                                                const UtAngleValue& aRAAN_Range);

   bool        IsSetup() const;
   std::string ValidateSetup() const;

   //! Return if these options are for a Walker Delta design.
   bool IsWalkerDelta() const { return mConstellationType == ConstellationType::cWALKER_DELTA; }

   //! Return if these options are for a Walker Star design.
   bool IsWalkerStar() const { return mConstellationType == ConstellationType::cWALKER_STAR; }

   //! Return if these options are for a general design.
   bool IsGeneral() const { return mConstellationType == ConstellationType::cGENERAL; }

   //! Return the total number of members in this constellation design.
   unsigned int GetTotalSats() const { return mNumTotalSats; }

   //! Return the number of orbital planes in this constellation design.
   unsigned int GetNumPlanes() const { return mNumPlanes; }

   //! Return the number of members per plane in this constellation design.
   unsigned int GetSatsPerPlane() const { return mSatsPerPlane; }

   //! Return the Walker inter-plane phasing parameter in this constellation design.
   unsigned int GetWalkerF() const { return mWalkerF; }

   //! Return the inter-plane phasing angle in this constellation design.
   const UtAngleValue& GetAnomalyAlias() const { return mAnomalyAlias; }

   //! Return the range in RAAN over which the orbital planes are distributed in this constellation design.
   const UtAngleValue& GetRAAN_Range() const { return mRAAN_Range; }

   //! Return the semi-major axis of the orbits in this constellation design.
   const UtLengthValue& GetSemiMajorAxis() const { return mOrbitSize.GetSemiMajorAxis(); }

   //! Return the circular altitude of the orbits in this constellation design.
   const UtLengthValue& GetCircularAltitude() const { return mOrbitSize.GetCircularAltitude(); }

   //! Return the orbital period of the orbits in this constellation design.
   const UtTimeValue& GetOrbitalPeriod() const { return mOrbitSize.GetOrbitalPeriod(); }

   //! Return the number of revolutions per day of the orbits in this constellation design.
   double GetRevolutionsPerDay() const { return mOrbitSize.GetRevolutionsPerDay(); }

   //! Return if the orbit size was specified via semi-major axis.
   bool SpecifiedAsSemiMajorAxis() const { return mOrbitSize.SpecifiedAsSemiMajorAxis(); }

   //! Return if the orbit size was specified via the circular altitude.
   bool SpecifiedAsCircularAltitude() const { return mOrbitSize.SpecifiedAsCircularAltitude(); }

   //! Return if the orbit size was specified via the orbital period.
   bool SpecifiedAsOrbitalPeriod() const { return mOrbitSize.SpecifiedAsOrbitalPeriod(); }

   //! Return if the orbit size was specified via the number of revolutions per day.
   bool SpecifiedAsRevolutionsPerDay() const { return mOrbitSize.SpecifiedAsRevsPerDay(); }

   //! Return the inclination of the orbits in this constellation design.
   const UtAngleValue& GetInclination() const { return mInclination; }

   //! Return the RAAN of the zeroth orbital plane in this constellation design.
   const UtAngleValue& GetInitialRAAN() const { return mInitialRAAN; }

   //! Return the true anomaly of the zeroth member of the zeroth plane in this constellation design.
   const UtAngleValue& GetInitialAnomaly() const { return mInitialAnomaly; }

   //! Return the name of the constellation design.
   const std::string& GetConstellationName() const { return mConstellationName; }

   //! Return the type of platform that would be used for members of this constellation design.
   const std::string& GetPlatformType() const { return mPlatformType; }

   //! Return the base path to which any generated files will be placed for this constellation design.
   const UtPath& GetBasePath() const { return mBasePath; }

   //! Set the semi-major axis of the orbits in this constellation design.
   void SetSemiMajorAxis(const UtLengthValue& aSemiMajorAxis) { mOrbitSize.SetSemiMajorAxis(aSemiMajorAxis); }

   //! Set the circular altitude of the orbits in this constellation design.
   void SetCircularAltitude(const UtLengthValue& aCircularAltitude)
   {
      mOrbitSize.SetCircularAltitude(aCircularAltitude);
   }

   //! Set the orbital period of the orbits in this constellation design.
   void SetOrbitalPeriod(const UtTimeValue& aOrbitalPeriod) { mOrbitSize.SetOrbitalPeriod(aOrbitalPeriod); }

   //! Set the number of revolutions per day of the orbits in this constellation design.
   void SetRevolutionsPerDay(double aRevolutionsPerDay) { mOrbitSize.SetRevolutionsPerDay(aRevolutionsPerDay); }

   //! Set the inclination of the orbits in this constellation design.
   void SetInclination(const UtAngleValue& aInclination) { mInclination = aInclination; }

   //! Set the RAAN of the zeroth plane in this constellation design.
   void SetInitialRAAN(const UtAngleValue& aInitialRAAN) { mInitialRAAN = aInitialRAAN; }

   //! Set the true anomaly of the zeroth member of the zeroth orbital plane in this constellation design.
   void SetInitialAnomaly(const UtAngleValue& aInitialAnomaly) { mInitialAnomaly = aInitialAnomaly; }

   //! Set the name of this constellation design.
   void SetConstellationName(const std::string& aConstellationName) { mConstellationName = aConstellationName; }

   //! Set the platform type of the members of this constellation design.
   void SetPlatformType(const std::string& aPlatformType) { mPlatformType = aPlatformType; }

   //! Set the base path of any generated files for this constellation design.
   void SetBasePath(const UtPath& aBasePath) { mBasePath = aBasePath; }

   void OutputOptions(std::ostream& aStream) const;

   std::string GetGeneratedFilePrefix() const;

   std::string  GetMemberName(unsigned int aPlane, unsigned int aSatellite) const;
   UtAngleValue GetMemberRAAN(unsigned int aPlane) const;
   UtAngleValue GetMemberAnomaly(unsigned int aPlane, unsigned int aSatellite) const;

   constexpr static const char* cBASE_NAME{"Constellation Name"};
   constexpr static const char* cPLATFORM_TYPE{"Platform Type"};
   constexpr static const char* cGENERATION_PATH{"Path to generated files"};
   constexpr static const char* cREVS_PER_DAY{"Revs. Per Day"};
   constexpr static const char* cINCLINATION{"Inclination"};
   constexpr static const char* cNUM_PLANES{"Number of Planes"};
   constexpr static const char* cSATS_PER_PLANE{"Satellites per Plane"};
   constexpr static const char* cINITIAL_RAAN{"Initial RAAN"};
   constexpr static const char* cRAAN_RANGE{"RAAN Range"};
   constexpr static const char* cINITIAL_ANOMALY{"Initial Anomaly"};
   constexpr static const char* cANOMALY_ALIAS{"Anomaly Alias"};
   constexpr static const char* cALTITUDE{"Orbit Altitude"};
   constexpr static const char* cSEMI_MAJOR_AXIS{"Semi-major Axis"};
   constexpr static const char* cPERIOD{"Orbital Period"};
   constexpr static const char* cHEADER_START_MARKER{
      "# New file created by AFSIM Wizard using the Constellation Maker\n/*\n"};

private:
   enum class ConstellationType
   {
      cINVALID,
      cGENERAL,
      cWALKER_DELTA,
      cWALKER_STAR
   };

   WsfConstellationOptions() = default;
   WsfConstellationOptions(ConstellationType   aType,
                           unsigned int        aNumTotalSats,
                           unsigned int        aNumPlanes,
                           unsigned int        aSatsPerPlane,
                           unsigned int        aWalkerF,
                           const UtAngleValue& aAnomalyAlias,
                           const UtAngleValue& aRAAN_Range)
      : mConstellationType{aType}
      , mNumTotalSats{aNumTotalSats}
      , mNumPlanes{aNumPlanes}
      , mSatsPerPlane{aSatsPerPlane}
      , mWalkerF{aWalkerF}
      , mAnomalyAlias{aAnomalyAlias}
      , mRAAN_Range{aRAAN_Range}
   {
   }

   static bool         ValidWalkerInputs(unsigned int aNumTotalSats, unsigned int aNumPlanes, unsigned int aWalkerF);
   static UtAngleValue AnomalyAliasFromWalkerF(unsigned int aWalkerF, unsigned int aNumSatsTotal);


   // Walker constellations are defined via t/p/f; the total number of satellites,
   // the number of planes, and an inter-plane phasing given as an integer f in
   // the range [0, p - 1]. The two kinds of Walker constellation differ in the
   // range of RAAN over which the planes are distributed. The following members
   // are immutable so that this object can maintain the dependencies
   // between them.
   ConstellationType mConstellationType{ConstellationType::cINVALID};
   unsigned int      mNumTotalSats{0};
   unsigned int      mNumPlanes{0};
   unsigned int      mSatsPerPlane{0};
   unsigned int      mWalkerF{0};
   UtAngleValue      mAnomalyAlias{};
   UtAngleValue      mRAAN_Range{};

   // These member variables define the rest of the parameters of the constellation,
   // but do not have any interdependencies, so they may be modified freely.
   wsf::space::OrbitSize mOrbitSize{};
   UtAngleValue          mInclination{-90.0, UtUnitAngle::cDEGREES};
   UtAngleValue          mInitialRAAN{-90.0, UtUnitAngle::cDEGREES};
   UtAngleValue          mInitialAnomaly{-90.0, UtUnitAngle::cDEGREES};
   std::string           mConstellationName{};
   std::string           mPlatformType{};
   UtPath                mBasePath{"./"};
};

#endif // WSFCONSTELLATIONOPTIONS_HPP
