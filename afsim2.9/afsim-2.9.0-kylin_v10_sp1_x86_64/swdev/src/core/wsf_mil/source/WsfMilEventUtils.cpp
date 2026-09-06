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

#include "WsfMilEventUtils.hpp"

#include <iosfwd>
#include <ostream>
#include <sstream>

#include "UtTime.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfEventUtils.hpp"
#include "WsfImplicitWeapon.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeaponEffects.hpp"
#include "WsfWeaponEngagement.hpp"

namespace wsf
{
namespace event
{

namespace utils
{

// =================================================================================================
void GetAdditionalDE_Info(WsfDirectedEnergyWeapon* aWeaponPtr,
                          WsfPlatform*             aTargetPtr,
                          std::string&             aInfo,
                          bool                     aPrintSingleLinePerEvent)
{
   GetAdditionalImplicitInfo(aWeaponPtr, aTargetPtr, aInfo, aPrintSingleLinePerEvent);
   WsfDirectedEnergyWeapon::Beam* beamPtr = aWeaponPtr->GetBeam();
   if (beamPtr != nullptr)
   {
      std::ostringstream oss;
      oss << ContinueChar(aPrintSingleLinePerEvent);
      oss << " Power: " << beamPtr->GetAveragePower() / 1000.0 << " kW";
      oss << " Peak_Irradiance: " << beamPtr->GetPeakIrradiance() / 1.0E+7 << " kW/cm^2";
      oss << " Edge_Iradiance: " << beamPtr->GetEdgeIrradiance() / 1.0E+7 << " kW/cm^2";
      oss << " Damage_Radius: ";

      if (aWeaponPtr->GetCurrentEngagement() != nullptr)
      {
         WsfWeaponEffects* wePtr = aWeaponPtr->GetCurrentEngagement()->GetWeaponEffects();
         if (wePtr->GetDamageRadius() > 0.0)
         {
            oss << wePtr->GetDamageRadius() * 100.0 << " cm" << ContinueChar(aPrintSingleLinePerEvent);
         }
         else
         {
            oss << beamPtr->GetEdgeRadius() * 100.0 << " cm" << ContinueChar(aPrintSingleLinePerEvent);
         }
      }

      oss << " Energy_Density: " << beamPtr->GetEnergyDensity() / 1.0E+7 << " kJ/cm^2";
      oss << " Energy: " << beamPtr->GetEnergy() / 1000.0 << " kJ" << ContinueChar(aPrintSingleLinePerEvent);

      oss << " Beam_Semi_Major_Axis: " << beamPtr->GetSemiMajorAxis() * 100.0 << " cm";
      oss << " Beam_Semi_Minor_Axis: " << beamPtr->GetSemiMinorAxis() * 100.0 << " cm";
      oss << " Incidence_Angle: " << beamPtr->GetIncidenceAngle() * UtMath::cDEG_PER_RAD << " deg";

      aInfo += oss.str();
   }
}

// =================================================================================================
void GetAdditionalImplicitInfo(WsfImplicitWeapon* aWeaponPtr,
                               WsfPlatform*       aTargetPtr,
                               std::string&       aInfo,
                               bool               aPrintSingleLinePerEvent)
{
   std::ostringstream oss;
   if (aWeaponPtr != nullptr)
   {
      oss << "Weapon: " << aWeaponPtr->GetName();
      oss << " (" << aWeaponPtr->GetType() << ")";
   }

   if (aTargetPtr != nullptr)
   {
      oss << " Target_Type: " << aTargetPtr->GetType() << ContinueChar(aPrintSingleLinePerEvent);
      WsfPlatform* firingPlatform = aWeaponPtr->GetPlatform();
      double       targetVecNED[3];
      firingPlatform->GetRelativeLocationNED(aTargetPtr, targetVecNED);
      oss << " Target_Relative_Loc(NED): " << targetVecNED[0] << " " << targetVecNED[1] << " " << targetVecNED[2]
          << ContinueChar(aPrintSingleLinePerEvent);
      double bearing   = atan2(targetVecNED[1], targetVecNED[0]) * UtMath::cDEG_PER_RAD;
      double range     = UtVec3d::Magnitude(targetVecNED);
      double elevation = -asin(targetVecNED[2] / range) * UtMath::cDEG_PER_RAD;
      oss << " Target_Range_Bearing_Elevation: " << range << " " << bearing << " " << elevation;
   }
   else
   {
      oss << " (Target Not in Simulation)";
   }

   aInfo = oss.str();
}

// =================================================================================================
std::string GetTargetName(size_t aTargetIndex, WsfSimulation& aSimulation)
{
   WsfPlatform* targetPtr = aSimulation.GetPlatformByIndex(aTargetIndex);
   if (targetPtr != nullptr)
   {
      return targetPtr->GetName();
   }
   else
   {
      static const std::string unknown = "<UNKNOWN>";
      return unknown;
   }
}

// =================================================================================================
void PrintWE_FiringGeometry(std::ostream&               aStream,
                            WsfPlatform*                aShooterPtr,
                            WsfPlatform*                aTargetPtr,
                            const wsf::event::Settings& aSettings)
{
   if (aShooterPtr != nullptr)
   {
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << " Launch:";
      PrintLocationData(aStream, aShooterPtr, aSettings);
      PrintNonLocationData(aStream, aShooterPtr, " ", aSettings);
   }

   if (aTargetPtr != nullptr)
   {
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << " Target:";
      PrintLocationData(aStream, aTargetPtr, aSettings);
      PrintNonLocationData(aStream, aTargetPtr, " ", aSettings);
   }

   if ((aShooterPtr != nullptr) && (aTargetPtr != nullptr))
   {
      double wpnToTgtOffsetNED[3];
      aShooterPtr->GetRelativeLocationNED(aTargetPtr, wpnToTgtOffsetNED);

      double heading;
      double pitch;
      double roll;
      aShooterPtr->GetOrientationNED(heading, pitch, roll);

      double n           = wpnToTgtOffsetNED[0];
      double e           = wpnToTgtOffsetNED[1];
      double d           = wpnToTgtOffsetNED[2];
      double slantRange  = sqrt(n * n + e * e + d * d);
      double groundRange = sqrt(n * n + e * e);
      double tgtHeading  = atan2(e, n);
      double tgtBearing  = tgtHeading - heading;
      double downRange   = cos(tgtBearing) * groundRange;
      double crossRange  = sin(tgtBearing) * groundRange;
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << " Truth: Slant_Range: " << slantRange << " m";
      aStream << " Ground_Range: " << groundRange << " m";
      aStream << " Down_Range: " << downRange << " m";
      aStream << " Cross_Range: " << crossRange << " m";
      aStream << " Relative_Alt: " << -d << " m";
   }
}

// =================================================================================================
void PrintWE_PlayerData(std::ostream&              aStream,
                        const WsfWeaponEngagement* aEngagementPtr,
                        WsfPlatform*               aOtherPtr,
                        WsfPlatform*&              aFiringPtr,
                        WsfPlatform*&              aWeaponPtr,
                        WsfPlatform*&              aTargetPtr,
                        WsfSimulation&             aSimulation,
                        int                        aTimeFormat)
{
   // Populate these pointers (if possible), only for the caller's benefit:
   aFiringPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetFiringPlatformIndex());
   aWeaponPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetWeaponPlatformIndex());
   aTargetPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetTargetPlatformIndex());

   if (aEngagementPtr->GetFiringPlatformName() != 0)
   {
      aStream << ' ' << aEngagementPtr->GetFiringPlatformName();
   }
   else if (aEngagementPtr->GetWeaponPlatformName() != 0)
   {
      // Not 'fired' by something else. Treat the weapon platform name as the firing platform name.
      aStream << ' ' << aEngagementPtr->GetWeaponPlatformName();
   }
   else
   {
      aStream << " <UNKNOWN>";
   }

   if (aOtherPtr != nullptr)
   {
      aStream << ' ' << aOtherPtr->GetName();
   }
   else if (aEngagementPtr->GetTargetPlatformIndex() == 0)
   {
      aStream << " <NO-TARGET>";
   }
   else
   {
      aStream << ' ' << aSimulation.GetPlatformNameId(aEngagementPtr->GetTargetPlatformIndex());
   }

   if (aEngagementPtr->GetWeaponPlatformName() == nullptr)
   {
      aStream << " IMPLICIT ( " << aEngagementPtr->GetWeaponSystemName() << " )";
   }
   else
   {
      aStream << ' ' << aEngagementPtr->GetWeaponPlatformName();
   }

   aStream << " Engagement: " << aEngagementPtr->GetSerialNumber();
   aStream << " Start_Time: " << UtTime(aEngagementPtr->GetStartTime(), aTimeFormat);
}

// =================================================================================================
void PrintWE_RelativeGeometry(std::ostream&               aStream,
                              const WsfWeaponEngagement*  aEngagementPtr,
                              WsfPlatform*                aWeaponPtr,
                              WsfPlatform*                aTargetPtr,
                              const wsf::event::Settings& aSettings)
{
   bool   printOneLine = aSettings.PrintSingleLinePerEvent();
   double weaponLocWCS[3];
   double targetLocWCS[3];
   double locECI[3];
   double lat;
   double lon;
   double alt;

   if (aWeaponPtr != nullptr)
   {
      aEngagementPtr->GetWeaponLocationWCS(aWeaponPtr, weaponLocWCS);
      aStream << ContinueChar(printOneLine) << " Weapon:";
      if (aSettings.PrintLLA_Locations())
      {
         UtEntity::ConvertWCSToLLA(weaponLocWCS, lat, lon, alt);
         PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
      }
      else
      {
         aWeaponPtr->ConvertWCSToECI(weaponLocWCS, locECI);
         PrintLocationDataECI(aStream, locECI);
      }
      PrintNonLocationData(aStream, aWeaponPtr, " ", aSettings);
   }

   if (aTargetPtr != nullptr)
   {
      aEngagementPtr->GetTargetLocationWCS(aTargetPtr, targetLocWCS);
      aStream << ContinueChar(printOneLine) << " Target:";
      if (aSettings.PrintLLA_Locations())
      {
         UtEntity::ConvertWCSToLLA(targetLocWCS, lat, lon, alt);
         PrintLocationDataLLA(aStream, lat, lon, alt, aSettings.GetLatLonFormat());
      }
      else
      {
         aTargetPtr->ConvertWCSToECI(targetLocWCS, locECI);
         PrintLocationDataECI(aStream, locECI);
      }
      PrintNonLocationData(aStream, aTargetPtr, " ", aSettings);
   }

   if ((aWeaponPtr != nullptr) && (aTargetPtr != nullptr))
   {
      // The current target location and the computed target location may be different.
      UtEntity target(*aTargetPtr);
      target.SetLocationWCS(targetLocWCS);
      double wpnPosInTgtECS[3];
      target.ConvertWCSToECS(weaponLocWCS, wpnPosInTgtECS);
      // The miss distance as declared by the engagement model. This may be different that the distance
      // between the current positions because the PCA may have occurred between missile timesteps and
      // the missile model may not report back the locations at PCA.
      double missDistance = aEngagementPtr->GetMissDistance(aTargetPtr);
      // Scale the location of the weapon WRT to the target so the magnitude matches the declared miss distance.
      UtVec3d::Normalize(wpnPosInTgtECS);
      UtVec3d::Multiply(wpnPosInTgtECS, wpnPosInTgtECS, missDistance);
      aStream << ContinueChar(printOneLine) << " Miss_Distance: " << missDistance << " m";
      aStream << ContinueChar(printOneLine) << " Weapon_Loc_ECS: [ " << wpnPosInTgtECS[0] << ' ' << wpnPosInTgtECS[1]
              << ' ' << wpnPosInTgtECS[2] << " ] m";
      double wpnVelWCS[3];
      aWeaponPtr->GetVelocityWCS(wpnVelWCS);
      double tgtVelWCS[3];
      aTargetPtr->GetVelocityWCS(tgtVelWCS);
      double relVelWCS[3];
      UtVec3d::Subtract(relVelWCS, wpnVelWCS, tgtVelWCS);
      double impactSpeed = UtVec3d::Normalize(relVelWCS);
      double dirECS[3];
      target.ConvertWCSVectorToECS(dirECS, relVelWCS);
      aStream << ContinueChar(printOneLine) << " Weapon_Vel_ECS: " << impactSpeed << " m/s * [ " << dirECS[0] << ' '
              << dirECS[1] << ' ' << dirECS[2] << " ]";
   }
   else if (aWeaponPtr != nullptr)
   {
      double missDistance = aEngagementPtr->GetMissDistance(nullptr);
      aStream << ContinueChar(printOneLine) << " Miss_Distance: " << missDistance << " m";
   }
}

// =================================================================================================
void PrintWE_Termination(std::ostream&               aStream,
                         const WsfWeaponEngagement*  aEngagementPtr,
                         WsfPlatform*                aOtherPtr,
                         WsfSimulation&              aSimulation,
                         const wsf::event::Settings& aSettings)
{
   // In the case of a weapon termination, then aOtherPtr is zero.  This is normal.
   bool isIncidentalTgt = false;

   WsfPlatform* firingPtr = nullptr;
   WsfPlatform* weaponPtr = nullptr;
   WsfPlatform* targetPtr = nullptr;

   PrintWE_PlayerData(aStream, aEngagementPtr, aOtherPtr, firingPtr, weaponPtr, targetPtr, aSimulation, aSettings.GetTimeFormat());

   // Print out weapon effects data...

   aStream << ContinueChar(aSettings.PrintSingleLinePerEvent());
   WsfWeaponEffects* effectsPtr = aEngagementPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      if (effectsPtr->GetPkConstraint() >= 0.0)
      {
         if (effectsPtr->UseLaunchPk())
         {
            aStream << " Launch_Pk: " << effectsPtr->GetLaunchPk();
         }
         else if (effectsPtr->UseInterceptPk())
         {
            aStream << " Intercept_Pk: " << effectsPtr->GetInterceptPk();
         }
         aStream << " Pk_Degrade: " << effectsPtr->GetPkDegrade();
         aStream << " Pk: " << effectsPtr->GetPkConstraint();
         if (effectsPtr->GetPkDrawn() >= 0.0)
         {
            aStream << " Pk_Drawn: " << effectsPtr->GetPkDrawn();
         }
         if (effectsPtr->GetCEP() > 0.0)
         {
            aStream << " CEP: " << effectsPtr->GetCEP();
         }
      }
   }

   if ((targetPtr == nullptr) && (aOtherPtr == nullptr))
   {
      // There is complete agreement that the target is missing...
      aStream << " UNKNOWN_TARGET Damage_Factor: 0";
   }
   else if (targetPtr == aOtherPtr)
   {
      // There is complete agreement on who the desired target is...
      aStream << " INTENDED_TARGET Damage_Factor: " << targetPtr->GetDamageFactor();
   }
   else if (aOtherPtr != nullptr)
   {
      // There is agreement that this was NOT the intended target...
      isIncidentalTgt = true;
      aStream << " INCIDENTAL_TARGET Damage_Factor: " << aOtherPtr->GetDamageFactor();
   }
   else if (targetPtr != nullptr)
   {
      // There is partial agreement on who the target is...
      aStream << " INTENDED_TARGET Damage_Factor: " << targetPtr->GetDamageFactor();
   }

   WsfWeaponEffects::Result result = aEngagementPtr->GetTargetResult();
   if (isIncidentalTgt)
   {
      result = aEngagementPtr->GetIncidentalResult();
   }

   aStream << " Result: " << WsfWeaponEffects::GetResultString(result);

   std::string extendedResult(aEngagementPtr->GetExtendedResult());
   if (!extendedResult.empty())
   {
      for (char& eResult : extendedResult)
      {
         if (eResult == ' ')
         {
            eResult = '_';
         }
      }
      aStream << ContinueChar(aSettings.PrintSingleLinePerEvent()) << " Extended_Result: " << extendedResult;
   }

   // Print the relative geometry data. If the target being evaluated is not the intended target,
   // then print the geometry data using the target being evaluated.

   if (aOtherPtr != nullptr)
   {
      targetPtr = aOtherPtr;
   }

   if (weaponPtr == nullptr)
   {
      // Implicit Engagement:
      PrintWE_FiringGeometry(aStream, firingPtr, targetPtr, aSettings);
   }
   else
   {
      // Explicit Engagement:
      PrintWE_RelativeGeometry(aStream, aEngagementPtr, weaponPtr, targetPtr, aSettings);
   }

   aStream << '\n';
}
} // namespace utils

namespace utilsCSV
{

// =================================================================================================
void GetAdditionalDE_Info(WsfDirectedEnergyWeapon* aWeaponPtr, WsfPlatform* aTargetPtr, std::string& aInfo)
{
   GetAdditionalImplicitInfo(aWeaponPtr, aTargetPtr, aInfo);
   WsfDirectedEnergyWeapon::Beam* beamPtr = aWeaponPtr->GetBeam();
   if (beamPtr != nullptr)
   {
      std::ostringstream oss;
      oss << ',' << beamPtr->GetAveragePower() / 1000.0 << ',' << beamPtr->GetPeakIrradiance() / 1.0E+7 << ','
          << beamPtr->GetEdgeIrradiance() / 1.0E+7;

      oss << ',';
      if (aWeaponPtr->GetCurrentEngagement() != nullptr)
      {
         WsfWeaponEffects* wePtr = aWeaponPtr->GetCurrentEngagement()->GetWeaponEffects();
         if (wePtr->GetDamageRadius() > 0.0)
         {
            oss << wePtr->GetDamageRadius() * 100.0;
         }
         else
         {
            oss << beamPtr->GetEdgeRadius() * 100.0;
         }
      }

      oss << ',' << beamPtr->GetEnergyDensity() / 1.0E+7 << ',' << beamPtr->GetEnergy() / 1000.0;
      oss << ',' << beamPtr->GetSemiMajorAxis() * 100.0 << ',' << beamPtr->GetSemiMinorAxis() * 100.0 << ','
          << beamPtr->GetIncidenceAngle() * UtMath::cDEG_PER_RAD;
      aInfo += oss.str();
   }
}

// =================================================================================================
void GetAdditionalImplicitInfo(WsfImplicitWeapon* aWeaponPtr, WsfPlatform* aTargetPtr, std::string& aInfo)
{
   std::ostringstream oss;
   if (aWeaponPtr != nullptr)
   {
      oss << aWeaponPtr->GetName() << ',' << aWeaponPtr->GetType();
   }
   else
   {
      oss << ',';
   }

   if (aTargetPtr != nullptr)
   {
      oss << ',' << aTargetPtr->GetType();
      WsfPlatform* firingPlatform = aWeaponPtr->GetPlatform();
      double       targetVecNED[3];
      firingPlatform->GetRelativeLocationNED(aTargetPtr, targetVecNED);
      oss << ',' << targetVecNED[0] << ',' << targetVecNED[1] << ',' << targetVecNED[2];
      double bearing   = atan2(targetVecNED[1], targetVecNED[0]) * UtMath::cDEG_PER_RAD;
      double range     = UtVec3d::Magnitude(targetVecNED);
      double elevation = -asin(targetVecNED[2] / range) * UtMath::cDEG_PER_RAD;
      oss << ',' << range << ',' << bearing << ',' << elevation;
   }
   else
   {
      oss << ",,,,,,,";
   }

   aInfo = oss.str();
}

// =================================================================================================
std::string GetTargetName(std::ostream& aStream, size_t aTargetIndex, WsfSimulation& aSimulation)
{
   WsfPlatform* targetPtr = aSimulation.GetPlatformByIndex(aTargetIndex);
   if (targetPtr != nullptr)
   {
      return targetPtr->GetName();
   }
   else
   {
      static const std::string unknown = "UNKNOWN";
      return unknown;
   }
}

// =================================================================================================
void PrintWE_FiringGeometry(std::ostream& aStream, WsfPlatform* aShooterPtr, WsfPlatform* aTargetPtr)
{
   if (aShooterPtr != nullptr)
   {
      // aStream << ContinueChar(aSettings)  ',';
      PrintLocationData(aStream, aShooterPtr);
      PrintNonLocationData(aStream, aShooterPtr, " ");
   }
   else
   {
      aStream << ",,,,,,,,,,,,,,,,,,,,,,,,,";
      //,lat    ,long   ,alt    ,hdg    ,pit    ,rol    ,x      ,y      ,z      ,spd
      // PrintLocationData(0.0, 0.0, 0.0);
      // aStream << ", 0, 0, 0, 0, 0, 0, 0"; //hdg,pit,rol,x,y,z,spd
   }

   if (aTargetPtr != nullptr)
   {
      // aStream << ContinueChar(aSettings)  ',';
      PrintLocationData(aStream, aTargetPtr);
      PrintNonLocationData(aStream, aTargetPtr, " ");
   }
   else
   {
      aStream << ",,,,,,,,,,,,,,,,,,,,,,,,,";
      //,lat    ,long   ,alt    ,hdg    ,pit    ,rol    ,spd ,spd x ,spdy  ,spdz  ,acl ,aclx, acly, aclz
      // PrintLocationData(0.0, 0.0, 0.0);
      // aStream << ", 0, 0, 0, 0, 0, 0, 0"; //hdg,pit,rol,x,y,z,spd
   }

   if ((aShooterPtr != nullptr) && (aTargetPtr != nullptr))
   {
      double wpnToTgtOffsetNED[3];
      aShooterPtr->GetRelativeLocationNED(aTargetPtr, wpnToTgtOffsetNED);

      double heading;
      double pitch;
      double roll;
      aShooterPtr->GetOrientationNED(heading, pitch, roll);

      double n           = wpnToTgtOffsetNED[0];
      double e           = wpnToTgtOffsetNED[1];
      double d           = wpnToTgtOffsetNED[2];
      double slantRange  = sqrt(n * n + e * e + d * d);
      double groundRange = sqrt(n * n + e * e);
      double tgtHeading  = atan2(e, n);
      double tgtBearing  = tgtHeading - heading;
      double downRange   = cos(tgtBearing) * groundRange;
      double crossRange  = sin(tgtBearing) * groundRange;
      aStream << ',' << slantRange;
      aStream << ',' << groundRange;
      aStream << ',' << downRange;
      aStream << ',' << crossRange;
      aStream << ',' << -d;
   }
   else
   {
      aStream << ",,,,,";
   }
}

// =================================================================================================
void PrintWE_PlayerData(std::ostream&              aStream,
                        const WsfWeaponEngagement* aEngagementPtr,
                        WsfPlatform*               aOtherPtr,
                        WsfPlatform*&              aFiringPtr,
                        WsfPlatform*&              aWeaponPtr,
                        WsfPlatform*&              aTargetPtr,
                        WsfSimulation&             aSimulation)
{
   // Populate these pointers (if possible), only for the caller's benefit:
   aFiringPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetFiringPlatformIndex());
   aWeaponPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetWeaponPlatformIndex());
   aTargetPtr = aSimulation.GetPlatformByIndex(aEngagementPtr->GetTargetPlatformIndex());

   if (aEngagementPtr->GetFiringPlatformName() != 0)
   {
      aStream << aEngagementPtr->GetFiringPlatformName();
   }
   else if (aEngagementPtr->GetWeaponPlatformName() != 0)
   {
      // Not 'fired' by something else. Treat the weapon platform name as the firing platform name.
      aStream << aEngagementPtr->GetWeaponPlatformName();
   }
   else
   {
      aStream << "UNKNOWN";
   }

   aStream << ',';
   if (aOtherPtr != nullptr)
   {
      aStream << aOtherPtr->GetName();
   }
   else if (aEngagementPtr->GetTargetPlatformIndex() == 0)
   {
      aStream << "NO_TARGET";
   }
   else
   {
      aStream << aSimulation.GetPlatformNameId(aEngagementPtr->GetTargetPlatformIndex());
   }

   aStream << ',';
   if (aEngagementPtr->GetWeaponPlatformName() == nullptr)
   {
      aStream << aEngagementPtr->GetWeaponSystemName();
   }
   else
   {
      aStream << aEngagementPtr->GetWeaponPlatformName();
   }

   aStream << ',' << aEngagementPtr->GetSerialNumber();
}

// =================================================================================================
void PrintWE_RelativeGeometry(std::ostream&              aStream,
                              const WsfWeaponEngagement* aEngagementPtr,
                              WsfPlatform*               aWeaponPtr,
                              WsfPlatform*               aTargetPtr)
{
   double weaponLocWCS[3];
   double targetLocWCS[3];
   double locECI[3];
   double lat;
   double lon;
   double alt;

   if (aWeaponPtr != nullptr)
   {
      aEngagementPtr->GetWeaponLocationWCS(aWeaponPtr, weaponLocWCS);
      /* if (Out().PrintLLA_Locations())
      {*/
      UtEntity::ConvertWCSToLLA(weaponLocWCS, lat, lon, alt);
      PrintLocationDataLLA(aStream, lat, lon, alt);
      /* }
      else
      {*/
      aWeaponPtr->ConvertWCSToECI(weaponLocWCS, locECI);
      PrintLocationDataECI(aStream, locECI);
      /*}*/
      PrintNonLocationData(aStream, aWeaponPtr, " ");
   }
   else
   {
      aStream << ",,,,,,,,,,,,,,,,,,";
      //,lat    ,long   ,alt    ,hdg    ,pit    ,rol    ,x      ,y      ,z      ,spd
      // PrintLocationData(0.0, 0.0, 0.0);
      // aStream << ",,,,,,,"; //hdg,pit,rol,x,y,z,spd
   }

   if (aTargetPtr != nullptr)
   {
      aEngagementPtr->GetTargetLocationWCS(aTargetPtr, targetLocWCS);
      /*if (Out().PrintLLA_Locations())
      {*/
      UtEntity::ConvertWCSToLLA(targetLocWCS, lat, lon, alt);
      PrintLocationDataLLA(aStream, lat, lon, alt);
      /* }
      else
      {*/
      aTargetPtr->ConvertWCSToECI(targetLocWCS, locECI);
      PrintLocationDataECI(aStream, locECI);
      //}
      PrintNonLocationData(aStream, aTargetPtr, " ");
   }
   else
   {
      //,lat ,long ,alt ,hdg ,pit ,rol ,x ,y ,z ,spd
      // PrintLocationData(0.0, 0.0, 0.0);
      // aStream << ", 0, 0, 0, 0, 0, 0, 0"; //hdg,pit,rol,x,y,z,spd
      aStream << ",,,,,,,,,,,,,,,,,,";
   }

   aStream << ",,,,,"; // must add spacing for the implicit engagement section
   if ((aWeaponPtr != nullptr) && (aTargetPtr != nullptr))
   {
      // The current target location and the computed target location may be different.
      UtEntity target(*aTargetPtr);
      target.SetLocationWCS(targetLocWCS);
      double wpnPosInTgtECS[3];
      target.ConvertWCSToECS(weaponLocWCS, wpnPosInTgtECS);
      // The miss distance as declared by the engagement model. This may be different that the distance
      // between the current positions because the PCA may have occurred between missile timesteps and
      // the missile model may not report back the locations at PCA.
      double missDistance = aEngagementPtr->GetMissDistance(aTargetPtr);
      // Scale the location of the weapon WRT to the target so the magnitude matches the declared miss distance.
      UtVec3d::Normalize(wpnPosInTgtECS);
      UtVec3d::Multiply(wpnPosInTgtECS, wpnPosInTgtECS, missDistance);
      aStream << ',' << missDistance;
      aStream << ',' << wpnPosInTgtECS[0] << ',' << wpnPosInTgtECS[1] << ',' << wpnPosInTgtECS[2];
      double wpnVelWCS[3];
      aWeaponPtr->GetVelocityWCS(wpnVelWCS);
      double tgtVelWCS[3];
      aTargetPtr->GetVelocityWCS(tgtVelWCS);
      double relVelWCS[3];
      UtVec3d::Subtract(relVelWCS, wpnVelWCS, tgtVelWCS);
      double impactSpeed = UtVec3d::Normalize(relVelWCS);
      double dirECS[3];
      target.ConvertWCSVectorToECS(dirECS, relVelWCS);
      aStream << ',' << impactSpeed << ',' << dirECS[0] << ',' << dirECS[1] << ',' << dirECS[2];
   }
   else if (aWeaponPtr != nullptr)
   {
      double missDistance = aEngagementPtr->GetMissDistance(nullptr);
      aStream << ',' << missDistance;
      aStream << ",,,,,,,";
   }
   else
   {
      aStream << ",,,,,,,,";
   }
}

// =================================================================================================
void PrintWE_Termination(std::ostream&              aStream,
                         const WsfWeaponEngagement* aEngagementPtr,
                         WsfPlatform*               aOtherPtr,
                         WsfSimulation&             aSimulation,
                         int                        aTimeFormat)
{
   // In the case of a weapon termination, then aOtherPtr is zero.  This is normal.
   bool isIncidentalTgt = false;

   WsfPlatform* firingPtr = nullptr;
   WsfPlatform* weaponPtr = nullptr;
   WsfPlatform* targetPtr = nullptr;

   PrintWE_PlayerData(aStream, aEngagementPtr, aOtherPtr, firingPtr, weaponPtr, targetPtr, aSimulation);
   aStream << ',' << UtTime(aEngagementPtr->GetStartTime(), aTimeFormat);
   // Print out weapon effects data...

   WsfWeaponEffects* effectsPtr = aEngagementPtr->GetWeaponEffects();
   if (effectsPtr != nullptr)
   {
      if (effectsPtr->GetPkConstraint() >= 0.0)
      {
         aStream << ',';
         if (effectsPtr->UseLaunchPk())
         {
            aStream << "1,0," << effectsPtr->GetLaunchPk();
         }
         else if (effectsPtr->UseInterceptPk())
         {
            aStream << "0,1," << effectsPtr->GetInterceptPk();
         }
         else
         {
            aStream << "0,0,";
         }

         aStream << ',' << effectsPtr->GetPkDegrade();
         aStream << ',' << effectsPtr->GetPkConstraint();
         aStream << ',';
         if (effectsPtr->GetPkDrawn() >= 0.0)
         {
            aStream << effectsPtr->GetPkDrawn();
         }

         aStream << ',';
         if (effectsPtr->GetCEP() >= 0.0)
         {
            aStream << effectsPtr->GetCEP();
         }
      }
      else
      {
         // launch_pk, intercept_pk,PK,Pk Degrade,Pk Constraint,Pk Draw,CEP
         aStream << ",,,,,,,";
      }
   }
   else
   {
      // launch_pk, intercept_pk, pk,Pk Degrade,Pk Constraint,Pk Draw,CEP
      aStream << ",,,,,,,";
   }

   if ((targetPtr == nullptr) && (aOtherPtr == nullptr))
   {
      // There is complete agreement that the target is missing...
      aStream << ",UNKNOWN_TARGET,0";
   }
   else if (targetPtr == aOtherPtr)
   {
      // There is complete agreement on who the desired target is...
      aStream << ",INTENDED_TARGET, " << targetPtr->GetDamageFactor();
   }
   else if (aOtherPtr != nullptr)
   {
      // There is agreement that this was NOT the intended target...
      isIncidentalTgt = true;
      aStream << ",INCIDENTAL_TARGET," << aOtherPtr->GetDamageFactor();
   }
   else if (targetPtr != nullptr)
   {
      // There is partial agreement on who the target is...
      aStream << ",INTENDED_TARGET," << targetPtr->GetDamageFactor();
   }
   else
   {
      aStream << ",UNKNOWN_TARGET,0";
   }

   WsfWeaponEffects::Result result = aEngagementPtr->GetTargetResult();
   if (isIncidentalTgt)
   {
      result = aEngagementPtr->GetIncidentalResult();
   }

   aStream << ',' << WsfWeaponEffects::GetResultString(result);

   std::string extendedResult(aEngagementPtr->GetExtendedResult());
   aStream << ',';
   if (!extendedResult.empty())
   {
      for (char& i : extendedResult)
      {
         if (i == ' ')
         {
            i = '_';
         }
      }
      aStream << extendedResult;
   }
   else
   {
      aStream << "N/A";
   }

   // Print the relative geometry data. If the target being evaluated is not the intended target,
   // then print the geometry data using the target being evaluated.

   if (aOtherPtr != nullptr)
   {
      targetPtr = aOtherPtr;
   }

   if (weaponPtr == nullptr)
   {
      // Implicit Engagement:
      PrintWE_FiringGeometry(aStream, firingPtr, targetPtr);
   }
   else
   {
      // Explicit Engagement:
      PrintWE_RelativeGeometry(aStream, aEngagementPtr, weaponPtr, targetPtr);
   }
   aStream << '\n';
}

} // namespace utilsCSV

} // namespace event
} // namespace wsf
