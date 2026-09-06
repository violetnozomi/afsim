// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISENHANCEDGROUNDSOLDIERRECORD_HPP
#define DISENHANCEDGROUNDSOLDIERRECORD_HPP

#include "dis_export.h"

#include "DisBasicGroundSoldierRecord.hpp"

//! Contains additional information regarding an entity found within a group of soldiers
class DIS_EXPORT DisEnhancedGroundSoldierRecord : public DisBasicGroundSoldierRecord
{
public:
   DisEnhancedGroundSoldierRecord() = default;
   DisEnhancedGroundSoldierRecord(GenI& aGenI);
   DisEnhancedGroundSoldierRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisEnhancedGroundSoldierRecord(const DisBasicGroundSoldierRecord& aRecord, GenI& aGenI);
   DisEnhancedGroundSoldierRecord(const DisEnhancedGroundSoldierRecord& aSrc);

   // Input/output
   virtual void Get(GenI& aGenI) override;
   DisUint16    GetLength() override;
   virtual void Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                            IsValid() const override;
   virtual DisEnhancedGroundSoldierRecord* Clone() const override;

   // Getters
   DisUint8 GetWaterStatus() const;
   DisEnum8 GetRestStatus() const;
   DisUint8 GetPrimaryAmmunition() const;
   DisUint8 GetSecondaryAmmunition() const;

   // Setters
   void SetWaterStatus(DisUint8 aWaterStatus);
   void SetRestStatus(const DisEnum8& aRestStatus);
   void SetPrimaryAmmunition(DisUint8 aPrimaryAmmunition);
   void SetSecondaryAmmunition(DisUint8 aSecondaryAmmunition);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisEnhancedGroundSoldierRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! The amount of water on hand for this (grouped) entity
   DisUint8 mWaterStatus = 0;

   //! Rest status for this entity
   DisEnum8 mRestStatus = 0;

   //! The amount of primary ammunition remaining for this grouped unit. The type of
   //! ammunition is specified by the natural ammunition units for the primary weapons system
   DisUint8 mPrimaryAmmunition = 0;

   //! The amount of secondary ammunition remaining for this grouped unit. The type of
   //! ammunition is specified by the natural ammunition units for the secondary weapons system
   DisUint8 mSecondaryAmmunition = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 4;
};
#endif
