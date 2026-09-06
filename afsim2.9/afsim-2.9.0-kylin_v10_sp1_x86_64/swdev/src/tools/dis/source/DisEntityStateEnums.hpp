// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef VWC_DIS_DIS_ENTITY_STATE_ENUMS_H
#define VWC_DIS_DIS_ENTITY_STATE_ENUMS_H

namespace DisEntityStateEnums
{
enum ForceID
{
   OtherForce  = 0,
   Friendly    = 1,
   Opposing    = 2,
   Neutral     = 3,
   Friendly_2  = 4,
   Opposing_2  = 5,
   Neutral_2   = 6,
   Friendly_3  = 7,
   Opposing_3  = 8,
   Neutral_3   = 9,
   Friendly_4  = 10,
   Opposing_4  = 11,
   Neutral_4   = 12,
   Friendly_5  = 13,
   Opposing_5  = 14,
   Neutral_5   = 15,
   Friendly_6  = 16,
   Opposing_6  = 17,
   Neutral_6   = 18,
   Friendly_7  = 19,
   Opposing_7  = 20,
   Neutral_7   = 21,
   Friendly_8  = 22,
   Opposing_8  = 23,
   Neutral_8   = 24,
   Friendly_9  = 25,
   Opposing_9  = 26,
   Neutral_9   = 27,
   Friendly_10 = 28,
   Opposing_10 = 29,
   Neutral_10  = 30
};

struct LandPlatformAppearance
{
#ifdef _MIPS_ISA                       // Swap Bit Ordering on SGI
   unsigned bit31_unused : 1;          // Bit 31
   unsigned surrenderState : 1;        // Bit 30
   unsigned interiorLightsOn : 1;      // Bit 29
   unsigned spotLightsOn : 1;          // Bit 28
   unsigned blackoutBreakLightsOn : 1; // Bit 27
   unsigned blackoutLightsOn : 1;      // Bit 26
   unsigned rampDown : 1;              // Bit 25
   unsigned tentExtended : 1;          // Bit 24
   unsigned deactivated : 1;           // Bit 23
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned frozenStatus : 1;          // Bit 21
   unsigned bit20_unused : 1;          // Bit 20
   unsigned concealed : 1;             // Bit 19
   unsigned comouflageType : 2;        // Bits 17-18
   unsigned launcherRaised : 1;        // Bit 16
   unsigned isFlaming : 1;             // Bit 15
   unsigned brakeLightsOn : 1;         // Bit 14
   unsigned tailLightsOn : 1;          // Bit 13
   unsigned headLightsOn : 1;          // Bit 12
   unsigned hatchState : 3;            // Bits 9-11
   unsigned trailingEffects : 2;       // Bits 7-8
   unsigned smoke : 2;                 // Bits 5-6
   unsigned damage : 2;                // Bits 3-4
   unsigned firePowerKill : 1;         // Bit 2
   unsigned mobilityKill : 1;          // Bit 1
   unsigned paintScheme : 1;           // Bit 0
#else                                  // Linux & SGI
   unsigned paintScheme : 1;           // Bit 0
   unsigned mobilityKill : 1;          // Bit 1
   unsigned firePowerKill : 1;         // Bit 2
   unsigned damage : 2;                // Bits 3-4
   unsigned smoke : 2;                 // Bits 5-6
   unsigned trailingEffects : 2;       // Bits 7-8
   unsigned hatchState : 3;            // Bits 9-11
   unsigned headLightsOn : 1;          // Bit 12
   unsigned tailLightsOn : 1;          // Bit 13
   unsigned brakeLightsOn : 1;         // Bit 14
   unsigned isFlaming : 1;             // Bit 15
   unsigned launcherRaised : 1;        // Bit 16
   unsigned comouflageType : 2;        // Bits 17-18
   unsigned concealed : 1;             // Bit 19
   unsigned bit20_unused : 1;          // Bit 20
   unsigned frozenStatus : 1;          // Bit 21
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned deactivated : 1;           // Bit 23
   unsigned tentExtended : 1;          // Bit 24
   unsigned rampDown : 1;              // Bit 25
   unsigned blackoutLightsOn : 1;      // Bit 26
   unsigned blackoutBreakLightsOn : 1; // Bit 27
   unsigned spotLightsOn : 1;          // Bit 28
   unsigned interiorLightsOn : 1;      // Bit 29
   unsigned surrenderState : 1;        // Bit 30
   unsigned bit31_unused : 1;          // Bit 31
#endif                                 // _MIPS_ISA
};                                     // struct LandPlatformAppearance

struct AirPlatformAppearance
{
#ifdef _MIPS_ISA                       // Swap Bit Ordering on SGI
   unsigned bits30_31_unused : 2;      // Bits 30-31
   unsigned interiorLightsOn : 1;      // Bit 29
   unsigned spotLightsOn : 1;          // Bit 28
   unsigned bits25_27_unused : 3;      // Bits 25-27
   unsigned formationLights : 1;       // Bit 24
   unsigned deactivated : 1;           // Bit 23
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned frozenStatus : 1;          // Bit 21
   unsigned sig_db_index : 4;          // Bits 17-20
   unsigned afterburnerOn : 1;         // Bit 16
   unsigned isFlaming : 1;             // Bit 15
   unsigned antiCollisionLightsOn : 1; // Bit 14
   unsigned navigationLightsOn : 1;    // Bit 13
   unsigned landingLightsOn : 1;       // Bit 12
   unsigned canopyState : 3;           // Bits 9-11
   unsigned trailingEffects : 2;       // Bits 7-8
   unsigned smoke : 2;                 // Bits 5-6
   unsigned damage : 2;                // Bits 3-4
   unsigned bit2_unused : 1;           // Bit 2
   unsigned propulsionKill : 1;        // Bit 1
   unsigned paintScheme : 1;           // Bit 0
#else                                  // Linux & Windows
   unsigned paintScheme : 1;           // Bit 0
   unsigned propulsionKill : 1;        // Bit 1
   unsigned bit2_unused : 1;           // Bit 2
   unsigned damage : 2;                // Bits 3-4
   unsigned smoke : 2;                 // Bits 5-6
   unsigned trailingEffects : 2;       // Bits 7-8
   unsigned canopyState : 3;           // Bits 9-11
   unsigned landingLightsOn : 1;       // Bit 12
   unsigned navigationLightsOn : 1;    // Bit 13
   unsigned antiCollisionLightsOn : 1; // Bit 14
   unsigned isFlaming : 1;             // Bit 15
   unsigned afterburnerOn : 1;         // Bit 16
   unsigned sig_db_index : 4;          // Bits 17-20
   unsigned frozenStatus : 1;          // Bit 21
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned deactivated : 1;           // Bit 23
   unsigned formationLights : 1;       // Bit 24
   unsigned bits25_27_unused : 3;      // Bits 25-27
   unsigned spotLightsOn : 1;          // Bit 28
   unsigned interiorLightsOn : 1;      // Bit 29
   unsigned bits30_31_unused : 2;      // Bits 30-31
#endif                                 // _MIPS_ISA
};                                     // struct AirPlatformAppearance

struct SurfacePlatformAppearance
{
#ifdef _MIPS_ISA                  // Swap Bit Ordering on SGI
   unsigned bits30_31_unused : 2; // Bits 30-31
   unsigned interiorLightsOn : 1; // Bit 29
   unsigned spotLightsOn : 1;     // Bit 28
   unsigned bits24_27_unused : 4; // Bits 24-27
   unsigned deactivated : 1;      // Bit 23
   unsigned powerPlantOn : 1;     // Bit 22
   unsigned frozenStatus : 1;     // Bit 21
   unsigned sig_db_index : 4;     // Bits 17-20
   unsigned bit16 : 1;            // Bit 16
   unsigned isFlaming : 1;        // Bit 15
   unsigned bits13_14_unused : 2; // Bits 13-14
   unsigned runningLightsOn : 1;  // Bit 12
   unsigned bits9_11_unused : 3;  // Bits 9-11
   unsigned trailingEffects : 2;  // Bits 7-8
   unsigned smoke : 2;            // Bits 5-6
   unsigned damage : 2;           // Bits 3-4
   unsigned bit2_unused : 1;      // Bit 2
   unsigned mobilityKill : 1;     // Bit 1
   unsigned paintScheme : 1;      // Bit 0
#else                             // Linux & Windows
   unsigned paintScheme : 1;           // Bit 0
   unsigned mobilityKill : 1;          // Bit 1
   unsigned bit2_unused : 1;           // Bit 2
   unsigned damage : 2;                // Bits 3-4
   unsigned smoke : 2;                 // Bits 5-6
   unsigned trailingEffects : 2;       // Bits 7-8
   unsigned bits9_11_unused : 3;       // Bits 9-11
   unsigned runningLightsOn : 1;       // Bit 12
   unsigned bits13_14_unused : 2;      // Bits 13-14
   unsigned isFlaming : 1;             // Bit 15
   unsigned bit16 : 1;                 // Bit 16
   unsigned sig_db_index : 4;          // Bits 17-20
   unsigned frozenStatus : 1;          // Bit 21
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned deactivated : 1;           // Bit 23
   unsigned bits24_27_unused : 4;      // Bits 24-27
   unsigned spotLightsOn : 1;          // Bit 28
   unsigned interiorLightsOn : 1;      // Bit 29
   unsigned bits30_31_unused : 2;      // Bits 30-31
#endif                            // _MIPS_ISA
};                                // struct SurfacePlatformAppearance

struct SubSurfacePlatformAppearance
{
#ifdef _MIPS_ISA                  // Swap Bit Ordering on SGI
   unsigned bits24_31_unused : 8; // Bits 24-31
   unsigned deactivated : 1;      // Bit 23
   unsigned powerPlantOn : 1;     // Bit 22
   unsigned frozenStatus : 1;     // Bit 21
   unsigned sig_db_index : 4;     // Bits 17-20
   unsigned bit16 : 1;            // Bit 16
   unsigned isFlaming : 1;        // Bit 15
   unsigned bits13_14_unused : 2; // Bits 13-14
   unsigned runningLightsOn : 1;  // Bit 12
   unsigned hatchState : 3;       // Bits 9-11
   unsigned bits7_8_unused : 2;   // Bits 7-8
   unsigned smoke : 2;            // Bits 5-6
   unsigned damage : 2;           // Bits 3-4
   unsigned bit2_unused : 1;      // Bit 2
   unsigned mobilityKill : 1;     // Bit 1
   unsigned paintScheme : 1;      // Bit 0
#else                             // Linux & Windows
   unsigned paintScheme : 1;           // Bit 0
   unsigned mobilityKill : 1;          // Bit 1
   unsigned bit2_unused : 1;           // Bit 2
   unsigned damage : 2;                // Bits 3-4
   unsigned smoke : 2;                 // Bits 5-6
   unsigned bits7_8_unused : 2;        // Bits 7-8
   unsigned hatchState : 3;            // Bits 9-11
   unsigned runningLightsOn : 1;       // Bit 12
   unsigned bits13_14_unused : 2;      // Bits 13-14
   unsigned isFlaming : 1;             // Bit 15
   unsigned bit16 : 1;                 // Bit 16
   unsigned sig_db_index : 4;          // Bits 17-20
   unsigned frozenStatus : 1;          // Bit 21
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned deactivated : 1;           // Bit 23
   unsigned bits24_31_unused : 8;      // Bits 24-31
#endif                            // _MIPS_ISA
};                                // struct SubSurfacePlatformAppearance

struct SpacePlatformAppearance
{
#ifdef _MIPS_ISA                  // Swap Bit Ordering on SGI
   unsigned bits24_31_unused : 8; // Bits 24-31
   unsigned deactivated : 1;      // Bit 23
   unsigned powerPlantOn : 1;     // Bit 22
   unsigned frozenStatus : 1;     // Bit 21
   unsigned sig_db_index : 4;     // Bits 16-20
   unsigned bit16 : 1;            // Bit 16
   unsigned isFlaming : 1;        // Bit 15
   unsigned bits7_14_unused : 8;  // Bits 7-14
   unsigned smoke : 2;            // Bits 5-6
   unsigned damage : 2;           // Bits 3-4
   unsigned bit2_unused : 1;      // Bit 2
   unsigned mobilityKill : 1;     // Bit 1
   unsigned paintScheme : 1;      // Bit 0
#else                             // Linux & Windows
   unsigned paintScheme : 1;           // Bit 0
   unsigned mobilityKill : 1;          // Bit 1
   unsigned bit2_unused : 1;           // Bit 2
   unsigned damage : 2;                // Bits 3-4
   unsigned smoke : 2;                 // Bits 5-6
   unsigned bits7_14_unused : 8;       // Bits 7-14
   unsigned isFlaming : 1;             // Bit 15
   unsigned bit16 : 1;                 // Bit 16
   unsigned sig_db_index : 4;          // Bits 17-20
   unsigned frozenStatus : 1;          // Bit 21
   unsigned powerPlantOn : 1;          // Bit 22
   unsigned deactivated : 1;           // Bit 23
   unsigned bits24_31_unused : 8;      // Bits 24-31
#endif
}; // struct SpacePlatformAppearance

enum Capabilities
{
   OtherCapability  = 0,
   AmmunitionSupply = 1,
   FuelSupply       = 2,
   RecoveryService  = 4,
   RepairService    = 8
};

} // namespace DisEntityStateEnums

#endif
