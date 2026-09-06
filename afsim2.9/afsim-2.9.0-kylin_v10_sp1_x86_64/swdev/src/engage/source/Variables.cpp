// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "Variables.hpp"

#include "Platform.hpp"
#include "Simulation.hpp"
#include "UtAtmosphere.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"

namespace engage
{

std::vector<Variables::RealVariable>    Variables::sRealVariables;
std::vector<Variables::IntegerVariable> Variables::sIntegerVariables;
std::vector<Variables::StringVariable>  Variables::sStringVariables;

#define DEFINE_REAL_ACCESSOR(NAME) double NAME(Simulation& aSimulation, size_t aIndex)
#define DEFINE_INTEGER_ACCESSOR(NAME) size_t NAME(Simulation& aSimulation, size_t aIndex)
#define DEFINE_STRING_ACCESSOR(NAME) std::string NAME(Simulation& aSimulation, size_t aIndex)

// TODO Define more variables...
//  =================================================================================================
//  Define accessors to retrieve each known variable type from where ever it is stored.

namespace
{
// Helper methods
inline double GetLocationX(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetLocationX();
}
inline double GetLocationY(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetLocationY();
}
inline double GetLocationZ(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetLocationZ();
}

inline double GetVelocityX(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetVelocityX();
}
inline double GetVelocityY(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetVelocityY();
}
inline double GetVelocityZ(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetVelocityZ();
}

inline double GetAccelerationX(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetAccelerationX();
}
inline double GetAccelerationY(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetAccelerationY();
}
inline double GetAccelerationZ(Platform* aPlatformPtr)
{
   return (aPlatformPtr == nullptr) ? 0.0 : aPlatformPtr->GetAccelerationZ();
}
double GetSpeed(Platform* aPlatformPtr)
{
   double speed(0.0);
   if ((aPlatformPtr != nullptr) && aPlatformPtr->IsValid())
   {
      speed = aPlatformPtr->GetWsfPlatform()->GetSpeed();
   }
   return speed;
}
double GetMachNumber(Simulation& aSimulation, Platform* aPlatformPtr)
{
   double mach(0.0);
   if ((aPlatformPtr != nullptr) && aPlatformPtr->IsValid())
   {
      double alt   = aPlatformPtr->GetLocationZ();
      double speed = aPlatformPtr->GetWsfPlatform()->GetSpeed();
      mach         = speed / aSimulation.GetAtmosphere().SonicVelocity(alt);
   }
   return mach;
}
double GetGeeForce(Platform* aPlatformPtr)
{
   double gee(0.0);
   // if (aPlatformPtr != 0)
   //{
   //    double locWCS[3];
   //    aPlatformPtr->GetWsfPlatform()->GetLocationWCS(locWCS);
   //    double r = UtVec3d::Magnitude(locWCS);
   //    double accel = aPlatformPtr->GetWsfPlatform()->GetAcceleration();
   //    // TODO...
   // }
   return gee;
}
double GetSlantRange(Platform* aSrcPtr, Platform* aTgtPtr)
{
   double range(0.0);
   if ((aSrcPtr != nullptr) && (aTgtPtr != nullptr) && aSrcPtr->IsValid() && aTgtPtr->IsValid())
   {
      double srcToTgtWCS[3];
      aSrcPtr->GetWsfPlatform()->GetRelativeLocationWCS(aTgtPtr->GetWsfPlatform(), srcToTgtWCS);
      range = UtVec3d::Magnitude(srcToTgtWCS);
   }
   return range;
}

// ==============================================================================================

DEFINE_REAL_ACCESSOR(GetTime)
{
   return aSimulation.GetSimTime();
}

// ==============================================================================================
// Target methods

DEFINE_REAL_ACCESSOR(GetTargetX)
{
   return GetLocationX(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetY)
{
   return GetLocationY(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetZ)
{
   return GetLocationZ(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetVX)
{
   return GetVelocityX(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetVY)
{
   return GetVelocityY(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetVZ)
{
   return GetVelocityZ(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetAX)
{
   return GetAccelerationX(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetAY)
{
   return GetAccelerationY(aSimulation.GetTarget());
}
DEFINE_REAL_ACCESSOR(GetTargetAZ)
{
   return GetAccelerationZ(aSimulation.GetTarget());
}

// ==============================================================================================
// Weapon methods

DEFINE_REAL_ACCESSOR(GetWeaponFlightTime)
{
   double    time(0.0);
   Platform* platformPtr = aSimulation.GetWeapon();
   if ((platformPtr != nullptr) && platformPtr->IsValid())
   {
      time = aSimulation.GetSimTime() - platformPtr->GetWsfPlatform()->GetCreationTime();
   }
   return time;
}
DEFINE_REAL_ACCESSOR(GetWeaponX)
{
   return GetLocationX(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponY)
{
   return GetLocationY(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponZ)
{
   return GetLocationZ(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponVX)
{
   return GetVelocityX(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponVY)
{
   return GetVelocityY(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponVZ)
{
   return GetVelocityZ(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponAX)
{
   return GetAccelerationX(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponAY)
{
   return GetAccelerationY(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponAZ)
{
   return GetAccelerationZ(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponSpeed)
{
   return GetSpeed(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponMach)
{
   return GetMachNumber(aSimulation, aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponGeeForce)
{
   return GetGeeForce(aSimulation.GetWeapon());
}
DEFINE_REAL_ACCESSOR(GetWeaponToTargetRange)
{
   return GetSlantRange(aSimulation.GetWeapon(), aSimulation.GetTarget());
}
} // namespace

// =================================================================================================
Variables::Variables()
{
   // All of the data associated with this class is static. Instantiating it simple ensures the
   // static data is created. We only want to do this once.

   if (!sRealVariables.empty())
   {
      return;
   }

   sRealVariables.push_back(RealVariable()); // Dummy entry
   DefineRealVariable("time", "time", UtInput::cTIME, GetTime);
   DefineRealVariable("target_x", "xt", UtInput::cLENGTH, GetTargetX);
   DefineRealVariable("target_y", "yt", UtInput::cLENGTH, GetTargetY);
   DefineRealVariable("target_z", "zt", UtInput::cLENGTH, GetTargetZ);
   DefineRealVariable("target_vx", "xtdot", UtInput::cSPEED, GetTargetVX);
   DefineRealVariable("target_vy", "ytdot", UtInput::cSPEED, GetTargetVY);
   DefineRealVariable("target_vz", "ztdot", UtInput::cSPEED, GetTargetVZ);
   DefineRealVariable("target_ax", "xtddot", UtInput::cACCELERATION, GetTargetAX);
   DefineRealVariable("target_ay", "ytddot", UtInput::cACCELERATION, GetTargetAY);
   DefineRealVariable("target_az", "ztddot", UtInput::cACCELERATION, GetTargetAZ);
   DefineRealVariable("weapon_flight_time", "ftime", UtInput::cTIME, GetWeaponFlightTime);
   DefineRealVariable("weapon_x", "x", UtInput::cLENGTH, GetWeaponX);
   DefineRealVariable("weapon_y", "y", UtInput::cLENGTH, GetWeaponY);
   DefineRealVariable("weapon_z", "z", UtInput::cLENGTH, GetWeaponZ);
   DefineRealVariable("weapon_vx", "xdot", UtInput::cSPEED, GetWeaponVX);
   DefineRealVariable("weapon_vy", "ydot", UtInput::cSPEED, GetWeaponVY);
   DefineRealVariable("weapon_vz", "zdot", UtInput::cSPEED, GetWeaponVZ);
   DefineRealVariable("weapon_ax", "xddot", UtInput::cACCELERATION, GetWeaponAX);
   DefineRealVariable("weapon_ay", "yddot", UtInput::cACCELERATION, GetWeaponAY);
   DefineRealVariable("weapon_az", "zddot", UtInput::cACCELERATION, GetWeaponAZ);
   DefineRealVariable("weapon_speed", "v", UtInput::cSPEED, GetWeaponSpeed);
   DefineRealVariable("weapon_mach", "fmach", UtInput::cNON_DIMENSIONAL, GetWeaponMach);
   DefineRealVariable("weapon_gee_force", "gee", UtInput::cNON_DIMENSIONAL, GetWeaponGeeForce);
   DefineRealVariable("weapon_to_target_range", "rtm", UtInput::cLENGTH, GetWeaponToTargetRange);

   sIntegerVariables.push_back(IntegerVariable()); // Dummy entry
   sStringVariables.push_back(StringVariable());   // Dummy entry
}

// =================================================================================================
//! Find a real variable definition with the specified name.
//! @param aName [input] The name of the variable being requested.
//! @returns The variable index (used to access the run-time value of a variable), or 0 if not found.
size_t Variables::FindRealVariable(const std::string& aName)
{
   std::string name(aName);
   UtStringUtil::ToLower(name);
   for (size_t i = 0; i < sRealVariables.size(); ++i)
   {
      const RealVariable& v = sRealVariables[i];
      if ((name == v.mFullName) || (name == v.mESAMS_Name))
      {
         return i;
      }
   }
   return 0;
}

// =================================================================================================
//! Find an integer variable definition with the specified name.
//! @param aName [input] The name of the variable being requested.
//! @returns The variable index (used to access the run-time value of a variable), or 0 if not found.
size_t Variables::FindIntegerVariable(const std::string& aName)
{
   std::string name(aName);
   UtStringUtil::ToLower(name);
   for (size_t i = 0; i < sIntegerVariables.size(); ++i)
   {
      const IntegerVariable& v = sIntegerVariables[i];
      if ((name == v.mFullName) || (name == v.mESAMS_Name))
      {
         return i;
      }
   }
   return 0;
}

// =================================================================================================
//! Find a string variable definition with the specified name.
//! @param aName [input] The name of the variable being requested.
//! @returns The variable index (used to access the run-time value of a variable), or 0 if not found.
size_t Variables::FindStringVariable(const std::string& aName)
{
   std::string name(aName);
   UtStringUtil::ToLower(name);
   for (size_t i = 0; i < sStringVariables.size(); ++i)
   {
      const StringVariable& v = sStringVariables[i];
      if ((name == v.mFullName) || (name == v.mESAMS_Name))
      {
         return i;
      }
   }
   return 0;
}

// =================================================================================================
//! Get the scale factor to convert the specified real variable from standard to user-specified units.
//! @param aVarIndex [input] Index of the variable.
//! @param aUnits    [input] User-specified units.
// static
double Variables::GetScaleFactor(size_t aVarIndex, const std::string& aUnits)
{
   if ((aVarIndex < 1) || (aVarIndex >= sRealVariables.size()))
   {
      return 1.0;
   }
   if (sRealVariables[aVarIndex].mUnitType == UtInput::cNON_DIMENSIONAL)
   {
      return 1.0;
   }
   return UtInput::ConvertValueTo(1.0, aUnits, sRealVariables[aVarIndex].mUnitType);
}

// =================================================================================================
//! Get the current value associated with the specified variable.
//! @param aVarIndex   [input] The index of the variable whose value is being requested.
//! @param aSimulation [input] The simulation containing the variable.
//! @param aArrayIndex [input] The array index for variables that are arrays.
//! @returns The current value of the requested variable.
// static
double Variables::GetRealValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex)
{
   if ((aVarIndex < 1) || (aVarIndex >= sRealVariables.size()))
   {
      return 0.0;
   }
   return sRealVariables[aVarIndex].mAccessorPtr(aSimulation, aArrayIndex);
}

// =================================================================================================
//! Get the current value associated with the specified variable.
//! @param aVarIndex   [input] The index of the variable whose value is being requested.
//! @param aSimulation [input] The simulation containing the variable.
//! @param aArrayIndex [input] The array index for variables that are arrays.
//! @returns The current value of the requested variable.
// static
int Variables::GetIntegerValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex)
{
   if ((aVarIndex < 1) || (aVarIndex >= sIntegerVariables.size()))
   {
      return 0;
   }
   return sIntegerVariables[aVarIndex].mAccessorPtr(aSimulation, aArrayIndex);
}

// =================================================================================================
//! Get the current value associated with the specified variable.
//! @param aVarIndex   [input] The index of the variable whose value is being requested.
//! @param aSimulation [input] The simulation containing the variable.
//! @param aArrayIndex [input] The array index for variables that are arrays.
//! @returns The current value of the requested variable.
// static
std::string Variables::GetStringValue(size_t aVarIndex, Simulation& aSimulation, size_t aArrayIndex)
{
   if ((aVarIndex < 1) || (aVarIndex >= sStringVariables.size()))
   {
      return std::string();
   }
   return sStringVariables[aVarIndex].mAccessorPtr(aSimulation, aArrayIndex);
}

// =================================================================================================
//! Define a real/floating-point output variable.
//! @param aName        [input] The name of the variable.
//! @param aESAM_Name   [input] The name of the variable as known by ESAMS
//! @param aUnitType    [input] The unit of measure for the variable
//! @param aAccessorPtr [input] Pointer to the accessor function.
// private static
void Variables::DefineRealVariable(const std::string& aName,
                                   const std::string& aESAMS_Name,
                                   UtInput::ValueType aUnitType,
                                   RealAccessorPtr    aAccessorPtr)
{
   sRealVariables.push_back(RealVariable());
   RealVariable& v(sRealVariables.back());
   v.mFullName    = aName;
   v.mESAMS_Name  = aESAMS_Name;
   v.mUnitType    = aUnitType;
   v.mAccessorPtr = aAccessorPtr;
}

// =================================================================================================
//! Define a integer output variable.
//! @param aName        [input] The name of the variable.
//! @param aESAM_Name   [input] The name of the variable as known by ESAMS
//! @param aUnitType    [input] The unit of measure for the variable
//! @param aAccessorPtr [input] Pointer to the accessor function.
// private static
void Variables::DefineIntegerVariable(const std::string& aName, const std::string& aESAMS_Name, IntegerAccessorPtr aAccessorPtr)
{
   sIntegerVariables.push_back(IntegerVariable());
   IntegerVariable& v(sIntegerVariables.back());
   v.mFullName    = aName;
   v.mESAMS_Name  = aESAMS_Name;
   v.mAccessorPtr = aAccessorPtr;
}

// =================================================================================================
//! Define a string output variable.
//! @param aName        [input] The name of the variable.
//! @param aESAM_Name   [input] The name of the variable as known by ESAMS
//! @param aAccessorPtr [input] Pointer to the accessor function.
// private static
void Variables::DefineStringVariable(const std::string& aName, const std::string& aESAMS_Name, StringAccessorPtr aAccessorPtr)
{
   sStringVariables.push_back(StringVariable());
   StringVariable& v(sStringVariables.back());
   v.mFullName    = aName;
   v.mESAMS_Name  = aESAMS_Name;
   v.mAccessorPtr = aAccessorPtr;
}

} // namespace engage
