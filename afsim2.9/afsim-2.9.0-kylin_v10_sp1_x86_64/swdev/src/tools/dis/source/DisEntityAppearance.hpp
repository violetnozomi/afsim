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

#ifndef DIS_APPEARANCE_RECORD_HPP
#define DIS_APPEARANCE_RECORD_HPP

#include "dis_export.h"

#include <string>

#include "DisTypes.hpp"

class GenI;
class GenO;

/**!
 * This class is a utility to set and decipher DIS Entity Appearance bits.
 * Appearance Record is an attribute of DisEntityState.  One must include
 * DisAppearanceEnums.hpp to interpret the unsigned int values set or returned here.
 * Use namespace DisEnum::Appearance::Xxxx.  Example:  unsigned int GetPaintScheme()
 * return value of 1 means:  DisEnum::Appearance::PaintScheme::Camouflage,
 * versus a return value of zero, which means xxx::Uniform.
 */
class DIS_EXPORT DisEntityAppearance
{
public:
   DisEntityAppearance()                                = default;
   DisEntityAppearance(const DisEntityAppearance& aSrc) = default;
   DisEntityAppearance(DisUint32 aValue);

   DisUint32 Value() const { return mAppearanceRecord; }
   void      Set(DisUint32 aValue) { mAppearanceRecord = aValue; }

   std::string ToString(DisEnum8 aKind, DisEnum8 aDomain) const;
   void        Stream(std::ostream& aStream, DisEnum8 aKind, DisEnum8 aDomain) const;

   DisEntityAppearance* Clone() const;

   std::string GetLandPlatformString() const;
   std::string GetAirPlatformString() const;
   std::string GetSurfacePlatformString() const;
   std::string GetSubSurfacePlatformString() const;
   std::string GetSpacePlatformString() const;

   // Input/Output
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

   // Operators
   bool operator==(const DisEntityAppearance& aRhs) const;

   unsigned int GetPaintScheme() const;
   unsigned int GetMobility() const;
   unsigned int GetPropulsion() const;
   unsigned int GetFirepower() const;
   unsigned int GetNVGMode() const;
   unsigned int GetDamage() const;
   unsigned int GetSmoke() const;
   unsigned int GetEngineSmoke() const;
   unsigned int GetTrailingEffects() const;
   unsigned int GetHatch() const;
   unsigned int GetCanopy() const;
   unsigned int GetHeadlights() const;
   unsigned int GetLandingLights() const;
   unsigned int GetRunningLights() const;
   unsigned int GetTaillights() const;
   unsigned int GetNavigationLights() const;
   unsigned int GetAntiCollisionLights() const;
   unsigned int GetLowerAntiCollisionLightsOn() const;
   unsigned int GetUpperAntiCollisionLightsOn() const;
   unsigned int GetAntiCollisionLightsDay() const;
   unsigned int GetBrakelights() const;
   unsigned int GetFlaming() const;
   unsigned int GetIsBlinking() const;
   unsigned int GetLauncher() const;
   unsigned int GetAfterburner() const;
   unsigned int GetCamouflageType() const;
   unsigned int GetConcealed() const;
   unsigned int GetFrozen() const;
   unsigned int GetPowerplant() const;
   unsigned int GetState() const;
   unsigned int GetTent() const;
   unsigned int GetFormationLights() const;
   unsigned int GetRamp() const;
   unsigned int GetLandingGear() const;
   unsigned int GetCargoDoors() const;
   unsigned int GetNavigationPositionBrightness() const;
   unsigned int GetBlackoutLights() const;
   unsigned int GetBlackoutBrakeLights() const;
   unsigned int GetSpotlights() const;
   unsigned int GetInteriorLights() const;
   unsigned int GetSurrenderState() const;
   unsigned int GetReverseThrustEngaged() const;
   unsigned int GetMaskedCloaked() const;
   unsigned int GetWeightOnWheels() const;

   void SetPaintScheme(unsigned int aValue);
   void SetMobility(unsigned int aValue);
   void SetPropulsion(unsigned int aValue);
   void SetFirepower(unsigned int aValue);
   void SetNVGMode(unsigned int aValue);
   void SetDamage(unsigned int aValue);
   void SetSmoke(unsigned int aValue);
   void SetEngineSmoke(unsigned int aValue);
   void SetTrailingEffects(unsigned int aValue);
   void SetHatch(unsigned int aValue);
   void SetCanopy(unsigned int aValue);
   void SetHeadlights(unsigned int aValue);
   void SetLandingLights(unsigned int aValue);
   void SetRunningLights(unsigned int aValue);
   void SetTaillights(unsigned int aValue);
   void SetNavigationLights(unsigned int aValue);
   void SetAntiCollisionLights(unsigned int aValue);
   void SetLowerAntiCollisionLightsOn(unsigned int aValue);
   void SetUpperAntiCollisionLightsOn(unsigned int aValue);
   void SetAntiCollisionLightsDay(unsigned int aValue);
   void SetBrakelights(unsigned int aValue);
   void SetFlaming(unsigned int aValue);
   void SetIsBlinking(unsigned int aValue);
   void SetLauncher(unsigned int aValue);
   void SetAfterburner(unsigned int aValue);
   void SetCamouflageType(unsigned int aValue);
   void SetConcealed(unsigned int aValue);

   //! SetFrozen() CAUTION:  This FROZEN bit may need to be controlled solely by your Simulation Manager.
   void SetFrozen(unsigned int aValue);
   void SetPowerplant(unsigned int aValue);

   //! SetState() CAUTION:  This DEACTIVATED bit may need to be controlled solely by your Simulation Manager.
   void SetState(unsigned int aValue);
   void SetTent(unsigned int aValue);
   void SetFormationLights(unsigned int aValue);
   void SetRamp(unsigned int aValue);
   void SetLandingGear(unsigned int aValue);
   void SetCargoDoors(unsigned int aValue);
   void SetNavigationPositionBrightness(unsigned int aValue);
   void SetBlackoutLights(unsigned int aValue);
   void SetBlackoutBrakeLights(unsigned int aValue);
   void SetSpotlights(unsigned int aValue);
   void SetInteriorLights(unsigned int aValue);
   void SetSurrenderState(unsigned int aValue);
   void SetReverseThrustEngaged(unsigned int aValue);
   void SetMaskedCloaked(unsigned int aValue);
   void SetWeightOnWheels(unsigned int aValue);

private:
   void GetMemberData(GenI& aGenI);

   DisUint32 mAppearanceRecord = 0;
};

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisEntityAppearance& aRecord)
{
   aRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEntityAppearance& aRecord)
{
   aRecord.Get(aGenI);
   return aGenI;
}
#endif
