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

#include "WsfSpaceEventResults.hpp"

#include <ostream>

#include "UtOrbitalPropagatorBase.hpp"
#include "WsfEventUtils.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceEventUtils.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace wsf
{
namespace event
{

void EclipseEntry::Print(std::ostream& aStream) const
{
#undef GetTimeFormat
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ECLIPSE_ENTRY ";
   utils::PrintOrbitalElementsData(aStream, mSpaceMoverPtr->GetOrbitalState().GetOrbitalElements());
   aStream << '\n';
   utils::PrintLocationData(aStream, mSpaceMoverPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void EclipseEntry::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ECLIPSE_ENTRY";
   aStream << ',' << mSpaceMoverPtr->GetPlatform()->GetName();
   utilsCSV::PrintOrbitalElementsData(aStream, mSpaceMoverPtr->GetOrbitalState().GetOrbitalElements());
   utilsCSV::PrintLocationData(aStream, mSpaceMoverPtr->GetPlatform());
   aStream << '\n';
}

void EclipseExit::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ECLIPSE_EXIT ";
   utils::PrintOrbitalElementsData(aStream, mSpaceMoverPtr->GetOrbitalState().GetOrbitalElements());
   aStream << '\n';
   utils::PrintLocationData(aStream, mSpaceMoverPtr->GetPlatform(), mSettings);
   aStream << '\n';
}

void EclipseExit::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ECLIPSE_EXIT";
   aStream << ',' << mSpaceMoverPtr->GetPlatform()->GetName();
   utilsCSV::PrintOrbitalElementsData(aStream, mSpaceMoverPtr->GetOrbitalState().GetOrbitalElements());
   utilsCSV::PrintLocationData(aStream, mSpaceMoverPtr->GetPlatform());
   aStream << '\n';
}

void OrbitalManeuverInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBITAL_MANEUVER_INITIATED ";
   utils::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver, mSettings.PrintSingleLinePerEvent());
   aStream << '\n';
}

void OrbitalManeuverInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBITAL_MANEUVER_INITIATED";
   utilsCSV::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver);
   aStream << '\n';
}

void OrbitalManeuverUpdated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBITAL_MANEUVER_UPDATED ";
   utils::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver, mSettings.PrintSingleLinePerEvent());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " Delta-V: " << mManeuver.GetDeltaV()
           << " m/s" << '\n';
}

void OrbitalManeuverUpdated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBITAL_MANEUVER_UPDATED";
   utilsCSV::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver);
   aStream << ',' << mManeuver.GetDeltaV() << '\n';
}

void OrbitalManeuverCanceled::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBITAL_MANEUVER_CANCELED ";
   utils::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver, mSettings.PrintSingleLinePerEvent());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " Delta-V: " << mManeuver.GetDeltaV()
           << " m/s" << '\n';
}

void OrbitalManeuverCanceled::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBITAL_MANEUVER_CANCELED";
   utilsCSV::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver);
   aStream << ',' << mManeuver.GetDeltaV() << '\n';
}

void OrbitalManeuverCompleted::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBITAL_MANEUVER_COMPLETED ";
   utils::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver, mSettings.PrintSingleLinePerEvent());
   aStream << utils::ContinueChar(mSettings.PrintSingleLinePerEvent()) << " Delta-V: " << mManeuver.GetDeltaV()
           << " m/s" << '\n';
}

void OrbitalManeuverCompleted::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBITAL_MANEUVER_COMPLETED";
   utilsCSV::PrintOrbitalManeuverData(aStream, mSpaceMoverPtr, mManeuver);
   aStream << ',' << mManeuver.GetDeltaV() << '\n';
}

void OrbitDeterminationInitiated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBIT_DETERMINATION_INITIATED";
   aStream << " " << mPlatformPtr->GetName() << " "; // platform name
   utils::PrintTrackTargetName(aStream, &mLocalTrack, *mPlatformPtr->GetSimulation());
   aStream << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintOrbitalElementsData(aStream, mPropagator.GetOrbitalState().GetOrbitalElements());
   aStream << "\n";
}

void OrbitDeterminationInitiated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBIT_DETERMINATION_INITIATED";
   aStream << ',' << mPlatformPtr->GetName(); // platform name
   utilsCSV::PrintTrackTargetName(aStream, &mLocalTrack, *mPlatformPtr->GetSimulation());
   utilsCSV::PrintOrbitalElementsData(aStream, mPropagator.GetOrbitalState().GetOrbitalElements());
   aStream << "\n";
}

void OrbitDeterminationUpdated::Print(std::ostream& aStream) const
{
   utils::PrintTime(aStream, mSimTime, mSettings.GetTimeFormat());
   aStream << "ORBIT_DETERMINATION_UPDATED";
   aStream << " " << mPlatformPtr->GetName() << " "; // platform name
   utils::PrintTrackTargetName(aStream, &mLocalTrack, *mPlatformPtr->GetSimulation());
   aStream << " " << utils::ContinueChar(mSettings.PrintSingleLinePerEvent());
   utils::PrintOrbitalElementsData(aStream, mPropagator.GetOrbitalState().GetOrbitalElements());
   aStream << "\n";
}

void OrbitDeterminationUpdated::PrintCSV(std::ostream& aStream) const
{
   utilsCSV::PrintTime(aStream, mSimTime);
   aStream << ',' << "ORBIT_DETERMINATION_UPDATED";
   aStream << ',' << mPlatformPtr->GetName(); // platform name
   utilsCSV::PrintTrackTargetName(aStream, &mLocalTrack, *mPlatformPtr->GetSimulation());
   utilsCSV::PrintOrbitalElementsData(aStream, mPropagator.GetOrbitalState().GetOrbitalElements());
   aStream << "\n";
}

} // namespace event
} // namespace wsf
