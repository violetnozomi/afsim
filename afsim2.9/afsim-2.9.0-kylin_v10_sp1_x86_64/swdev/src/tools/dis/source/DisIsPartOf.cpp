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

#include "DisIsPartOf.hpp"

#include "DisNatureEnum.hpp"
#include "DisPduEnums.hpp"
#include "DisStationNameEnum.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIsPartOf::DisIsPartOf()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::IsPartOf);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   for (int i = 0; i < 3; i++)
   {
      mPartLocation[i] = 0;
   }
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisIsPartOf::DisIsPartOf(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisIsPartOf::DisIsPartOf(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisIsPartOf::DisIsPartOf(const DisIsPartOf& aSrc)
   : DisPdu(aSrc)
   , mOriginatingSimulation(aSrc.mOriginatingSimulation)
   , mReceivedEntityId(aSrc.mReceivedEntityId)
   , mRelationshipNature(aSrc.mRelationshipNature)
   , mRelationshipPosition(aSrc.mRelationshipPosition)
   , mNamedLocationIdentificationRecord(aSrc.mNamedLocationIdentificationRecord)
   , mPartType(aSrc.mPartType)
{
   SetPduType(aSrc.GetPduType());
   SetProtocolFamily(aSrc.GetProtocolFamily());
   for (int i = 0; i < 3; i++)
   {
      mPartLocation[i] = aSrc.mPartLocation[i];
   }
   GetLength();
}

DisIsPartOf* DisIsPartOf::Clone() const
{
   return new DisIsPartOf(*this);
}

int DisIsPartOf::GetClass() const
{
   return DisEnum::Pdu::Type::IsPartOf;
}

int DisIsPartOf::GetFamily() const
{
   return DisEnum::Pdu::Family::EntityManagement;
}

void DisIsPartOf::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIsPartOf::GetLength()
{
   DisUint16 length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   SetLength(length);
   return length;
}

void DisIsPartOf::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   aGenO << mOriginatingSimulation;
   aGenO << mReceivedEntityId;
   aGenO << mRelationshipNature;
   aGenO << mRelationshipPosition;
   aGenO << mPartLocation[0] << mPartLocation[1] << mPartLocation[2];
   mNamedLocationIdentificationRecord.Put(aGenO);
   aGenO << mPartType;
}

const DisEntityId& DisIsPartOf::GetOriginatingSimulation() const
{
   return mOriginatingSimulation;
}

const DisEntityId& DisIsPartOf::GetReceivingEntity() const
{
   return mReceivedEntityId;
}

const DisNatureEnum DisIsPartOf::GetRelationshipNature() const
{
   return DisNatureEnum(mRelationshipNature);
}

DisEnum16 DisIsPartOf::GetRelationshipPosition() const
{
   return mRelationshipPosition;
}

void DisIsPartOf::GetPartLocation(DisFloat32& aPartLocationX, DisFloat32& aPartLocationY, DisFloat32& aPartLocationZ) const
{
   aPartLocationX = mPartLocation[0];
   aPartLocationY = mPartLocation[1];
   aPartLocationZ = mPartLocation[2];
}

const DisStationNameEnum DisIsPartOf::GetStationName() const
{
   return DisStationNameEnum(mNamedLocationIdentificationRecord.GetStationName());
}

DisUint16 DisIsPartOf::GetStationNumber() const
{
   return mNamedLocationIdentificationRecord.GetStationNumber();
}

const DisNamedLocationIdentificationRecord DisIsPartOf::GetNamedLocationIdentificationRecord() const
{
   return mNamedLocationIdentificationRecord;
}

const DisEntityType& DisIsPartOf::GetPartType() const
{
   return mPartType;
}

void DisIsPartOf::SetOriginatingSimulation(const DisEntityId& aOriginatingSimulation)
{
   mOriginatingSimulation = aOriginatingSimulation;
}

void DisIsPartOf::SetReceivingEntity(const DisEntityId& aReceivingEntity)
{
   mReceivedEntityId = aReceivingEntity;
}

void DisIsPartOf::SetRelationshipNature(DisNatureEnum aRelationshipNature)
{
   mRelationshipNature = static_cast<DisEnum16>(aRelationshipNature);
}

void DisIsPartOf::SetRelationshipPosition(DisEnum16 aRelationshipPosition)
{
   mRelationshipPosition = aRelationshipPosition;
}

void DisIsPartOf::SetPartLocation(DisFloat32 aPartLocationX, DisFloat32 aPartLocationY, DisFloat32 aPartLocationZ)
{
   mPartLocation[0] = aPartLocationX;
   mPartLocation[1] = aPartLocationY;
   mPartLocation[2] = aPartLocationZ;
}

void DisIsPartOf::SetStationName(DisStationNameEnum aStationName)
{
   mNamedLocationIdentificationRecord.SetStationName(aStationName);
}

void DisIsPartOf::SetStationNumber(DisUint16 aStationNumber)
{
   mNamedLocationIdentificationRecord.SetStationNumber(aStationNumber);
}

void DisIsPartOf::SetNamedLocationIdentificationRecord(const DisNamedLocationIdentificationRecord& aNamedLocationIdentificationRecord)
{
   mNamedLocationIdentificationRecord = aNamedLocationIdentificationRecord;
}

void DisIsPartOf::SetNamedLocationIdentificationRecord(DisStationNameEnum aStationName, DisEnum16 aStationNumber)
{
   mNamedLocationIdentificationRecord.SetStationName(aStationName);
   mNamedLocationIdentificationRecord.SetStationNumber(aStationNumber);
}

void DisIsPartOf::SetPartType(const DisEntityType& aPartType)
{
   mPartType = aPartType;
}

bool DisIsPartOf::IsValid() const
{
   return DisPdu::IsValid() && mOriginatingSimulation.IsValid() && mReceivedEntityId.IsValid() &&
          ValidateScalarArray(mPartLocation, 3) && mNamedLocationIdentificationRecord.IsValid() && mPartType.IsValid();
}

std::string DisIsPartOf::GetStringId() const
{
   return "DisIsPartOf";
}

std::string DisIsPartOf::ToString() const
{
   std::string returnString = "Originating Simulation: " + mOriginatingSimulation.ToString() + '\n';
   returnString += "Received Entity:        " + mReceivedEntityId.ToString() + '\n';
   returnString += "Relationship Nature:    " + std::to_string(mRelationshipNature) + '\n';
   returnString += "Relationship Position:  " + std::to_string(mRelationshipPosition) + '\n';
   returnString += "   Relationship Position X:  " + std::to_string(mPartLocation[0]) + '\n';
   returnString += "   Relationship Position Y:  " + std::to_string(mPartLocation[1]) + '\n';
   returnString += "   Relationship Position Z:  " + std::to_string(mPartLocation[2]) + '\n';
   returnString += mNamedLocationIdentificationRecord.ToString() + '\n';
   returnString += mPartType.ToString();
   return returnString;
}

void DisIsPartOf::GetMemberData(GenI& aGenI)
{
   aGenI >> mOriginatingSimulation;
   aGenI >> mReceivedEntityId;
   aGenI >> mRelationshipNature;
   aGenI >> mRelationshipPosition;
   aGenI >> mPartLocation[0] >> mPartLocation[1] >> mPartLocation[2];
   mNamedLocationIdentificationRecord.Get(aGenI);
   aGenI >> mPartType;
   GetLength();
}
