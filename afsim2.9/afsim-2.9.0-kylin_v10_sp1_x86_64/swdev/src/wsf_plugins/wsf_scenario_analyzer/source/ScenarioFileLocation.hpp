// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: ScenarioFileLocation.hpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name: Radiance Technologies
*   Address:         350 Wynn Drive
*                    Huntsville, AL 35805
*
*   Abstract:
*
*******************************************************************************/
#ifndef SCENARIOFILELOCATION_HPP
#define SCENARIOFILELOCATION_HPP
#include <string>

#include "wsf_scenario_analyzer_lib_export.h"

// A ScenarioFileLocation contains the information needed to find the place in the scenario files where an AFSIM object,
// such as a WSF_PLATFORM or a WSF_COMM, is defined. A vector<ScenarioFileLocation> may optionally may be passed to
// buildResultMessage in order to enable links from the check result displayed in the IDE to locations in
// the scenario files that are relevant to the check.
//
// Any strings may be used to initialize mType and mName, but links will only be created if mType and mName
// correspond to the type and name of an AFSIM object that exists in the scenario.
//
// mType must be one of the following valid types: "platform", "comms", processors", "sensors", "weapons", "fuels",
// "zone", "antennaPattern", "infraredSignature", "acousticSignature", "opticalSignature", "radarSignature",
// "reflectivitySignature", "inherentContrast"
//
// mName must be the result of calling GetName() on an instance of a class for which that function is implemented.
struct WSF_SCENARIO_ANALYZER_LIB_EXPORT ScenarioFileLocation
{
   ScenarioFileLocation(const std::string& type, const std::string& name)
      : mType(type),
        mName(name)
   {}
   ~ScenarioFileLocation() = default;

   std::string mType;
   std::string mName;
};

#endif