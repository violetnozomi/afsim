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

#include "DisTransferOwnership.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

// Total TO PDU size = 320 bit + the sum of each Record
// Each Record is 128 + Record Length * Record Count + Padding (64bit Boundary)
// sBasePduSize list the base message size with Record Count == 0
// The PDU Header is 96 bits, 224 bits are for this packet
static const DisUint16 sMyBasePduSize = 28; // 28 bytes == 224 bits

DisTransferOwnership::DisTransferOwnership()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged)
   , mTransferType(0)
   , mTransferEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfRecordSets(0)
   , mSpecRecordList()
{
   SetPduType(DisEnum::Pdu::Type::TransferOwnership);
   SetProtocolFamily(DisEnum::Pdu::Family::EntityManagement);
   GetLength();
}

DisTransferOwnership::DisTransferOwnership(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged)
   , mTransferType(0)
   , mTransferEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfRecordSets(0)
   , mSpecRecordList()
{
   GetMemberData(aGenI);
}

DisTransferOwnership::DisTransferOwnership(GenI& aGenI)
   : DisPdu(aGenI)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged)
   , mTransferType(0)
   , mTransferEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfRecordSets(0)
   , mSpecRecordList()
{
   GetMemberData(aGenI);
}

// virtual
DisPdu* DisTransferOwnership::Clone() const
{
   return new DisTransferOwnership(*this);
}

// virtual
int DisTransferOwnership::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::TransferOwnership);
}

// virtual
void DisTransferOwnership::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisTransferOwnership::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sMyBasePduSize;

   mNumberOfRecordSets = mSpecRecordList.GetSize();
   std::vector<DisSpecRecord*>::const_iterator iter(mSpecRecordList.GetBegin());
   while (iter != mSpecRecordList.GetEnd())
   {
      byteCount += (*iter++)->GetLengthOfSet();
   }
   SetLength(byteCount);
   return byteCount;
}

void DisTransferOwnership::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mReliabilityService;
   aGenI >> mTransferType;
   aGenI >> mTransferEntity;
   aGenI >> mNumberOfRecordSets;

   mSpecRecordList.RemoveAll();
   mSpecRecordList.Reserve(mNumberOfRecordSets);
   for (unsigned int j = 0; j < mNumberOfRecordSets; ++j)
   {
      // Create a new record and add it to the list
      DisSpecRecord* disSpecRecord = new DisSpecRecord;
      mSpecRecordList.Add(disSpecRecord);

      // Read the data in the record
      aGenI >> *disSpecRecord;
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisTransferOwnership::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mReliabilityService;
   aGenO << mTransferType;
   aGenO << mTransferEntity;
   aGenO << mNumberOfRecordSets;

   std::vector<DisSpecRecord*>::const_iterator iter(mSpecRecordList.GetBegin());
   while (iter != mSpecRecordList.GetEnd())
   {
      // Write the data in the record
      aGenO << **iter++;
   }
}

void DisTransferOwnership::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      => Originating Id     = " << GetOriginatingEntity() << std::endl;
   aStream << "      => Receiving Id       = " << GetReceivingEntity() << std::endl;
   aStream << "      => Request Id         = " << static_cast<int>(GetRequestId()) << std::endl;
   aStream << "      => Reliability        = " << static_cast<int>(GetReliabilityService()) << " (Ack = 0, NoAck = 1)"
           << std::endl;
   aStream << "      => Transfer Type      = " << static_cast<int>(mTransferType) << std::endl;
   aStream << "      => Transfer Entity Id = " << mTransferEntity << std::endl;
   aStream << "      => Record Count       = " << static_cast<int>(mNumberOfRecordSets) << std::endl;
   aStream << "      === End DisTransferOwnership ===" << std::endl << std::endl;
}

const DisSpecRecord* DisTransferOwnership::GetRecordSet(unsigned int aIndex) const
{
   if (mSpecRecordList.GetSize() > aIndex)
   {
      return mSpecRecordList[aIndex];
   }
   return nullptr;
}

void DisTransferOwnership::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

void DisTransferOwnership::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

const DisEntityId& DisTransferOwnership::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

DisUint32 DisTransferOwnership::GetRequestId() const
{
   return mRequestId;
}

DisEnum8 DisTransferOwnership::GetReliabilityService() const
{
   return mReliabilityService;
}

bool DisTransferOwnership::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntity.IsValid())
      isValid = false;
   if (!mReceivingEntity.IsValid())
      isValid = false;
   if (!mTransferEntity.IsValid())
      isValid = false;

   auto iter(mSpecRecordList.GetBegin());
   while (iter != mSpecRecordList.GetEnd())
   {
      if (!(*iter++)->IsValid())
      {
         isValid = false;
         break;
      }
   }

   return isValid;
}
