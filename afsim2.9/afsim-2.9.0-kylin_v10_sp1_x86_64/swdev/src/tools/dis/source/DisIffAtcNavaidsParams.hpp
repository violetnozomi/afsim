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

#ifndef DISIFFATCNAVAIDSPARAMS_HPP
#define DISIFFATCNAVAIDSPARAMS_HPP

#include "dis_export.h"

#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisIffAtcNavaidsParams
{
public:
   DisIffAtcNavaidsParams();
   ~DisIffAtcNavaidsParams();
   // Using default copy constructor

   // Accessors

   DisFloat32 GetEffectiveRadiatedPower() const;
   DisFloat32 GetFrequency() const;
   DisFloat32 GetPulseRepetitionFrequency() const;
   DisFloat32 GetPulseWidth() const;
   DisUint32  GetBurstLength() const;
   DisEnum8   GetApplicableModes() const;

   // Mutators

   void SetEffectiveRadiatedPower(DisFloat32 aEffectiveRadiatedPower);
   void SetFrequency(DisFloat32 aFrequency);
   void SetPulseRepetitionFrequency(DisFloat32 aPulseRepetitionFrequency);
   void SetPulseWidth(DisFloat32 aPulseWidth);
   void SetBurstLength(DisUint32 aBurstLength);
   void SetApplicableModes(DisEnum8 aApplicableModes);

   // Input/Output

   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;

   // Data Validation
   bool IsValid() const;

private:
   DisFloat32 mEffectiveRadiatedPower;
   DisFloat32 mFrequency;
   DisFloat32 mPulseRepetitionFrequency;
   DisFloat32 mPulseWidth;
   DisUint32  mBurstLength;
   DisEnum8   mApplicableModes;
};

// Accessors

inline DisFloat32 DisIffAtcNavaidsParams::GetEffectiveRadiatedPower() const
{
   return mEffectiveRadiatedPower;
}
inline DisFloat32 DisIffAtcNavaidsParams::GetFrequency() const
{
   return mFrequency;
}
inline DisFloat32 DisIffAtcNavaidsParams::GetPulseRepetitionFrequency() const
{
   return mPulseRepetitionFrequency;
}
inline DisFloat32 DisIffAtcNavaidsParams::GetPulseWidth() const
{
   return mPulseWidth;
}
inline DisUint32 DisIffAtcNavaidsParams::GetBurstLength() const
{
   return mBurstLength;
}
inline DisEnum8 DisIffAtcNavaidsParams::GetApplicableModes() const
{
   return mApplicableModes;
}

// Mutators

inline void DisIffAtcNavaidsParams::SetEffectiveRadiatedPower(DisFloat32 aEffectiveRadiatedPower)
{
   mEffectiveRadiatedPower = aEffectiveRadiatedPower;
}
inline void DisIffAtcNavaidsParams::SetFrequency(DisFloat32 aFrequency)
{
   mFrequency = aFrequency;
}
inline void DisIffAtcNavaidsParams::SetPulseRepetitionFrequency(DisFloat32 aPulseRepetitionFrequency)
{
   mPulseRepetitionFrequency = aPulseRepetitionFrequency;
}
inline void DisIffAtcNavaidsParams::SetPulseWidth(DisFloat32 aPulseWidth)
{
   mPulseWidth = aPulseWidth;
}
inline void DisIffAtcNavaidsParams::SetBurstLength(DisUint32 aBurstLength)
{
   mBurstLength = aBurstLength;
}
inline void DisIffAtcNavaidsParams::SetApplicableModes(DisEnum8 aApplicableModes)
{
   mApplicableModes = aApplicableModes;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisIffAtcNavaidsParams& aData)
{
   aData.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisIffAtcNavaidsParams& aData)
{
   aData.Get(aGenI);
   return aGenI;
}

#endif
