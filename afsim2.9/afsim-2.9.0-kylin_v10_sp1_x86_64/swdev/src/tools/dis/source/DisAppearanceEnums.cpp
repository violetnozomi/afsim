// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisAppearanceEnums.hpp"

#include <sstream>
#include <utility>
#include <vector>

#include "DisEnumConversion.hpp"

namespace DisEnum
{

namespace Appearance
{

namespace PaintScheme
{

const std::vector<std::pair<Enum, std::string>> data = {{Uniform, "Unifom Color"}, {Camouflage, "Camouflage"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Uniform && aIn <= Camouflage;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace PaintScheme
namespace Damage
{

const std::vector<std::pair<Enum, std::string>> data = {{NoDamage, "No Damage"},
                                                        {Slight, "Camouflage"},
                                                        {Moderate, "Moderate Damage"},
                                                        {Destroyed, "Destroyed"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NoDamage && aIn <= Destroyed;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Damage

namespace TrailingEffects
{

const std::vector<std::pair<Enum, std::string>> data = {{None, "None"}, {Small, "Small"}, {Medium, "Medium"}, {Large, "Large"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= None && aIn <= Large;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace TrailingEffects

namespace Hatch
{

const std::vector<std::pair<Enum, std::string>> data = {{NotApplicable, "Not Applicable"},
                                                        {Closed, "Closed"},
                                                        {Popped, "Popped"},
                                                        {PoppedPersonVisible, "Popped and Person is Visible"},
                                                        {Open, "Open"},
                                                        {OpenPersonVisible, "Open and Person is Visible"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotApplicable && aIn <= OpenPersonVisible;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Hatch

namespace LauncherOperational
{

const std::vector<std::pair<Enum, std::string>> data = {{NotRaisedNotOperational, "Not Raised/Not Operational"},
                                                        {RaisedOperational, "Raised/Operational"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotRaisedNotOperational && aIn <= RaisedOperational;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LauncherOperational

namespace CamouflageType
{

const std::vector<std::pair<Enum, std::string>> data = {{Desert, "Desert Camouflage"},
                                                        {Winter, "Winter Cmouflage"},
                                                        {Forest, "Forest Camouflage"},
                                                        {Other, "Other"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Desert && aIn <= Other;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace CamouflageType

namespace ConcealedPosition
{

const std::vector<std::pair<Enum, std::string>> data = {{NotConcealed, "Not Concealed"},
                                                        {PreparedConcealedPosition, "Prepared Concealed Position"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotConcealed && aIn <= PreparedConcealedPosition;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace ConcealedPosition

namespace EntityObjectState
{

const std::vector<std::pair<Enum, std::string>> data = {{Active, "Active"}, {Deactivated, "Deactivated"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Active && aIn <= Deactivated;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace EntityObjectState

namespace Canopy
{

const std::vector<std::pair<Enum, std::string>> data = {{NotApplicable, "Not Applicable"},
                                                        {SingleClosed, "Single Canopy/Single Troop Door Closed"},
                                                        {FrontRearOrLeftRightClosed,
                                                         "Front and Rear Canopy/Left and Right Troop Door Closed"},
                                                        {FrontOrLeftOpen, "Front Canopy/Left Troop Door Open"},
                                                        {SingleOpen, "Single Canopy/Single Troop Door Open"},
                                                        {RearRightOpen, "Rear Canopy/Right Troop Door Open"},
                                                        {FrontRearLeftRightOpen,
                                                         "Front and Rear Canopy/Left and Right Troop Door Open"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotApplicable && aIn <= FrontRearLeftRightOpen;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Canopy

namespace SubsurfaceHatch
{

const std::vector<std::pair<Enum, std::string>> data = {{NotApplicable, "Not Applicable"},
                                                        {Closed, "Hatch is Closed"},
                                                        {Open, "Hatch is Open"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn == NotApplicable || aIn == Closed || aIn == Open;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace SubsurfaceHatch

namespace LifeFormHealth
{

const std::vector<std::pair<Enum, std::string>> data = {{NoInjury, "No Injury"},
                                                        {SlightInjury, "Slight injury"},
                                                        {ModerateInjury, "Moderate Injury"},
                                                        {FatalInjury, "Fatal Injury"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NoInjury && aIn <= FatalInjury;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LifeFormHealth

namespace LifeFormCompliance
{

const std::vector<std::pair<Enum, std::string>> data = {{NotSpecified, "Not Specified"},
                                                        {Detained, "Detained"},
                                                        {Surrender, "Surrender"},
                                                        {UsingFists, "using Fists"},
                                                        {VerbalAbuse1, "Verbal Abuse Level 1"},
                                                        {VerbalAbuse2, "Verbal Abuse Level 2"},
                                                        {VerbalAbuse3, "Verbal Abuse Level 3"},
                                                        {PassiveResistance1, "Passive Resistance Level 1"},
                                                        {PassiveResistance2, "Passive Resistance Level 2"},
                                                        {PassiveResistance3, "Passive Resistance Level 3"},
                                                        {NonLethalWeapon1, "Using Non-Lethal Weapon 1"},
                                                        {NonLethalWeapon2, "Using Non-Lethal Weapon 2"},
                                                        {NonLethalWeapon3, "Using Non-Lethal Weapon 3"},
                                                        {NonLethalWeapon4, "Using Non-Lethal Weapon 4"},
                                                        {NonLethalWeapon5, "Using Non-Lethal Weapon 5"},
                                                        {NonLethalWeapon6, "Using Non-Lethal Weapon 6"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotSpecified && aIn <= NonLethalWeapon6;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LifeFormCompliance

namespace LifeFormPosture
{

const std::vector<std::pair<Enum, std::string>> data = {{NotSpecified, "Not Specified"},
                                                        {UprightStandingStill, "Upright, Standing Still"},
                                                        {UprightWalking, "Upright, Walking"},
                                                        {UprightRunning, "Upright, Running"},
                                                        {Kneeling, "Kneeling"},
                                                        {Prone, "Prone"},
                                                        {Crawling, "Crawling"},
                                                        {Swimming, "Swimming"},
                                                        {Parachuting, "Parachuting"},
                                                        {Jumping, "Jumping"},
                                                        {Sitting, "Sitting"},
                                                        {Squatting, "Squatting"},
                                                        {Crouching, "Crouching"},
                                                        {Wading, "Wading"},
                                                        {Surrender, "Surrender"},
                                                        {Detained, "Detained"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotSpecified && aIn <= Detained;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LifeFormPosture

namespace LifeFormWeapon
{

const std::vector<std::pair<Enum, std::string>> data = {{NotPresent, "Not Present"},
                                                        {Stowed, "Stowed"},
                                                        {DeployedActive, "Deployed/Active"},
                                                        {FiringPositionInUse, "Firing Position/In Use"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotPresent && aIn <= FiringPositionInUse;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LifeFormWeapon

namespace ConcealedMovement
{

const std::vector<std::pair<Enum, std::string>> data = {{Open, "OpenMovement"},
                                                        {RushesBetweenCover, "Rushes Between Covered Positions"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Open && aIn <= RushesBetweenCover;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace ConcealedMovement

namespace EnvironmentalDensity
{

const std::vector<std::pair<Enum, std::string>> data = {{Clear, "Clear"},
                                                        {Hazy, "Hazy"},
                                                        {Dense, "Dense"},
                                                        {VeryDense, "Very Dense"},
                                                        {Opaque, "Opaque"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Clear && aIn <= Opaque;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace EnvironmentalDensity

namespace AntiCollision
{

const std::vector<std::pair<Enum, std::string>> data = {{Day, "Day"}, {Night, "Night"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));


const bool IsValid(int aIn)
{
   return aIn >= Day && aIn <= Night;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace AntiCollision

namespace NavigationPositionBrightness
{

const std::vector<std::pair<Enum, std::string>> data = {{Dim, "Dim"}, {Bright, "Bright"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Dim && aIn <= Bright;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace NavigationPositionBrightness

namespace SupplyDeployed
{

const std::vector<std::pair<Enum, std::string>> data = {{NotApplicable, "Not Applicable"},
                                                        {Stowed, "Stowed"},
                                                        {Deployed, "Deployed"},
                                                        {DeployedActive, "Deployed and Active"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotApplicable && aIn <= DeployedActive;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace SupplyDeployed

namespace NVGMode
{

const std::vector<std::pair<Enum, std::string>> data = {{OvertLighting, "Overt Lighting"},
                                                        {CovertLighting, "Covert Lighting"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= OvertLighting && aIn <= CovertLighting;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace NVGMode

namespace Parachute
{

const std::vector<std::pair<Enum, std::string>> data = {{None, "None"},
                                                        {Deployed, "Deployed"},
                                                        {Collapsed, "Collapsed"},
                                                        {MalfunctionOrStreamer, "Malfunction/Streamer"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= None && aIn <= MalfunctionOrStreamer;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Parachute

namespace FlareSmokeColor
{

const std::vector<std::pair<Enum, std::string>> data = {{White, "White"}, {Red, "Red"}, {Green, "Green"}, {IR, "IR"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= White && aIn <= IR;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace FlareSmokeColor

namespace FlareSmoke
{

const std::vector<std::pair<Enum, std::string>> data = {{NotIgnited, "Not Ignited"},
                                                        {Burning, "Burning"},
                                                        {BurnedOut, "Burned Out"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotIgnited && aIn <= BurnedOut;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace FlareSmoke

namespace SpotChaff
{

const std::vector<std::pair<Enum, std::string>> data = {{None, "None"}, {Deployed, "Deployed"}, {Malfunction, "Malfunction"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= None && aIn <= Malfunction;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace SpotChaff

namespace IEDPresent
{

const std::vector<std::pair<Enum, std::string>> data = {{None, "None"},
                                                        {Visible, "Visible"},
                                                        {PartiallyHidden, "Partially Hidden"},
                                                        {CompletelyHidden, "Completely Hidden"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= None && aIn <= CompletelyHidden;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace IEDPresent

namespace CoverShroudStatus
{

const std::vector<std::pair<Enum, std::string>> data = {{Closed, "Closed"},
                                                        {Opening, "Opening"},
                                                        {BlownOrDetached, "Cover/Shroud Blown/Detached"},
                                                        {OpenOrAttached, "Open/Attached"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Closed && aIn <= OpenOrAttached;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace CoverShroudStatus

namespace Color
{

const DisEnum::Conversion<Enum>& GetConversion()
{
   typedef std::pair<Enum, std::string> PAIR;
   typedef std::vector<PAIR>            VECTOR;
   static PAIR                          data[] = {
#define X(Value, String, Enumeration) std::make_pair(Enumeration, String),
#include "DisColor.xenum"
#undef X
   };
   static const VECTOR                    colors(DisEnum::MakeVector(data));
   static const DisEnum::Conversion<Enum> conv(colors);
   return conv;
}

const bool IsValid(DisEnum8 aIn)
{
   return aIn >= NotSpecified && aIn <= White;
}

const std::string& ToString(Enum aIn)
{
   return GetConversion().ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return GetConversion().ToEnum(aIn);
}

} // namespace Color

namespace ExtendedPaintScheme
{

const std::vector<std::pair<Enum, std::string>> data = {{Default, "Default"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn == Default;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace ExtendedPaintScheme

namespace Season
{

const std::vector<std::pair<Enum, std::string>> data = {{Summer, "Summer"},
                                                        {Winter, "Winter"},
                                                        {Spring, "Spring"},
                                                        {Autumn, "Autumn"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Summer && aIn <= Autumn;
}
const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Season

namespace Material
{

const std::vector<std::pair<Enum, std::string>> data = {{NotSpecified, "Not Specified"},
                                                        {Plastic, "Plastic"},
                                                        {Rubber, "Rubber"},
                                                        {Road, "Road"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotSpecified && aIn <= Road;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Material

namespace LeafCoverage
{

const std::vector<std::pair<Enum, std::string>> data = {{Normal, "Normal"}, {Bare, "Bare"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Normal && aIn <= Bare;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LeafCoverage

namespace Mobility
{

const std::vector<std::pair<Enum, std::string>> data = {{NoKill, "Not Killed"}, {Kill, "Killed "}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NoKill && aIn <= Kill;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Mobility

namespace Firepower
{

const std::vector<std::pair<Enum, std::string>> data = {{NoKill, "Not Killed"}, {Kill, "Killed "}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NoKill && aIn <= Kill;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Firepower
namespace Propulsion
{

const std::vector<std::pair<Enum, std::string>> data = {{NoKill, "Not Killed"}, {Kill, "Killed "}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NoKill && aIn <= Kill;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Propulsion

namespace Smoking
{

const std::vector<std::pair<Enum, std::string>> data = {{NotSmoking, "Not Smoking"}, {Smoking, "Smoking "}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotSmoking && aIn <= Smoking;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Smoking

namespace EngineSmoking
{

const std::vector<std::pair<Enum, std::string>> data = {{NotSmoking, "Not Smoking"}, {Smoking, "Smoking "}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotSmoking && aIn <= Smoking;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace EngineSmoking

namespace HeadLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace HeadLights

namespace Taillights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Taillights

namespace BrakeLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}
const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace BrakeLights

namespace Flaming
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Flaming

namespace Blinking
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Blinking

namespace FrozenType
{

const std::vector<std::pair<Enum, std::string>> data = {{NotFrozen, "Not Frozen"}, {Frozen, "Frozen"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotFrozen && aIn <= Frozen;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace FrozenType

namespace Powerplant
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Powerplant

namespace Tent
{

const std::vector<std::pair<Enum, std::string>> data = {{NotExtended, "Not Extended"}, {Extended, "Extended"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotExtended && aIn <= Extended;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Tent
namespace LandingGear
{

const std::vector<std::pair<Enum, std::string>> data = {{NotExtended, "Not Extended"}, {Extended, "Extended"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotExtended && aIn <= Extended;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LandingGear

namespace Ramp
{

const std::vector<std::pair<Enum, std::string>> data = {{NotExtended, "Not Extended"}, {Extended, "Extended"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotExtended && aIn <= Extended;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Ramp

namespace BlackoutLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace BlackoutLights
namespace LandingLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));


const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LandingLights

namespace RunningLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace RunningLights

namespace NavigationLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace NavigationLights

namespace AntiCollisionLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace AntiCollisionLights

namespace LowerAntiCollisionLight
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace LowerAntiCollisionLight
namespace UpperAntiCollisionLight
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace UpperAntiCollisionLight

namespace FormationLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace FormationLights

namespace BlackoutBrakeLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace BlackoutBrakeLights

namespace SpotSearchlights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace SpotSearchlights

namespace InteriorLights
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace InteriorLights

namespace Afterburner
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace Afterburner

namespace ReverseThrust
{

const std::vector<std::pair<Enum, std::string>> data = {{Off, "Off"}, {On, "On"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Off && aIn <= On;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace ReverseThrust
namespace WeightOnWheels
{

const std::vector<std::pair<Enum, std::string>> data = {{No, "No"}, {Yes, "Yes"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= No && aIn <= Yes;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace WeightOnWheels
namespace SurrenderState
{

const std::vector<std::pair<Enum, std::string>> data = {{Fighting, "Fighting"}, {Surrendered, "Surrendered"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Fighting && aIn <= Surrendered;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace SurrenderState

namespace MaskedCloaked
{

const std::vector<std::pair<Enum, std::string>> data = {{NotCovered, "Not Covered"}, {MaskedCloaked, "Masked/Cloaked"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= NotCovered && aIn <= MaskedCloaked;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace MaskedCloaked

namespace CargoDoors
{

const std::vector<std::pair<Enum, std::string>> data = {{Closed, "Closed"}, {Open, "Open"}};

DisEnum::Conversion<Enum> conv(std::vector<std::pair<Enum, std::string>>(data.begin(), data.end()));

const bool IsValid(int aIn)
{
   return aIn >= Closed && aIn <= Open;
}

const std::string& ToString(Enum aIn)
{
   return conv.ToString(aIn);
}

const Enum ToEnum(const std::string& aIn)
{
   return conv.ToEnum(aIn);
}

} // namespace CargoDoors

} // namespace Appearance

} // namespace DisEnum
