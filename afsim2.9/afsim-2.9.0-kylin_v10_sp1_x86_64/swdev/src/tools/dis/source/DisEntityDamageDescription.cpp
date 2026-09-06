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

#include "DisEntityDamageDescription.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisEntityDamageDescription::DisEntityDamageDescription()
   : mDamageDiameter(0.0)
   , mTemperature(-1000.0)
   , mComponentIdentification(0)
   , mComponentDamageStatus(0)
   , mComponentVisualDamageStatus(0)
   , mComponentVisualSmokeColor(0)
   , mFireEventId()
{
   for (int i = 0; i < 3; ++i)
   {
      mDamageLocation[i] = 0.0;
   }
}

// virtual
DisEntityDamageDescription::~DisEntityDamageDescription() {}

void DisEntityDamageDescription::Get(GenI& aGenI)
{
   DisUint16 pad16;

   aGenI >> mDamageLocation[0];
   aGenI >> mDamageLocation[1];
   aGenI >> mDamageLocation[2];
   aGenI >> mDamageDiameter;
   aGenI >> mTemperature;
   aGenI >> mComponentIdentification;
   aGenI >> mComponentDamageStatus;
   aGenI >> mComponentVisualDamageStatus;
   aGenI >> mComponentVisualSmokeColor;
   aGenI >> mFireEventId;
   aGenI >> pad16;
}

DisUint16 DisEntityDamageDescription::GetLength()
{
   static const DisUint16 sLength = 32;
   return sLength;
}

void DisEntityDamageDescription::Put(GenO& aGenO) const
{
   DisUint16 pad16 = 0;

   aGenO << mDamageLocation[0];
   aGenO << mDamageLocation[1];
   aGenO << mDamageLocation[2];
   aGenO << mDamageDiameter;
   aGenO << mTemperature;
   aGenO << mComponentIdentification;
   aGenO << mComponentDamageStatus;
   aGenO << mComponentVisualDamageStatus;
   aGenO << mComponentVisualSmokeColor;
   aGenO << mFireEventId;
   aGenO << pad16;
}
