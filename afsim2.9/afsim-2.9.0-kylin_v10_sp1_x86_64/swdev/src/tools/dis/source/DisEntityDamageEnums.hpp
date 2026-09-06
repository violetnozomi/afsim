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

#ifndef DISENTITYDAMAGEDEENUMS_HPP
#define DISENTITYDAMAGEDEENUMS_HPP

namespace DisEnum
{
namespace EntityDamage
{
namespace ComponentIdentification
{
enum
{
   EntityStructure            = 1,
   ControlSystem              = 2,
   ControlSurface             = 3,
   EngineOrPropulsionSystem   = 4,
   CrewMember                 = 5,
   Fuse                       = 6,
   AcquisitionSensor          = 7,
   TrackingSensor             = 8,
   FuelTankOrSolidRocketMotor = 9
};
}

namespace ComponentDamageStatus
{
enum
{
   NoDamage     = 1,
   MinorDamage  = 2,
   MediumDamage = 3,
   MajorDamage  = 4,
   Destroyed    = 5
};
}

namespace ComponentVisualDamageAppearance
{
enum
{
   FireMask          = 0x00000001, // Bit  0
   SmokeMask         = 0x00000006, // Bits 1-2
   SurfaceDamageMask = 0x00000018  // Bits 3-4
};
enum // Fire
{
   NoFire      = 0,
   FirePresent = 1
};
enum // Smoke
{
   NoSmoke       = 0,
   LightSmoke    = 2,
   ModerateSmoke = 4,
   HeavySmoke    = 6
};
enum // Surface Damage
{
   NormalAppearance = 0,
   LightCharring    = 8,
   HeavyCharring    = 16,
   Holes            = 24 // one or more holes burned through surface
};
} // namespace ComponentVisualDamageAppearance

namespace VisualSmokeColor
{
enum
{
   NoSmoke    = 0,
   WhiteSmoke = 1,
   GreySmoke  = 2,
   BlackSmoke = 3
};
}
} // namespace EntityDamage
} // namespace DisEnum

#endif
