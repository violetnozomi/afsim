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

#ifndef DISGROUNDLOGISTICSVEHICLERECORD_HPP
#define DISGROUNDLOGISTICSVEHICLERECORD_HPP

#include "dis_export.h"

#include "DisGroupEntityDescriptionRecord.hpp"
#include "DisTypes.hpp"

/**!
 * Specifies the entity state and logistics information about an individual entity within a group of
 * ground logistics vehicles
 */
class DIS_EXPORT DisGroundLogisticsVehicleRecord : public DisGroupedEntityDescriptionRecord
{
public:
   DisGroundLogisticsVehicleRecord() = default;
   DisGroundLogisticsVehicleRecord(GenI& aGenI);
   DisGroundLogisticsVehicleRecord(const DisGroupedEntityDescriptionRecord& aRecord, GenI& aGenI);
   DisGroundLogisticsVehicleRecord(const DisGroundLogisticsVehicleRecord& aSrc);

   // Input/output
   void      Get(GenI& aGenI) override;
   DisUint16 GetLength() override;
   void      Put(GenO& aGenO) const override;

   // Data Validation
   bool IsValid() const override;

   DisGroundLogisticsVehicleRecord* Clone() const override;

   // Getters
   DisInt8 GetEntitySpeed() const;

   // Setters
   void SetEntitySpeed(DisInt8 aEntitySpeed);

   // The following operators are provided so they can be used in
   // containers that require comparisons...
   bool operator==(const DisGroundLogisticsVehicleRecord& aRhs) const;

private:
   void GetMemberData(GenI& aGenI);

   //! Speed of a grouped entity (meters per second)
   DisInt8 mEntitySpeed = 0;

   //! Local record size in octets
   static const int cLOCAL_RECORD_SIZE_IN_OCTETS = 1;
};
#endif
