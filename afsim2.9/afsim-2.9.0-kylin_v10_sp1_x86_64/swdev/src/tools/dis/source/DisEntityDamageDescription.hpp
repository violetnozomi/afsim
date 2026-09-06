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

#ifndef DISENTITYDAMAGEDESCRIPTION_HPP
#define DISENTITYDAMAGEDESCRIPTION_HPP

#include "dis_export.h"

#include "DisEventId.hpp"
#include "DisTypes.hpp"
class GenI;
class GenO;

class DIS_EXPORT DisEntityDamageDescription
{
public:
   DisEntityDamageDescription();
   virtual ~DisEntityDamageDescription();

   // Accessors
   void GetDamageLocation(DisFloat32& aDamageLocationX, DisFloat32& aDamageLocationY, DisFloat32& aDamageLocationZ) const;
   DisFloat32        GetDamageDiameter() const;
   DisFloat32        GetTemperature() const;
   DisEnum8          GetComponentIdentification() const;
   DisEnum8          GetComponentDamageStatus() const;
   DisEnum8          GetComponentVisualDamageStatus() const;
   DisEnum8          GetComponentVisualSmokeColor() const;
   const DisEventId& GetFireEventId() const;

   // Input/Output
   void      Get(GenI& aGenI);
   DisUint16 GetLength();
   void      Put(GenO& aGenO) const;

   // Mutators
   void SetDamageLocation(DisFloat32 aDamageLocationX, DisFloat32 aDamageLocationY, DisFloat32 aDamageLocationZ);
   void SetDamageDiameter(DisFloat32 aDamageDiameter);
   void SetTemperature(DisFloat32 aTemperature);
   void SetComponentIdentifications(DisEnum8 aComponentIdentifications);
   void SetComponentDamageStatus(DisEnum8 aComponentDamageStatus);
   void SetComponentVisualDamageStatus(DisEnum8 aComponentVisualDamageStatus);
   void SetComponentVisualSmokeColor(DisEnum8 aComponentVisualSmokeColor);
   void SetFireEventId(const DisEventId& aFireEventId);

protected:
private:
   DisFloat32 mDamageLocation[3];
   DisFloat32 mDamageDiameter;
   DisFloat32 mTemperature;
   DisEnum8   mComponentIdentification;
   DisEnum8   mComponentDamageStatus;
   DisEnum8   mComponentVisualDamageStatus;
   DisEnum8   mComponentVisualSmokeColor;
   DisEventId mFireEventId;
};

// Accessors
inline void DisEntityDamageDescription::GetDamageLocation(DisFloat32& aDamageLocationX,
                                                          DisFloat32& aDamageLocationY,
                                                          DisFloat32& aDamageLocationZ) const
{
   aDamageLocationX = mDamageLocation[0];
   aDamageLocationY = mDamageLocation[1];
   aDamageLocationZ = mDamageLocation[2];
}

inline DisFloat32 DisEntityDamageDescription::GetDamageDiameter() const
{
   return mDamageDiameter;
}

inline DisFloat32 DisEntityDamageDescription::GetTemperature() const
{
   return mTemperature;
}

inline DisEnum8 DisEntityDamageDescription::GetComponentIdentification() const
{
   return mComponentIdentification;
}

inline DisEnum8 DisEntityDamageDescription::GetComponentDamageStatus() const
{
   return mComponentDamageStatus;
}

inline DisEnum8 DisEntityDamageDescription::GetComponentVisualDamageStatus() const
{
   return mComponentVisualDamageStatus;
}

inline DisEnum8 DisEntityDamageDescription::GetComponentVisualSmokeColor() const
{
   return mComponentVisualSmokeColor;
}

inline const DisEventId& DisEntityDamageDescription::GetFireEventId() const
{
   return mFireEventId;
}

// Mutators
inline void DisEntityDamageDescription::SetDamageLocation(DisFloat32 aDamageLocationX,
                                                          DisFloat32 aDamageLocationY,
                                                          DisFloat32 aDamageLocationZ)
{
   mDamageLocation[0] = aDamageLocationX;
   mDamageLocation[1] = aDamageLocationY;
   mDamageLocation[2] = aDamageLocationZ;
}

inline void DisEntityDamageDescription::SetDamageDiameter(DisFloat32 aDamageDiameter)
{
   mDamageDiameter = aDamageDiameter;
}

inline void DisEntityDamageDescription::SetTemperature(DisFloat32 aTemperature)
{
   mTemperature = aTemperature;
}

inline void DisEntityDamageDescription::SetComponentIdentifications(DisEnum8 aComponentIdentification)
{
   mComponentIdentification = aComponentIdentification;
}

inline void DisEntityDamageDescription::SetComponentDamageStatus(DisEnum8 aComponentDamageStatus)
{
   mComponentDamageStatus = aComponentDamageStatus;
}

inline void DisEntityDamageDescription::SetComponentVisualDamageStatus(DisEnum8 aComponentVisualDamageStatus)
{
   mComponentVisualDamageStatus = aComponentVisualDamageStatus;
}

inline void DisEntityDamageDescription::SetComponentVisualSmokeColor(DisEnum8 aComponentVisualSmokeColor)
{
   mComponentVisualSmokeColor = aComponentVisualSmokeColor;
}

inline void DisEntityDamageDescription::SetFireEventId(const DisEventId& aFireEventId)
{
   mFireEventId = aFireEventId;
}

// Define GenIO insertion and extraction operators
inline GenO& operator<<(GenO& aGenO, const DisEntityDamageDescription& aDesc)
{
   aDesc.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisEntityDamageDescription& aDesc)
{
   aDesc.Get(aGenI);
   return aGenI;
}
#endif
