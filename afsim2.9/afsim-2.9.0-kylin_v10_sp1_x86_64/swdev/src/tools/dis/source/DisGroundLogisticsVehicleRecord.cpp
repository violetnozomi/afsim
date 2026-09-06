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

#include "DisGroundLogisticsVehicleRecord.hpp"

#include "DisGroupedEntityCategoryEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGroundLogisticsVehicleRecord::DisGroundLogisticsVehicleRecord(GenI& aGenI)
   : DisGroupedEntityDescriptionRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisGroundLogisticsVehicleRecord::DisGroundLogisticsVehicleRecord(const DisGroupedEntityDescriptionRecord& aRecord,
                                                                 GenI&                                    aGenI)
   : DisGroupedEntityDescriptionRecord(aRecord)
{
   GetMemberData(aGenI);
}

DisGroundLogisticsVehicleRecord::DisGroundLogisticsVehicleRecord(const DisGroundLogisticsVehicleRecord& aSrc)
   : DisGroupedEntityDescriptionRecord(aSrc)
   , mEntitySpeed(aSrc.mEntitySpeed)
{
}

void DisGroundLogisticsVehicleRecord::Get(GenI& aGenI)
{
   DisGroupedEntityDescriptionRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGroundLogisticsVehicleRecord::GetLength()
{
   return DisGroupedEntityDescriptionRecord::GetLength() + cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisGroundLogisticsVehicleRecord::Put(GenO& aGenO) const
{
   DisGroupedEntityDescriptionRecord::Put(aGenO);
   aGenO << mEntitySpeed;
}

bool DisGroundLogisticsVehicleRecord::IsValid() const
{
   return DisGroupedEntityDescriptionRecord::IsValid();
}

DisGroundLogisticsVehicleRecord* DisGroundLogisticsVehicleRecord::Clone() const
{
   return new DisGroundLogisticsVehicleRecord(*this);
}

DisInt8 DisGroundLogisticsVehicleRecord::GetEntitySpeed() const
{
   return mEntitySpeed;
}

void DisGroundLogisticsVehicleRecord::SetEntitySpeed(DisInt8 aEntitySpeed)
{
   mEntitySpeed = aEntitySpeed;
}

void DisGroundLogisticsVehicleRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mEntitySpeed;
}

// Operators
bool DisGroundLogisticsVehicleRecord::operator==(const DisGroundLogisticsVehicleRecord& aRhs) const
{
   return ((DisGroupedEntityDescriptionRecord::operator==(aRhs)) && (mEntitySpeed == aRhs.mEntitySpeed));
}
