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

#ifndef DISENHANCEDROTORWINGAIRCRAFTRECORD_HPP
#define DISENHANCEDROTORWINGAIRCRAFTRECORD_HPP

#include "dis_export.h"

#include "DisBasicRotorWingAircraftRecord.hpp"

//! Contains additional information regarding an entity found within a group of fixed rotor wing aircraft
class DIS_EXPORT DisEnhancedRotorWingAircraftRecord : public DisBasicRotorWingAircraftRecord
{
public:
   DisEnhancedRotorWingAircraftRecord() = default;
   DisEnhancedRotorWingAircraftRecord(GenI& aGenI);
   DisEnhancedRotorWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisEnhancedRotorWingAircraftRecord(const DisBasicRotorWingAircraftRecord& aRecord, GenI& aGenI);
   DisEnhancedRotorWingAircraftRecord(const DisEnhancedRotorWingAircraftRecord& aSrc);

   // Input/output
   virtual void Get(GenI& aGenI) override;
   DisUint16    GetLength() override;
   virtual void Put(GenO& aGenO) const override;

   // Data Validation
   virtual bool                                IsValid() const override;
   virtual DisEnhancedRotorWingAircraftRecord* Clone() const override;

   virtual std::string ToString() const override;

   // Getters
   DisUint8 GetSupplementalFuelStatus() const;
   DisUint8 GetAirMaintenanceStatus() const;
   DisUint8 GetPrimaryAmmunitionRemaining() const;
   DisUint8 GetSecondaryAmmunitionRemaining() const;

   // Setters
   void SetSupplementalFuelStatus(DisUint8 aSupplementalFuelStatus);
   void SetAirMaintenanceStatus(DisUint8 aAirMaintenanceStatus);
   void SetPrimaryAmmunitionRemaining(DisUint8 aPrimaryAmmunitionRemaining);
   void SetSecondaryAmmunitionRemaining(DisUint8 aSecondaryAmmunitionRemaining);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisEnhancedRotorWingAircraftRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! The amount of supplemental fuel remaining
   DisUint8 mSupplementalFuelStatus = 0;

   //! The time since last maintenance was completed
   DisUint8 mAirMaintenanceStatus = 0;

   //! The amount of primary ammunition remaining
   DisUint8 mPrimaryAmmunitionRemaining = 0;

   //! The amount of secondary ammunition remaining
   DisUint8 mSecondaryAmmunitionRemaining = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 4;
};
#endif
