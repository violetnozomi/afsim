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

// DIS DE Damage Description Record

#ifndef DIS_DE_DAMAGE_DamageDescriptionSCRIPTION_RECORD_HPP
#define DIS_DE_DAMAGE_DamageDescriptionSCRIPTION_RECORD_HPP

#include "dis_export.h"

#include "DisDDRecord.hpp"
#include "DisEventId.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDEDamageDescriptionRecord : public DisDDRecord
{
public:
   DisDEDamageDescriptionRecord();
   DisDEDamageDescriptionRecord(DisDDRecord& aRecord, GenI& aGenI);
   ~DisDEDamageDescriptionRecord() override;

   // Input/output

   DisUint16 GetDerivedClassLength() override;
   void      GetDerivedClass(GenI& aGenI) override;
   void      PutDerivedClass(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   // Accessors

   void              GetDamageLocation(DisFloat32& aLocationX, DisFloat32& aLocationY, DisFloat32& aLocationZ) const;
   DisFloat32        GetDamageDiameter();
   DisFloat32        GetTemperature();
   DisEnum8          GetComponentIdentification();
   DisEnum8          GetComponentDamageStatus();
   DisEnum8          GetComponentVisualDamageStatus();
   DisEnum8          GetComponentVisualSmokeColor();
   const DisEventId& GetFireEvent();

   // Mutators

   void SetDamageLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ);
   void SetDamageDiameter(DisFloat32 aDamageDiameter);
   void SetTemperature(DisFloat32 aTemperature);
   void SetComponentIdentification(DisEnum8 aComponentIdentification);
   void SetComponentDamageStatus(DisEnum8 aComponentDamageStatus);
   void SetComponentVisualDamageStatus(DisEnum8 aComponentVisualDamageStatus);
   void SetComponentVisualSmokeColor(DisEnum8 aComponentVisualSmokeColor);
   void SetFireEvent(const DisEventId& aEvent);

private:
   DisFloat32 mDamageLocation[3];
   DisFloat32 mDamageDiameter;
   DisFloat32 mTemperature;
   DisEnum8   mComponentIdentification;
   DisEnum8   mComponentDamageStatus;
   DisEnum8   mComponentVisualDamageStatus;
   DisEnum8   mComponentVisualSmokeColor;
   DisEventId mFireEvent;
   DisUint16  mPadding;
};

// Accessors

inline void DisDEDamageDescriptionRecord::GetDamageLocation(DisFloat32& aLocationX,
                                                            DisFloat32& aLocationY,
                                                            DisFloat32& aLocationZ) const
{
   aLocationX = mDamageLocation[0];
   aLocationY = mDamageLocation[1];
   aLocationZ = mDamageLocation[2];
}

inline DisFloat32 DisDEDamageDescriptionRecord::GetDamageDiameter()
{
   return mDamageDiameter;
}

inline DisFloat32 DisDEDamageDescriptionRecord::GetTemperature()
{
   return mTemperature;
}

inline DisEnum8 DisDEDamageDescriptionRecord::GetComponentIdentification()
{
   return mComponentIdentification;
}

inline DisEnum8 DisDEDamageDescriptionRecord::GetComponentDamageStatus()
{
   return mComponentDamageStatus;
}

inline DisEnum8 DisDEDamageDescriptionRecord::GetComponentVisualDamageStatus()
{
   return mComponentVisualDamageStatus;
}

inline DisEnum8 DisDEDamageDescriptionRecord::GetComponentVisualSmokeColor()
{
   return mComponentVisualSmokeColor;
}

inline const DisEventId& DisDEDamageDescriptionRecord::GetFireEvent()
{
   return mFireEvent;
}

// Mutators

inline void DisDEDamageDescriptionRecord::SetDamageLocation(DisFloat32 aLocationX, DisFloat32 aLocationY, DisFloat32 aLocationZ)
{
   mDamageLocation[0] = aLocationX;
   mDamageLocation[1] = aLocationY;
   mDamageLocation[2] = aLocationZ;
}

inline void DisDEDamageDescriptionRecord::SetDamageDiameter(DisFloat32 aDamageDiameter)
{
   mDamageDiameter = aDamageDiameter;
}

inline void DisDEDamageDescriptionRecord::SetTemperature(DisFloat32 aTemperature)
{
   mTemperature = aTemperature;
}

inline void DisDEDamageDescriptionRecord::SetComponentIdentification(DisEnum8 aId)
{
   mComponentIdentification = aId;
}

inline void DisDEDamageDescriptionRecord::SetComponentDamageStatus(DisEnum8 aStatus)
{
   mComponentDamageStatus = aStatus;
}

inline void DisDEDamageDescriptionRecord::SetComponentVisualDamageStatus(DisEnum8 aStatus)
{
   mComponentVisualDamageStatus = aStatus;
}

inline void DisDEDamageDescriptionRecord::SetComponentVisualSmokeColor(DisEnum8 aColor)
{
   mComponentVisualSmokeColor = aColor;
}

inline void DisDEDamageDescriptionRecord::SetFireEvent(const DisEventId& aEvent)
{
   mFireEvent = aEvent;
}

#endif
