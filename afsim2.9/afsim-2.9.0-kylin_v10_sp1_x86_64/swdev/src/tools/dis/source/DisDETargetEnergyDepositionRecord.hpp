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

// DIS DE Target Energy Deposition Record

#ifndef DIS_DE_TARGET_ENERGY_DEPOSITION_RECORD_HPP
#define DIS_DE_TARGET_ENERGY_DEPOSITION_RECORD_HPP

#include "dis_export.h"

#include "DisEntityId.hpp"
#include "DisTypes.hpp"

class GenI;
class GenO;

class DIS_EXPORT DisDETargetEnergyDepositionRecord
{
public:
   DisDETargetEnergyDepositionRecord();
   DisDETargetEnergyDepositionRecord(GenI& aGenI);
   ~DisDETargetEnergyDepositionRecord();

   // Input/output

   DisUint16 GetLength() const;
   void      Get(GenI& aGenI);
   void      Put(GenO& aGenO) const;

   // Validate Data
   bool IsValid() const;

   // Accessors

   const DisEntityId& GetTargetEntityId();
   DisFloat32         GetPeakIrradiance();

   // Mutators

   void SetTargetEntityId(const DisEntityId& aTargetEntityId);
   void SetPeakIrradiance(const DisFloat32 aPeakIrradiance);

private:
   DisEntityId mTargetEntityId;
   DisUint16   mPadding1;
   DisFloat32  mPeakIrradiance;
};

// Accessors

inline DisUint16 DisDETargetEnergyDepositionRecord::GetLength() const
{
   return 12;
}

inline const DisEntityId& DisDETargetEnergyDepositionRecord::GetTargetEntityId()
{
   return mTargetEntityId;
}

inline DisFloat32 DisDETargetEnergyDepositionRecord::GetPeakIrradiance()
{
   return mPeakIrradiance;
}

// Mutators

inline void DisDETargetEnergyDepositionRecord::SetTargetEntityId(const DisEntityId& aTargetEntityId)
{
   mTargetEntityId = aTargetEntityId;
}

inline void DisDETargetEnergyDepositionRecord::SetPeakIrradiance(DisFloat32 aPeakIrradiance)
{
   mPeakIrradiance = aPeakIrradiance;
}

// Define GenIO insertion and extraction operators

inline GenO& operator<<(GenO& aGenO, const DisDETargetEnergyDepositionRecord& aDETargetEnergyDepositionRecord)
{
   aDETargetEnergyDepositionRecord.Put(aGenO);
   return aGenO;
}

inline GenI& operator>>(GenI& aGenI, DisDETargetEnergyDepositionRecord& aDETargetEnergyDepositionRecord)
{
   aDETargetEnergyDepositionRecord.Get(aGenI);
   return aGenI;
}

#endif
