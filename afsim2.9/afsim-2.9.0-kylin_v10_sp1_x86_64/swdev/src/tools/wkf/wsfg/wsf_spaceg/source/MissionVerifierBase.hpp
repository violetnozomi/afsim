// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MISSIONVERIFIERBASE_HPP
#define MISSIONVERIFIERBASE_HPP

#include <memory>
#include <string>

class WsfScenario;
class WsfSpaceMoverBase;

#include "wsf_spaceg_export.h"

#include "MissionVerificationResults.hpp"

namespace wsfg
{

namespace spaceg
{

class OrbitalMissionData;

//! This class represents the common functionality needed to verify orbital mission
//! sequences in the Astrolabe tool. Subclasses need only override a few methods that
//! are called from the template pattern method Verify.
class WSF_SPACEG_EXPORT MissionVerifierBase
{
public:
   MissionVerifierBase()          = default;
   virtual ~MissionVerifierBase() = default;

   MissionVerificationResults Verify(const std::string& aPlatformName, const OrbitalMissionData& aData);

protected:
   //! Return a space mover for the named platform.
   //!
   //! This should return the space mover for the platform with the given @p aPlatformName.
   //!
   //! \param aPlatformName - The name of the platform for which the mover should be returned.
   //! \returns - A space mover for the given platform.
   virtual WsfSpaceMoverBase* GetSpaceMover(const std::string& aPlatformName) = 0;

   //! Return the simulation start and platform creation time of the given platform.
   //!
   //! This will return the simulation start time and the platform creation time for the
   //! platform with the given @p aPlatformName.
   //!
   //! \param aPlatformName - The name of the platform for which the relevant times should be returned.
   //! \returns - A pair containing the simulation start time, and the platform creation time.
   virtual std::pair<UtCalendar, UtCalendar> GetStartEpoch(const std::string& aPlatformName) = 0;

   //! Return the scenario object that should be used during verification.
   //!
   //! Derived classes will need to provide a WsfScenario object that is used during verification.
   //!
   //! \returns - The scenario object to use during verification.
   virtual const WsfScenario& GetScenario() const = 0;

   static void    SetColors(MissionVerificationResults& aResults);
   static UtColor GetColor(int aIndex, int aSize);
   static float   Mix(float aStart, float aStop, double aParam);
};

} // namespace spaceg

} // namespace wsfg

#endif // MISSIONVERIFIERBASE_HPP
