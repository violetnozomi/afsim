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

#ifndef WSFSPACEEVENTUTILS_HPP
#define WSFSPACEEVENTUTILS_HPP

#include "wsf_space_export.h"

#include <ostream>

class UtOrbitalElements;
class UtOrbitalPropagatorBase;

class WsfOrbitalEvent;
class WsfSpaceMoverBase;

namespace wsf
{
namespace event
{

namespace utils
{

void PrintOrbitalElementsData(std::ostream& aStream, const UtOrbitalElements& aOE);

void PrintOrbitalManeuverData(std::ostream&          aStream,
                              WsfSpaceMoverBase*     aSpaceMoverPtr,
                              const WsfOrbitalEvent& aManeuver,
                              bool                   aPrintSingleLinePerEvent);
} // namespace utils

namespace utilsCSV
{

void PrintOrbitalElementsData(std::ostream& aStream, const UtOrbitalElements& aOE);

void PrintOrbitalManeuverData(std::ostream& aStream, WsfSpaceMoverBase* aSpaceMoverPtr, const WsfOrbitalEvent& aManeuver);
} // namespace utilsCSV

} // namespace event
} // namespace wsf

#endif
