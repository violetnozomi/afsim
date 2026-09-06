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

#ifndef DISENHANCEDFIXEDWINGAIRCRAFTRECORD_HPP
#define DISENHANCEDFIXEDWINGAIRCRAFTRECORD_HPP

#include "dis_export.h"

#include "DisBasicFixedWingAircraftRecord.hpp"

/**!
 * Specifies the entity state and logistics information about an individual entity within a group of
 * fixed wing aircraft
 */
class DIS_EXPORT DisEnhancedFixedWingAircraftRecord : public DisBasicFixedWingAircraftRecord
{
public:
   DisEnhancedFixedWingAircraftRecord() = default;
   DisEnhancedFixedWingAircraftRecord(GenI& aGenI);
   DisEnhancedFixedWingAircraftRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisEnhancedFixedWingAircraftRecord(const DisBasicFixedWingAircraftRecord& aRecord, GenI& aGenI);
   DisEnhancedFixedWingAircraftRecord(const DisEnhancedFixedWingAircraftRecord& aSrc);

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   DisEnhancedFixedWingAircraftRecord* Clone() const override;

   // Getters
   DisUint8 GetSupplementalFuelStatus() const;
   DisUint8 GetAirMaintenanceStatus() const;
   DisUint8 GetPrimaryAmmunition() const;
   DisUint8 GetSecondaryAmmunition() const;

   // Setters
   void SetSupplementalFuelStatus(const DisUint8 aFuelStatus);
   void SetAirMaintenanceStatus(const DisUint8 aAirMaintenanceStatus);
   void SetPrimaryAmmunition(const DisUint8 aPrimaryAmmunition);
   void SetSecondaryAmmunition(const DisUint8 aSecondaryAmmunition);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisEnhancedFixedWingAircraftRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! Fuel Status of a grouped entity shall be the amount of fuel remaining.
   //! Fuel Status shall be specified in 10 liter increments
   DisUint8 mSupplementalFuelStatus = 0;

   //! Ground Maintenance Status of a grouped entity shall be the distance traveled since the last failure.
   //! Ground Maintenance Status shall be specified in tens of kilometers
   DisUint8 mAirMaintenanceStatus = 0;

   //! Primary Ammunition of a grouped entity shall be the amount of primary ammunition remaining.
   //! Primary Ammunition shall be specified in natural ammunition units for the primary weapon system
   DisUint8 mPrimaryAmmunition = 0;

   //! Secondary Ammunition of a grouped entity shall be the amount of secondary ammunition remaining.
   //! Secondary Ammunition shall be specified in natural ammunition units for the secondary weapon system
   DisUint8 mSecondaryAmmunition = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 4;
};
#endif
