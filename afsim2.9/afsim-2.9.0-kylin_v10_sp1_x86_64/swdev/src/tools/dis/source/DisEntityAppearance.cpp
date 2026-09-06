// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisEntityAppearance.hpp"

#include <iostream>

#include "DisAppearanceEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{

const DisUint32 cPAINT_SCHEME_MASK          = 0x00000001; // bit 0
const DisUint32 cMOBILITY_KILL_MASK         = 0x00000002; // bit 1
const DisUint32 cPROPULSION_MASK            = 0x00000002; // bit 1
const DisUint32 cFIREPOWER_MASK             = 0x00000004; // bit 2
const DisUint32 cNVGMODE_MASK               = 0x00000004; // bit 2
const DisUint32 cDAMAGE_LEVEL_MASK          = 0x00000018; // bit 3&4
const DisUint32 cSMOKE_MASK                 = 0x00000060; // bit 5&6
const DisUint32 cENGINE_SMOKE_MASK          = 0x00000040; // bit 6
const DisUint32 cTRAILING_EFFECT_MASK       = 0x00000180; // bit 7&8
const DisUint32 cHATCH_MASK                 = 0x00000e00; // bit 9,10,11
const DisUint32 cCANOPY_MASK                = 0x00000e00; // bit 9,10,11
const DisUint32 cHEADLIGHT_MASK             = 0x00001000; // bit 12
const DisUint32 cLANDING_LIGHT_MASK         = 0x00001000; // bit 12
const DisUint32 cRUNNING_LIGHT_MASK         = 0x00001000; // bit 12
const DisUint32 cTAILLIGHT_MASK             = 0x00002000; // bit 13
const DisUint32 cNAVIGATION_LIGHT_MASK      = 0x00002000; // bit 13
const DisUint32 cBRAKELIGHT_MASK            = 0x00004000; // bit 14
const DisUint32 cANTI_COLLISION_LIGHT_MASK  = 0x00004000; // bit 14
const DisUint32 cFLAMING_MASK               = 0x00008000; // bit 15
const DisUint32 cLAUNCHER_MASK              = 0x00010000; // bit 16
const DisUint32 cAFTERBURNER_MASK           = 0x00010000; // bit 16
const DisUint32 cLOWER_ANTI_COLLISION_MASK  = 0x00020000; // bit 17
const DisUint32 cUPPER_ANTI_COLLISION_MASK  = 0x00040000; // bit 18
const DisUint32 cCAMOUFLAGE_MASK            = 0x00060000; // bit 17&18
const DisUint32 cCONCEALED_MASK             = 0x00080000; // bit 19
const DisUint32 cANTI_COLLISION_DAY_MASK    = 0x00080000; // bit 19
const DisUint32 cIS_BLINKING_MASK           = 0x00100000; // bit 20
const DisUint32 cFROZEN_MASK                = 0x00200000; // bit 21
const DisUint32 cPOWERPLANT_MASK            = 0x00400000; // bit 22
const DisUint32 cDEACTIVATED_MASK           = 0x00800000; // bit 23
const DisUint32 cTENT_MASK                  = 0x01000000; // bit 24
const DisUint32 cFORMATION_LIGHT_MASK       = 0x01000000; // bit 24
const DisUint32 cRAMP_MASK                  = 0x02000000; // bit 25
const DisUint32 cLANDING_GEAR_MASK          = 0x02000000; // bit 25
const DisUint32 cBLACKOUT_LIGHTS_MASK       = 0x04000000; // bit 26
const DisUint32 cCARGO_DOORS_MASK           = 0x04000000; // bit 26
const DisUint32 cBLACKOUT_BRAKES_MASK       = 0x08000000; // bit 27
const DisUint32 cNAVIGATION_BRIGHTNESS_MASK = 0x08000000; // bit 27
const DisUint32 cSPOTLIGHTS_MASK            = 0x10000000; // bit 28
const DisUint32 cINTERIOR_LIGHTS_MASK       = 0x20000000; // bit 29
const DisUint32 cSURRENDER_STATE_MASK       = 0x40000000; // bit 30
const DisUint32 cREVERSE_THRUST_MASK        = 0x40000000; // bit 30
const DisUint32 cMASKED_CLOAKED_MASK        = 0x80000000; // bit 31
const DisUint32 cWEIGHT_ON_WHEELS_MASK      = 0x80000000; // bit 31
} // namespace

DisEntityAppearance::DisEntityAppearance(DisUint32 aValue)
   : mAppearanceRecord(aValue){};

std::string DisEntityAppearance::ToString(DisEnum8 aKind, DisEnum8 aDomain) const
{
   std::string returnString;

   // Appearance record determined by kind and domain
   switch (aKind)
   {
   case 1: // platform type
      switch (aDomain)
      {
      case 1: // land domain
         returnString = GetLandPlatformString();
         break;
      case 2: // air domain
         returnString = GetAirPlatformString();
         break;
      case 3: // Surface domain
         returnString = GetSurfacePlatformString();
         break;
      case 4: // Subsurface domain
         returnString = GetSubSurfacePlatformString();
         break;
      case 5: // Space
         returnString = GetSpacePlatformString();
         break;
      }
      break;
   case 2: // Munition
      break;
   case 3: // Environmental
      break;
   case 4: // Cultural Feature
      break;
   case 5: // Sensor Emitter
      break;
   default:
      break;
   }
   return returnString;
}

void DisEntityAppearance::Stream(std::ostream& aStream, DisEnum8 aKind, DisEnum8 aDomain) const
{
   aStream << ToString(aKind, aDomain) << '\n';
}

DisEntityAppearance* DisEntityAppearance::Clone() const
{
   return new DisEntityAppearance(*this);
}

std::string DisEntityAppearance::GetLandPlatformString() const
{
   std::string returnString = "Land Platform Appearance:\n";
   returnString +=
      "   Paint Scheme:           " +
      DisEnum::Appearance::PaintScheme::ToString((DisEnum::Appearance::PaintScheme::Enum)GetPaintScheme()) + '\n';
   returnString += "   Mobility Killed:        " +
                   DisEnum::Appearance::Mobility::ToString((DisEnum::Appearance::Mobility::Enum)GetMobility()) + '\n';
   returnString += "   Firepower Killed:       " +
                   DisEnum::Appearance::Firepower::ToString((DisEnum::Appearance::Firepower::Enum)GetFirepower()) + '\n';
   returnString += "   Damage:                 " +
                   DisEnum::Appearance::Damage::ToString((DisEnum::Appearance::Damage::Enum)GetDamage()) + '\n';
   returnString += "   Smoking:                " +
                   DisEnum::Appearance::Smoking::ToString((DisEnum::Appearance::Smoking::Enum)GetSmoke()) + '\n';
   returnString +=
      "   Engine Smoking:         " +
      DisEnum::Appearance::EngineSmoking::ToString((DisEnum::Appearance::EngineSmoking::Enum)GetEngineSmoke()) + '\n';
   returnString +=
      "   Trailing Dust Cloud:    " +
      DisEnum::Appearance::TrailingEffects::ToString((DisEnum::Appearance::TrailingEffects::Enum)GetTrailingEffects()) +
      '\n';
   returnString += "   Primary Hatch:          " +
                   DisEnum::Appearance::Hatch::ToString((DisEnum::Appearance::Hatch::Enum)GetHatch()) + '\n';
   returnString += "   Headlights:             " +
                   DisEnum::Appearance::HeadLights::ToString((DisEnum::Appearance::HeadLights::Enum)GetHeadlights()) +
                   '\n';
   returnString += "   Taillights:             " +
                   DisEnum::Appearance::Taillights::ToString((DisEnum::Appearance::Taillights::Enum)GetTaillights()) +
                   '\n';
   returnString +=
      "   Brakelights:            " +
      DisEnum::Appearance::BrakeLights::ToString((DisEnum::Appearance::BrakeLights::Enum)GetBrakelights()) + '\n';
   returnString += "   Flaming:                " +
                   DisEnum::Appearance::Flaming::ToString((DisEnum::Appearance::Flaming::Enum)GetFlaming()) + '\n';
   returnString +=
      "   Launcher:               " +
      DisEnum::Appearance::LauncherOperational::ToString((DisEnum::Appearance::LauncherOperational::Enum)GetLauncher()) +
      '\n';
   returnString +=
      "   Camouflage Type:        " +
      DisEnum::Appearance::CamouflageType::ToString((DisEnum::Appearance::CamouflageType::Enum)GetCamouflageType()) + '\n';
   returnString +=
      "   Concealed:              " +
      DisEnum::Appearance::ConcealedPosition::ToString((DisEnum::Appearance::ConcealedPosition::Enum)GetConcealed()) +
      '\n';
   returnString += "   Frozen Status:          " +
                   DisEnum::Appearance::FrozenType::ToString((DisEnum::Appearance::FrozenType::Enum)GetFrozen()) + '\n';
   returnString += "   Powerplant:             " +
                   DisEnum::Appearance::Powerplant::ToString((DisEnum::Appearance::Powerplant::Enum)GetPowerplant()) +
                   '\n';
   returnString +=
      "   State:                  " +
      DisEnum::Appearance::EntityObjectState::ToString((DisEnum::Appearance::EntityObjectState::Enum)GetState()) + '\n';
   returnString +=
      "   Tent:                   " + DisEnum::Appearance::Tent::ToString((DisEnum::Appearance::Tent::Enum)GetTent()) + '\n';
   returnString +=
      "   Ramp:                   " + DisEnum::Appearance::Ramp::ToString((DisEnum::Appearance::Ramp::Enum)GetRamp()) + '\n';
   returnString +=
      "   Blackout lights:        " +
      DisEnum::Appearance::BlackoutLights::ToString((DisEnum::Appearance::BlackoutLights::Enum)GetBlackoutLights()) + '\n';
   returnString += "   Blackout Brake lights:  " +
                   DisEnum::Appearance::BlackoutBrakeLights::ToString(
                      (DisEnum::Appearance::BlackoutBrakeLights::Enum)GetBlackoutBrakeLights()) +
                   '\n';
   returnString +=
      "   Spot Lights:            " +
      DisEnum::Appearance::SpotSearchlights::ToString((DisEnum::Appearance::SpotSearchlights::Enum)GetSpotlights()) + '\n';
   returnString +=
      "   Interior Lights:        " +
      DisEnum::Appearance::InteriorLights::ToString((DisEnum::Appearance::InteriorLights::Enum)GetInteriorLights()) + '\n';
   returnString +=
      "   Surrendered State:      " +
      DisEnum::Appearance::SurrenderState::ToString((DisEnum::Appearance::SurrenderState::Enum)GetSurrenderState()) + '\n';
   returnString +=
      "   Masked/Cloaked:         " +
      DisEnum::Appearance::MaskedCloaked::ToString((DisEnum::Appearance::MaskedCloaked::Enum)GetMaskedCloaked()) + '\n';
   return returnString;
}

std::string DisEntityAppearance::GetAirPlatformString() const
{
   std::string returnString = "Air Platform Appearance:\n";
   returnString +=
      "   Paint Scheme:           " +
      DisEnum::Appearance::PaintScheme::ToString((DisEnum::Appearance::PaintScheme::Enum)GetPaintScheme()) + '\n';
   returnString += "   Mobility Kill:      " +
                   DisEnum::Appearance::Propulsion::ToString((DisEnum::Appearance::Propulsion::Enum)GetPropulsion()) +
                   '\n';
   returnString += "   Damage:                 " +
                   DisEnum::Appearance::Damage::ToString((DisEnum::Appearance::Damage::Enum)GetDamage()) + '\n';
   returnString += "   NVG Mode:               " +
                   DisEnum::Appearance::NVGMode::ToString((DisEnum::Appearance::NVGMode::Enum)GetNVGMode()) + '\n';
   returnString += "   Smoke:                  " +
                   DisEnum::Appearance::Smoking::ToString((DisEnum::Appearance::Smoking::Enum)GetSmoke()) + '\n';
   returnString +=
      "   Engine Smoking:         " +
      DisEnum::Appearance::EngineSmoking::ToString((DisEnum::Appearance::EngineSmoking::Enum)GetEngineSmoke()) + '\n';
   returnString +=
      "   Trailing Effect:        " +
      DisEnum::Appearance::TrailingEffects::ToString((DisEnum::Appearance::TrailingEffects::Enum)GetTrailingEffects()) +
      '\n';
   returnString += "   Canopy State:           " +
                   DisEnum::Appearance::Canopy::ToString((DisEnum::Appearance::Canopy::Enum)GetCanopy()) + '\n';
   returnString +=
      "   Landing Lights:         " +
      DisEnum::Appearance::LandingLights::ToString((DisEnum::Appearance::LandingLights::Enum)GetLandingLights()) + '\n';
   returnString +=
      "   Navigation Lights:      " +
      DisEnum::Appearance::NavigationLights::ToString((DisEnum::Appearance::NavigationLights::Enum)GetNavigationLights()) +
      '\n';
   returnString += "   Anti-Collision Lights:  " +
                   DisEnum::Appearance::AntiCollisionLights::ToString(
                      (DisEnum::Appearance::AntiCollisionLights::Enum)GetAntiCollisionLights()) +
                   '\n';
   returnString += "   Flaming Effect          " +
                   DisEnum::Appearance::Flaming::ToString((DisEnum::Appearance::Flaming::Enum)GetFlaming()) + '\n';
   returnString +=
      "   Afterburner:            " +
      DisEnum::Appearance::Afterburner::ToString((DisEnum::Appearance::Afterburner::Enum)GetAfterburner()) + '\n';
   returnString += "   Lower Anti-Collision Light: " +
                   DisEnum::Appearance::LowerAntiCollisionLight::ToString(
                      (DisEnum::Appearance::LowerAntiCollisionLight::Enum)GetLowerAntiCollisionLightsOn()) +
                   '\n';
   returnString += "   Upper Anti-Collision Light: " +
                   DisEnum::Appearance::UpperAntiCollisionLight::ToString(
                      (DisEnum::Appearance::UpperAntiCollisionLight::Enum)GetUpperAntiCollisionLightsOn()) +
                   '\n';
   returnString +=
      "   Anti-Collision Light Day/Night    = " +
      DisEnum::Appearance::AntiCollision::ToString((DisEnum::Appearance::AntiCollision::Enum)GetAntiCollisionLightsDay()) +
      '\n';
   returnString += "   Blinking:               " +
                   DisEnum::Appearance::Blinking::ToString((DisEnum::Appearance::Blinking::Enum)GetIsBlinking()) + '\n';
   returnString += "   Frozen Status:          " +
                   DisEnum::Appearance::FrozenType::ToString((DisEnum::Appearance::FrozenType::Enum)GetFrozen()) + '\n';
   returnString += "   Power Plant:            " +
                   DisEnum::Appearance::Powerplant::ToString((DisEnum::Appearance::Powerplant::Enum)GetPowerplant()) +
                   '\n';
   returnString +=
      "   State:                  " +
      DisEnum::Appearance::EntityObjectState::ToString((DisEnum::Appearance::EntityObjectState::Enum)GetState()) + '\n';
   returnString +=
      "   Formation Lights:       " +
      DisEnum::Appearance::FormationLights::ToString((DisEnum::Appearance::FormationLights::Enum)GetFormationLights()) +
      '\n';
   returnString +=
      "   Landing Gear:           " +
      DisEnum::Appearance::LandingGear::ToString((DisEnum::Appearance::LandingGear::Enum)GetLandingGear()) + '\n';
   returnString += "   Cargo Doors:            " +
                   DisEnum::Appearance::CargoDoors::ToString((DisEnum::Appearance::CargoDoors::Enum)GetCargoDoors()) +
                   '\n';
   returnString += "   Navigation/Position Brightness:" +
                   DisEnum::Appearance::NavigationPositionBrightness::ToString(
                      (DisEnum::Appearance::NavigationPositionBrightness::Enum)GetNavigationPositionBrightness()) +
                   '\n';
   returnString +=
      "   Spot/Search Light #1:   " +
      DisEnum::Appearance::SpotSearchlights::ToString((DisEnum::Appearance::SpotSearchlights::Enum)GetSpotlights()) + '\n';
   returnString +=
      "   Interior Lights:        " +
      DisEnum::Appearance::InteriorLights::ToString((DisEnum::Appearance::InteriorLights::Enum)GetInteriorLights()) + '\n';
   returnString +=
      "   Reverse Thrust:         " +
      DisEnum::Appearance::ReverseThrust::ToString((DisEnum::Appearance::ReverseThrust::Enum)GetReverseThrustEngaged()) +
      '\n';
   returnString +=
      "   Weight-on-Wheels:       " +
      DisEnum::Appearance::WeightOnWheels::ToString((DisEnum::Appearance::WeightOnWheels::Enum)GetWeightOnWheels()) + '\n';
   return returnString;
}

std::string DisEntityAppearance::GetSurfacePlatformString() const
{
   std::string returnString = "Surface Platform Appearance:\n";
   returnString +=
      "   Paint Scheme:           " +
      DisEnum::Appearance::PaintScheme::ToString((DisEnum::Appearance::PaintScheme::Enum)GetPaintScheme()) + '\n';
   returnString += "   Mobility Killed:        " +
                   DisEnum::Appearance::Mobility::ToString((DisEnum::Appearance::Mobility::Enum)GetMobility()) + '\n';
   returnString += "   Damage:                 " +
                   DisEnum::Appearance::Damage::ToString((DisEnum::Appearance::Damage::Enum)GetDamage()) + '\n';
   returnString += "   Smoke:                " +
                   DisEnum::Appearance::Smoking::ToString((DisEnum::Appearance::Smoking::Enum)GetSmoke()) + '\n';
   returnString +=
      "   Engine Smoking:         " +
      DisEnum::Appearance::EngineSmoking::ToString((DisEnum::Appearance::EngineSmoking::Enum)GetEngineSmoke()) + '\n';
   returnString +=
      "   Wake Size:              " +
      DisEnum::Appearance::TrailingEffects::ToString((DisEnum::Appearance::TrailingEffects::Enum)GetTrailingEffects()) +
      '\n';
   returnString +=
      "   Running Lights:         " +
      DisEnum::Appearance::RunningLights::ToString((DisEnum::Appearance::RunningLights::Enum)GetRunningLights()) + '\n';
   returnString += "   Flaming Effect:         " +
                   DisEnum::Appearance::Flaming::ToString((DisEnum::Appearance::Flaming::Enum)GetFlaming()) + '\n';
   returnString += "   Frozen Status:          " +
                   DisEnum::Appearance::FrozenType::ToString((DisEnum::Appearance::FrozenType::Enum)GetFrozen()) + '\n';
   returnString += "   Power Plant:             " +
                   DisEnum::Appearance::Powerplant::ToString((DisEnum::Appearance::Powerplant::Enum)GetPowerplant()) +
                   '\n';
   returnString +=
      "   State:                  " +
      DisEnum::Appearance::EntityObjectState::ToString((DisEnum::Appearance::EntityObjectState::Enum)GetState()) + '\n';
   returnString +=
      "   Spot Lights:             " +
      DisEnum::Appearance::SpotSearchlights::ToString((DisEnum::Appearance::SpotSearchlights::Enum)GetSpotlights()) + '\n';
   returnString +=
      "   Interior Lights:        " +
      DisEnum::Appearance::InteriorLights::ToString((DisEnum::Appearance::InteriorLights::Enum)GetInteriorLights()) + '\n';
   return returnString;
}

std::string DisEntityAppearance::GetSubSurfacePlatformString() const
{
   std::string returnString = "Sub Surface Platform Appearance:\n";
   returnString +=
      "   Paint Scheme:           " +
      DisEnum::Appearance::PaintScheme::ToString((DisEnum::Appearance::PaintScheme::Enum)GetPaintScheme()) + '\n';
   returnString += "   Mobility Kill:          " +
                   DisEnum::Appearance::Mobility::ToString((DisEnum::Appearance::Mobility::Enum)GetMobility()) + '\n';
   returnString += "   Damage:                 " +
                   DisEnum::Appearance::Damage::ToString((DisEnum::Appearance::Damage::Enum)GetDamage()) + '\n';
   returnString += "   Smoke:                  " +
                   DisEnum::Appearance::Smoking::ToString((DisEnum::Appearance::Smoking::Enum)GetSmoke()) + '\n';
   returnString +=
      "   Engine Smoking:         " +
      DisEnum::Appearance::EngineSmoking::ToString((DisEnum::Appearance::EngineSmoking::Enum)GetEngineSmoke()) + '\n';
   returnString += "   Hatch State:            " +
                   DisEnum::Appearance::Hatch::ToString((DisEnum::Appearance::Hatch::Enum)GetHatch()) + '\n';
   returnString +=
      "   Running Lights:         " +
      DisEnum::Appearance::RunningLights::ToString((DisEnum::Appearance::RunningLights::Enum)GetRunningLights()) + '\n';
   returnString += "   Flaming Effect:         " +
                   DisEnum::Appearance::Flaming::ToString((DisEnum::Appearance::Flaming::Enum)GetFlaming()) + '\n';
   returnString += "   Frozen Status:          " +
                   DisEnum::Appearance::FrozenType::ToString((DisEnum::Appearance::FrozenType::Enum)GetFrozen()) + '\n';
   returnString += "   Power Plant:            " +
                   DisEnum::Appearance::Powerplant::ToString((DisEnum::Appearance::Powerplant::Enum)GetPowerplant()) +
                   '\n';
   returnString +=
      "   State:                  " +
      DisEnum::Appearance::EntityObjectState::ToString((DisEnum::Appearance::EntityObjectState::Enum)GetState()) + '\n';
   return returnString;
}

std::string DisEntityAppearance::GetSpacePlatformString() const
{
   std::string returnString = "Space Platform Appearance:\n";
   returnString +=
      "   Paint Scheme:           " +
      DisEnum::Appearance::PaintScheme::ToString((DisEnum::Appearance::PaintScheme::Enum)GetPaintScheme()) + '\n';
   returnString += "   Mobility Kill:          " +
                   DisEnum::Appearance::Mobility::ToString((DisEnum::Appearance::Mobility::Enum)GetMobility()) + '\n';
   returnString += "   Damage:                 " +
                   DisEnum::Appearance::Damage::ToString((DisEnum::Appearance::Damage::Enum)GetDamage()) + '\n';
   returnString += "   Smoke:                  " +
                   DisEnum::Appearance::Smoking::ToString((DisEnum::Appearance::Smoking::Enum)GetSmoke()) + '\n';
   returnString +=
      "   Engine Smoke:           " +
      DisEnum::Appearance::EngineSmoking::ToString((DisEnum::Appearance::EngineSmoking::Enum)GetEngineSmoke()) + '\n';
   returnString += "   Flaming Effect:         " +
                   DisEnum::Appearance::Flaming::ToString((DisEnum::Appearance::Flaming::Enum)GetFlaming()) + '\n';
   returnString += "   Frozen Status:          " +
                   DisEnum::Appearance::FrozenType::ToString((DisEnum::Appearance::FrozenType::Enum)GetFrozen()) + '\n';
   returnString += "   Power Plant:            " +
                   DisEnum::Appearance::Powerplant::ToString((DisEnum::Appearance::Powerplant::Enum)GetPowerplant()) +
                   '\n';
   returnString +=
      "   State:                  " +
      DisEnum::Appearance::EntityObjectState::ToString((DisEnum::Appearance::EntityObjectState::Enum)GetState()) + '\n';
   return returnString;
}

void DisEntityAppearance::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisEntityAppearance::GetMemberData(GenI& aGenI)
{
   aGenI >> mAppearanceRecord;
}

void DisEntityAppearance::Put(GenO& aGenO) const
{
   aGenO << mAppearanceRecord;
}

bool DisEntityAppearance::IsValid() const
{
   return true;
}

bool DisEntityAppearance::operator==(const DisEntityAppearance& aRhs) const
{
   return mAppearanceRecord == aRhs.mAppearanceRecord;
}

// The right-most integer in the expressions below are the number of bit shifts.
unsigned int DisEntityAppearance::GetPaintScheme() const
{
   return ((mAppearanceRecord & cPAINT_SCHEME_MASK) >> 0);
}
unsigned int DisEntityAppearance::GetMobility() const
{
   return ((mAppearanceRecord & cMOBILITY_KILL_MASK) >> 1);
}
unsigned int DisEntityAppearance::GetPropulsion() const
{
   return ((mAppearanceRecord & cPROPULSION_MASK) >> 1);
}
unsigned int DisEntityAppearance::GetFirepower() const
{
   return ((mAppearanceRecord & cFIREPOWER_MASK) >> 2);
}
unsigned int DisEntityAppearance::GetNVGMode() const
{
   return ((mAppearanceRecord & cNVGMODE_MASK) >> 2);
}
unsigned int DisEntityAppearance::GetDamage() const
{
   return ((mAppearanceRecord & cDAMAGE_LEVEL_MASK) >> 3);
}
unsigned int DisEntityAppearance::GetSmoke() const
{
   return ((mAppearanceRecord & cSMOKE_MASK) >> 5);
}
unsigned int DisEntityAppearance::GetEngineSmoke() const
{
   return ((mAppearanceRecord & cENGINE_SMOKE_MASK) >> 6);
}
unsigned int DisEntityAppearance::GetTrailingEffects() const
{
   return ((mAppearanceRecord & cTRAILING_EFFECT_MASK) >> 7);
}
unsigned int DisEntityAppearance::GetHatch() const
{
   return ((mAppearanceRecord & cHATCH_MASK) >> 9);
}
unsigned int DisEntityAppearance::GetCanopy() const
{
   return ((mAppearanceRecord & cCANOPY_MASK) >> 9);
}
unsigned int DisEntityAppearance::GetHeadlights() const
{
   return ((mAppearanceRecord & cHEADLIGHT_MASK) >> 12);
}
unsigned int DisEntityAppearance::GetLandingLights() const
{
   return ((mAppearanceRecord & cLANDING_LIGHT_MASK) >> 12);
}
unsigned int DisEntityAppearance::GetRunningLights() const
{
   return ((mAppearanceRecord & cRUNNING_LIGHT_MASK) >> 12);
}
unsigned int DisEntityAppearance::GetTaillights() const
{
   return ((mAppearanceRecord & cTAILLIGHT_MASK) >> 13);
}
unsigned int DisEntityAppearance::GetNavigationLights() const
{
   return ((mAppearanceRecord & cNAVIGATION_LIGHT_MASK) >> 13);
}
unsigned int DisEntityAppearance::GetAntiCollisionLights() const
{
   return ((mAppearanceRecord & cANTI_COLLISION_LIGHT_MASK) >> 14);
}
unsigned int DisEntityAppearance::GetBrakelights() const
{
   return ((mAppearanceRecord & cBRAKELIGHT_MASK) >> 14);
}
unsigned int DisEntityAppearance::GetFlaming() const
{
   return ((mAppearanceRecord & cFLAMING_MASK) >> 15);
}
unsigned int DisEntityAppearance::GetLauncher() const
{
   return ((mAppearanceRecord & cLAUNCHER_MASK) >> 16);
}
unsigned int DisEntityAppearance::GetAfterburner() const
{
   return ((mAppearanceRecord & cAFTERBURNER_MASK) >> 16);
}
unsigned int DisEntityAppearance::GetCamouflageType() const
{
   return ((mAppearanceRecord & cCAMOUFLAGE_MASK) >> 17);
}
unsigned int DisEntityAppearance::GetLowerAntiCollisionLightsOn() const
{
   return ((mAppearanceRecord & cLOWER_ANTI_COLLISION_MASK) >> 17);
}
unsigned int DisEntityAppearance::GetUpperAntiCollisionLightsOn() const
{
   return ((mAppearanceRecord & cUPPER_ANTI_COLLISION_MASK) >> 18);
}
unsigned int DisEntityAppearance::GetAntiCollisionLightsDay() const
{
   return ((mAppearanceRecord & cANTI_COLLISION_DAY_MASK) >> 19);
}
unsigned int DisEntityAppearance::GetConcealed() const
{
   return ((mAppearanceRecord & cCONCEALED_MASK) >> 19);
}
unsigned int DisEntityAppearance::GetIsBlinking() const
{
   return ((mAppearanceRecord & cIS_BLINKING_MASK) >> 20);
}
unsigned int DisEntityAppearance::GetFrozen() const
{
   return ((mAppearanceRecord & cFROZEN_MASK) >> 21);
}
unsigned int DisEntityAppearance::GetPowerplant() const
{
   return ((mAppearanceRecord & cPOWERPLANT_MASK) >> 22);
}
unsigned int DisEntityAppearance::GetState() const
{
   return ((mAppearanceRecord & cDEACTIVATED_MASK) >> 23);
}
unsigned int DisEntityAppearance::GetTent() const
{
   return ((mAppearanceRecord & cTENT_MASK) >> 24);
}
unsigned int DisEntityAppearance::GetFormationLights() const
{
   return ((mAppearanceRecord & cFORMATION_LIGHT_MASK) >> 24);
}
unsigned int DisEntityAppearance::GetRamp() const
{
   return ((mAppearanceRecord & cRAMP_MASK) >> 25);
}
unsigned int DisEntityAppearance::GetLandingGear() const
{
   return ((mAppearanceRecord & cLANDING_GEAR_MASK) >> 25);
}
unsigned int DisEntityAppearance::GetBlackoutLights() const
{
   return ((mAppearanceRecord & cBLACKOUT_LIGHTS_MASK) >> 26);
}
unsigned int DisEntityAppearance::GetCargoDoors() const
{
   return ((mAppearanceRecord & cCARGO_DOORS_MASK) >> 26);
}
unsigned int DisEntityAppearance::GetBlackoutBrakeLights() const
{
   return ((mAppearanceRecord & cBLACKOUT_BRAKES_MASK) >> 27);
}
unsigned int DisEntityAppearance::GetNavigationPositionBrightness() const
{
   return ((mAppearanceRecord & cNAVIGATION_BRIGHTNESS_MASK) >> 27);
}
unsigned int DisEntityAppearance::GetSpotlights() const
{
   return ((mAppearanceRecord & cSPOTLIGHTS_MASK) >> 28);
}
unsigned int DisEntityAppearance::GetInteriorLights() const
{
   return ((mAppearanceRecord & cINTERIOR_LIGHTS_MASK) >> 29);
}
unsigned int DisEntityAppearance::GetReverseThrustEngaged() const
{
   return ((mAppearanceRecord & cREVERSE_THRUST_MASK) >> 30);
}
unsigned int DisEntityAppearance::GetSurrenderState() const
{
   return ((mAppearanceRecord & cSURRENDER_STATE_MASK) >> 30);
}
unsigned int DisEntityAppearance::GetMaskedCloaked() const
{
   return ((mAppearanceRecord & cMASKED_CLOAKED_MASK) >> 31);
}
unsigned int DisEntityAppearance::GetWeightOnWheels() const
{
   return ((mAppearanceRecord & cWEIGHT_ON_WHEELS_MASK) >> 31);
}

void DisEntityAppearance::SetPaintScheme(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 0) & cPAINT_SCHEME_MASK) | (mAppearanceRecord & ~cPAINT_SCHEME_MASK);
}
void DisEntityAppearance::SetMobility(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 1) & cMOBILITY_KILL_MASK) | (mAppearanceRecord & ~cMOBILITY_KILL_MASK);
};
void DisEntityAppearance::SetPropulsion(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 1) & cPROPULSION_MASK) | (mAppearanceRecord & ~cPROPULSION_MASK);
};
void DisEntityAppearance::SetFirepower(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 2) & cFIREPOWER_MASK) | (mAppearanceRecord & ~cFIREPOWER_MASK);
};
void DisEntityAppearance::SetNVGMode(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 2) & cNVGMODE_MASK) | (mAppearanceRecord & ~cNVGMODE_MASK);
};
void DisEntityAppearance::SetDamage(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 3) & cDAMAGE_LEVEL_MASK) | (mAppearanceRecord & ~cDAMAGE_LEVEL_MASK);
};
void DisEntityAppearance::SetSmoke(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 5) & cSMOKE_MASK) | (mAppearanceRecord & ~cSMOKE_MASK);
};
void DisEntityAppearance::SetEngineSmoke(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 6) & cENGINE_SMOKE_MASK) | (mAppearanceRecord & ~cENGINE_SMOKE_MASK);
};
void DisEntityAppearance::SetTrailingEffects(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 7) & cTRAILING_EFFECT_MASK) | (mAppearanceRecord & ~cTRAILING_EFFECT_MASK);
};
void DisEntityAppearance::SetHatch(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 9) & cHATCH_MASK) | (mAppearanceRecord & ~cHATCH_MASK);
};
void DisEntityAppearance::SetCanopy(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 9) & cCANOPY_MASK) | (mAppearanceRecord & ~cCANOPY_MASK);
};
void DisEntityAppearance::SetHeadlights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 12) & cHEADLIGHT_MASK) | (mAppearanceRecord & ~cHEADLIGHT_MASK);
};
void DisEntityAppearance::SetLandingLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 12) & cLANDING_LIGHT_MASK) | (mAppearanceRecord & ~cLANDING_LIGHT_MASK);
};
void DisEntityAppearance::SetRunningLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 12) & cRUNNING_LIGHT_MASK) | (mAppearanceRecord & ~cRUNNING_LIGHT_MASK);
};
void DisEntityAppearance::SetTaillights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 13) & cTAILLIGHT_MASK) | (mAppearanceRecord & ~cTAILLIGHT_MASK);
};
void DisEntityAppearance::SetNavigationLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 13) & cNAVIGATION_LIGHT_MASK) | (mAppearanceRecord & ~cNAVIGATION_LIGHT_MASK);
};
void DisEntityAppearance::SetAntiCollisionLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 14) & cANTI_COLLISION_LIGHT_MASK) | (mAppearanceRecord & ~cANTI_COLLISION_LIGHT_MASK);
};
void DisEntityAppearance::SetBrakelights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 14) & cBRAKELIGHT_MASK) | (mAppearanceRecord & ~cBRAKELIGHT_MASK);
};
void DisEntityAppearance::SetFlaming(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 15) & cFLAMING_MASK) | (mAppearanceRecord & ~cFLAMING_MASK);
};
void DisEntityAppearance::SetLauncher(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 16) & cLAUNCHER_MASK) | (mAppearanceRecord & ~cLAUNCHER_MASK);
};
void DisEntityAppearance::SetAfterburner(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 16) & cAFTERBURNER_MASK) | (mAppearanceRecord & ~cAFTERBURNER_MASK);
};
void DisEntityAppearance::SetCamouflageType(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 17) & cCAMOUFLAGE_MASK) | (mAppearanceRecord & ~cCAMOUFLAGE_MASK);
};
void DisEntityAppearance::SetLowerAntiCollisionLightsOn(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 17) & cLOWER_ANTI_COLLISION_MASK) | (mAppearanceRecord & ~cLOWER_ANTI_COLLISION_MASK);
};
void DisEntityAppearance::SetUpperAntiCollisionLightsOn(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 18) & cUPPER_ANTI_COLLISION_MASK) | (mAppearanceRecord & ~cUPPER_ANTI_COLLISION_MASK);
};
void DisEntityAppearance::SetAntiCollisionLightsDay(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 19) & cANTI_COLLISION_DAY_MASK) | (mAppearanceRecord & ~cANTI_COLLISION_DAY_MASK);
};
void DisEntityAppearance::SetConcealed(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 19) & cCONCEALED_MASK) | (mAppearanceRecord & ~cCONCEALED_MASK);
};
void DisEntityAppearance::SetIsBlinking(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 20) & cIS_BLINKING_MASK) | (mAppearanceRecord & ~cIS_BLINKING_MASK);
};
void DisEntityAppearance::SetFrozen(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 21) & cFROZEN_MASK) | (mAppearanceRecord & ~cFROZEN_MASK);
};
void DisEntityAppearance::SetPowerplant(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 22) & cPOWERPLANT_MASK) | (mAppearanceRecord & ~cPOWERPLANT_MASK);
};
void DisEntityAppearance::SetState(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 23) & cDEACTIVATED_MASK) | (mAppearanceRecord & ~cDEACTIVATED_MASK);
};
void DisEntityAppearance::SetTent(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 24) & cTENT_MASK) | (mAppearanceRecord & ~cTENT_MASK);
};
void DisEntityAppearance::SetFormationLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 24) & cFORMATION_LIGHT_MASK) | (mAppearanceRecord & ~cFORMATION_LIGHT_MASK);
};
void DisEntityAppearance::SetRamp(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 25) & cRAMP_MASK) | (mAppearanceRecord & ~cRAMP_MASK);
};
void DisEntityAppearance::SetLandingGear(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 25) & cLANDING_GEAR_MASK) | (mAppearanceRecord & ~cLANDING_GEAR_MASK);
}
void DisEntityAppearance::SetBlackoutLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 26) & cBLACKOUT_LIGHTS_MASK) | (mAppearanceRecord & ~cBLACKOUT_LIGHTS_MASK);
};
void DisEntityAppearance::SetCargoDoors(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 26) & cCARGO_DOORS_MASK) | (mAppearanceRecord & ~cCARGO_DOORS_MASK);
}
void DisEntityAppearance::SetBlackoutBrakeLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 27) & cBLACKOUT_BRAKES_MASK) | (mAppearanceRecord & ~cBLACKOUT_BRAKES_MASK);
};
void DisEntityAppearance::SetNavigationPositionBrightness(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 27) & cNAVIGATION_BRIGHTNESS_MASK) | (mAppearanceRecord & ~cNAVIGATION_BRIGHTNESS_MASK);
};
void DisEntityAppearance::SetSpotlights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 28) & cSPOTLIGHTS_MASK) | (mAppearanceRecord & ~cSPOTLIGHTS_MASK);
};
void DisEntityAppearance::SetInteriorLights(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 29) & cINTERIOR_LIGHTS_MASK) | (mAppearanceRecord & ~cINTERIOR_LIGHTS_MASK);
};
void DisEntityAppearance::SetReverseThrustEngaged(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 30) & cREVERSE_THRUST_MASK) | (mAppearanceRecord & ~cREVERSE_THRUST_MASK);
};
void DisEntityAppearance::SetSurrenderState(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 30) & cSURRENDER_STATE_MASK) | (mAppearanceRecord & ~cSURRENDER_STATE_MASK);
};
void DisEntityAppearance::SetMaskedCloaked(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 31) & cMASKED_CLOAKED_MASK) | (mAppearanceRecord & ~cMASKED_CLOAKED_MASK);
};
void DisEntityAppearance::SetWeightOnWheels(unsigned int aValue)
{
   mAppearanceRecord = ((aValue << 31) & cWEIGHT_ON_WHEELS_MASK) | (mAppearanceRecord & ~cWEIGHT_ON_WHEELS_MASK);
};
