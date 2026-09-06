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

#ifndef DISAPPEARANCEENUMS_HPP
#define DISAPPEARANCEENUMS_HPP

#include "dis_export.h"

#include <string>

#include "DisTypes.hpp"

/**!
 * For the DisUint32 bit values that each of these
 * enums apply to, see DisEntityAppearance.cpp.
 */
namespace DisEnum
{
namespace Appearance
{
namespace PaintScheme
{
enum Enum
{
   Uniform    = 0,
   Camouflage = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace PaintScheme

namespace Mobility
{
enum Enum
{
   NoKill = 0,
   Kill   = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Mobility

namespace Propulsion
{
enum Enum
{
   NoKill = 0,
   Kill   = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Propulsion

namespace Firepower
{
enum Enum
{
   NoKill = 0,
   Kill   = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Firepower

namespace Damage
{
enum Enum
{
   NoDamage  = 0,
   Slight    = 1,
   Moderate  = 2,
   Destroyed = 3
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Damage

namespace Smoking
{
enum Enum
{
   NotSmoking = 0,
   Smoking    = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Smoking

namespace EngineSmoking
{
enum Enum
{
   NotSmoking = 0,
   Smoking    = 1,
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace EngineSmoking

namespace TrailingEffects
{
enum Enum
{
   None   = 0,
   Small  = 1,
   Medium = 2,
   Large  = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace TrailingEffects

namespace Hatch
{
enum Enum
{
   NotApplicable       = 0,
   Closed              = 1,
   Popped              = 2,
   PoppedPersonVisible = 3,
   Open                = 4,
   OpenPersonVisible   = 5
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Hatch

namespace LauncherOperational
{
enum Enum
{
   NotRaisedNotOperational = 0,
   RaisedOperational       = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LauncherOperational

namespace CamouflageType
{
enum Enum
{
   Desert = 0,
   Winter = 1,
   Forest = 2,
   Other  = 3
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace CamouflageType

namespace ConcealedPosition
{
enum Enum
{
   NotConcealed              = 0,
   PreparedConcealedPosition = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace ConcealedPosition

namespace EntityObjectState
{
enum Enum
{
   Active      = 0,
   Deactivated = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace EntityObjectState

namespace Canopy
{
enum Enum
{
   NotApplicable              = 0,
   SingleClosed               = 1,
   FrontRearOrLeftRightClosed = 2,
   FrontOrLeftOpen            = 2,
   SingleOpen                 = 4,
   RearRightOpen              = 5,
   FrontRearLeftRightOpen     = 6
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Canopy

namespace SubsurfaceHatch
{
enum Enum
{
   NotApplicable = 0,
   Closed        = 1,
   Open          = 4
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace SubsurfaceHatch

namespace LifeFormHealth
{
enum Enum
{
   NoInjury       = 0,
   SlightInjury   = 1,
   ModerateInjury = 2,
   FatalInjury    = 3
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LifeFormHealth

namespace LifeFormCompliance
{
enum Enum
{
   NotSpecified       = 0,
   Detained           = 1,
   Surrender          = 2,
   UsingFists         = 3,
   VerbalAbuse1       = 4,
   VerbalAbuse2       = 5,
   VerbalAbuse3       = 6,
   PassiveResistance1 = 7,
   PassiveResistance2 = 8,
   PassiveResistance3 = 9,
   NonLethalWeapon1   = 10,
   NonLethalWeapon2   = 11,
   NonLethalWeapon3   = 12,
   NonLethalWeapon4   = 13,
   NonLethalWeapon5   = 14,
   NonLethalWeapon6   = 15,
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LifeFormCompliance

namespace LifeFormPosture
{
enum Enum
{
   NotSpecified         = 0,
   UprightStandingStill = 1,
   UprightWalking       = 2,
   UprightRunning       = 3,
   Kneeling             = 4,
   Prone                = 5,
   Crawling             = 6,
   Swimming             = 7,
   Parachuting          = 8,
   Jumping              = 9,
   Sitting              = 10,
   Squatting            = 11,
   Crouching            = 12,
   Wading               = 13,
   Surrender            = 14,
   Detained             = 15
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LifeFormPosture

namespace LifeFormWeapon
{
enum Enum
{
   NotPresent          = 0,
   Stowed              = 1,
   DeployedActive      = 2,
   FiringPositionInUse = 3
};
}

namespace ConcealedMovement
{
enum Enum
{
   Open               = 0,
   RushesBetweenCover = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace ConcealedMovement

namespace EnvironmentalDensity
{
enum Enum
{
   Clear     = 0,
   Hazy      = 1,
   Dense     = 2,
   VeryDense = 3,
   Opaque    = 4
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace EnvironmentalDensity

namespace AntiCollision
{
enum Enum
{
   Day   = 0,
   Night = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace AntiCollision
namespace NavigationPositionBrightness
{
enum Enum
{
   Dim    = 0,
   Bright = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace NavigationPositionBrightness
namespace SupplyDeployed
{
enum Enum
{
   NotApplicable  = 0,
   Stowed         = 1,
   Deployed       = 2,
   DeployedActive = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace SupplyDeployed

namespace NVGMode
{
enum Enum
{
   OvertLighting  = 0,
   CovertLighting = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace NVGMode

namespace Parachute
{
enum Enum
{
   None                  = 0,
   Deployed              = 1,
   Collapsed             = 2,
   MalfunctionOrStreamer = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Parachute

namespace FlareSmokeColor
{
enum Enum
{
   White = 0,
   Red   = 1,
   Green = 2,
   IR    = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace FlareSmokeColor

namespace FlareSmoke
{
enum Enum
{
   NotIgnited = 0,
   Burning    = 1,
   BurnedOut  = 2
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace FlareSmoke

namespace SpotChaff
{
enum Enum
{
   None        = 0,
   Deployed    = 1,
   Malfunction = 2
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace SpotChaff

namespace IEDPresent
{
enum Enum
{
   None             = 0,
   Visible          = 1,
   PartiallyHidden  = 2,
   CompletelyHidden = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace IEDPresent

namespace CoverShroudStatus
{
enum Enum
{
   Closed          = 0,
   Opening         = 1,
   BlownOrDetached = 2,
   OpenOrAttached  = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace CoverShroudStatus

namespace Color
{
enum Enum
{
#define X(Value, String, Enumeration) Enumeration = Value,
#include "DisColor.xenum"
#undef X
};

DIS_EXPORT const bool IsValid(DisEnum8 v);
DIS_EXPORT const std::string& ToString(Enum v);
DIS_EXPORT const Enum         ToEnum(const std::string& v);
} // namespace Color

namespace ExtendedPaintScheme
{
enum Enum
{
   Default = 0
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace ExtendedPaintScheme

namespace Season
{
enum Enum
{
   Summer = 0,
   Winter = 1,
   Spring = 2,
   Autumn = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Season

namespace Material
{
enum Enum
{
   NotSpecified = 0,
   Plastic      = 1,
   Rubber       = 2,
   Road         = 3
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Material

namespace LeafCoverage
{
enum Enum
{
   Normal = 0,
   Bare   = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LeafCoverage

namespace HeadLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace HeadLights

namespace LandingLights
{
enum Enum
{
   Off = 0,
   On  = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LandingLights

namespace LandingGear
{
enum Enum
{
   NotExtended = 0,
   Extended    = 1
};
DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LandingGear

namespace RunningLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace RunningLights

namespace Taillights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Taillights

namespace NavigationLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace NavigationLights

namespace AntiCollisionLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace AntiCollisionLights

namespace LowerAntiCollisionLight
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace LowerAntiCollisionLight

namespace UpperAntiCollisionLight
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace UpperAntiCollisionLight

namespace BrakeLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace BrakeLights

namespace Flaming
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Flaming

namespace Blinking
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Blinking

namespace CargoDoors
{
enum Enum
{
   Closed = 0,
   Open   = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace CargoDoors

namespace Launcher
{
enum Enum
{
   Off = 0,
   On  = 1
};
}

namespace Afterburner
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Afterburner

namespace FrozenType
{
enum Enum
{
   NotFrozen = 0,
   Frozen    = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace FrozenType

namespace Powerplant
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Powerplant

namespace State
{
enum
{
   Active      = 0,
   Deactivated = 1
};
}

namespace Tent
{
enum Enum
{
   NotExtended = 0,
   Extended    = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Tent

namespace FormationLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace FormationLights

namespace Ramp
{
enum Enum
{
   NotExtended = 0,
   Extended    = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace Ramp

namespace BlackoutLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace BlackoutLights

namespace BlackoutBrakeLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace BlackoutBrakeLights

namespace SpotSearchlights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace SpotSearchlights

namespace InteriorLights
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace InteriorLights

namespace ReverseThrust
{
enum Enum
{
   Off = 0,
   On  = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace ReverseThrust

namespace WeightOnWheels
{
enum Enum
{
   No  = 0,
   Yes = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace WeightOnWheels

namespace SurrenderState
{
enum Enum
{
   Fighting    = 0,
   Surrendered = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace SurrenderState

namespace MaskedCloaked
{
enum Enum
{
   NotCovered    = 0,
   MaskedCloaked = 1
};

DIS_EXPORT const bool IsValid(int aIn);
DIS_EXPORT const std::string& ToString(Enum aIn);
DIS_EXPORT const Enum         ToEnum(const std::string& aIn);
} // namespace MaskedCloaked
} // end namespace Appearance
} // namespace DisEnum

#endif
