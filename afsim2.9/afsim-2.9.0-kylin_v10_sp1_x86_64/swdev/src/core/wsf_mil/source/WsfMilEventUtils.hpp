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

#ifndef WSFMILEVENTUTILS_HPP
#define WSFMILEVENTUTILS_HPP

#include "wsf_mil_export.h"

#include <iosfwd>
#include <ostream>
#include <sstream>

#include "UtTime.hpp"

class WsfDirectedEnergyWeapon;
#include "WsfEventResult.hpp"
class WsfImplicitWeapon;
class WsfPlatform;
class WsfSimulation;
class WsfWeaponEngagement;

namespace wsf
{
namespace event
{

//! event_output utilities for printing of text based event results
namespace utils
{

WSF_MIL_EXPORT void GetAdditionalDE_Info(WsfDirectedEnergyWeapon* aWeaponPtr,
                                         WsfPlatform*             aTargetPtr,
                                         std::string&             aInfo,
                                         bool                     aPrintSingleLinePerEvent);

WSF_MIL_EXPORT void GetAdditionalImplicitInfo(WsfImplicitWeapon* aWeaponPtr,
                                              WsfPlatform*       aTargetPtr,
                                              std::string&       aInfo,
                                              bool               aPrintSingleLinePerEvent);

WSF_MIL_EXPORT std::string GetTargetName(size_t aTargetIndex, WsfSimulation& aSimulation);

WSF_MIL_EXPORT void PrintWE_FiringGeometry(std::ostream&               aStream,
                                           WsfPlatform*                aShooterPtr,
                                           WsfPlatform*                aTargetPtr,
                                           const wsf::event::Settings& aSettings);

WSF_MIL_EXPORT void PrintWE_PlayerData(std::ostream&              aStream,
                                       const WsfWeaponEngagement* aEngagementPtr,
                                       WsfPlatform*               aOtherPtr,
                                       WsfPlatform*&              aFiringPtr,
                                       WsfPlatform*&              aWeaponPtr,
                                       WsfPlatform*&              aTargetPtr,
                                       WsfSimulation&             aSimulation,
                                       int                        aTimeFormat);

WSF_MIL_EXPORT void PrintWE_RelativeGeometry(std::ostream&               aStream,
                                             const WsfWeaponEngagement*  aEngagementPtr,
                                             WsfPlatform*                aWeaponPtr,
                                             WsfPlatform*                aTargetPtr,
                                             const wsf::event::Settings& aSettings);

WSF_MIL_EXPORT void PrintWE_Termination(std::ostream&               aStream,
                                        const WsfWeaponEngagement*  aEngagementPtr,
                                        WsfPlatform*                aOtherPtr,
                                        WsfSimulation&              aSimulation,
                                        const wsf::event::Settings& aSettings);

} // namespace utils

//! csv_event_output utilities for printing of CSV based event results
namespace utilsCSV
{
WSF_MIL_EXPORT void GetAdditionalDE_Info(WsfDirectedEnergyWeapon* aWeaponPtr, WsfPlatform* aTargetPtr, std::string& aInfo);

WSF_MIL_EXPORT void GetAdditionalImplicitInfo(WsfImplicitWeapon* aWeaponPtr, WsfPlatform* aTargetPtr, std::string& aInfo);

WSF_MIL_EXPORT std::string GetTargetName(std::ostream& aStream, size_t aTargetIndex, WsfSimulation& aSimulation);

WSF_MIL_EXPORT void PrintWE_FiringGeometry(std::ostream& aStream, WsfPlatform* aShooterPtr, WsfPlatform* aTargetPtr);

WSF_MIL_EXPORT void PrintWE_PlayerData(std::ostream&              aStream,
                                       const WsfWeaponEngagement* aEngagementPtr,
                                       WsfPlatform*               aOtherPtr,
                                       WsfPlatform*&              aFiringPtr,
                                       WsfPlatform*&              aWeaponPtr,
                                       WsfPlatform*&              aTargetPtr,
                                       WsfSimulation&             aSimulation);

WSF_MIL_EXPORT void PrintWE_RelativeGeometry(std::ostream&              aStream,
                                             const WsfWeaponEngagement* aEngagementPtr,
                                             WsfPlatform*               aWeaponPtr,
                                             WsfPlatform*               aTargetPtr);

WSF_MIL_EXPORT void PrintWE_Termination(std::ostream&              aStream,
                                        const WsfWeaponEngagement* aEngagementPtr,
                                        WsfPlatform*               aOtherPtr,
                                        WsfSimulation&             aSimulation,
                                        int                        aTimeFormat);
} // namespace utilsCSV

} // namespace event
} // namespace wsf

#endif
