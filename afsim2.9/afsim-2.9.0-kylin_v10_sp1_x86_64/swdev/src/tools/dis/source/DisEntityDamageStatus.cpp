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

#include "DisEntityDamageStatus.hpp"

#include "DisDEDamageDescriptionRecord.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisEntityDamageStatus::DisEntityDamageStatus()
   : DisPdu()
   , mDamagedEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfDDRecords(0)
{
   SetPduType(DisEnum::Pdu::Type::DirectedEnergyDamageStatus);
   SetProtocolFamily(DisEnum::Pdu::Family::Warfare);
   GetLength();
}

DisEntityDamageStatus::DisEntityDamageStatus(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mDamagedEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfDDRecords(0)
{
   GetMemberData(aGenI);
}

DisEntityDamageStatus::DisEntityDamageStatus(GenI& aGenI)
   : DisPdu(aGenI)
   , mDamagedEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mNumberOfDDRecords(0)
{
   GetMemberData(aGenI);
}

DisEntityDamageStatus::~DisEntityDamageStatus() {}

DisPdu* DisEntityDamageStatus::Clone() const
{
   return new DisEntityDamageStatus(*this);
}

// Virtual
int DisEntityDamageStatus::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::DirectedEnergyDamageStatus);
}

void DisEntityDamageStatus::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisEntityDamageStatus::GetMemberData(aGenI);
}

DisUint16 DisEntityDamageStatus::GetLength()
{
   UpdateDDRecordCount();

   DisUint16 length = DisPdu::GetBaseLength() + 12;

   std::vector<DisDDRecord*>::const_iterator iter(mDDRecordList.GetBegin());
   while (iter != mDDRecordList.GetEnd())
   {
      length += (*iter++)->GetLength();
   }

   SetLength(length);
   return length;
}

void DisEntityDamageStatus::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mDamagedEntity;
   DisUint16 pad16;
   aGenI >> pad16;
   aGenI >> pad16; // Skip 32-bits of padding
   aGenI >> mNumberOfDDRecords;
   lengthToRead -= DisPdu::GetBaseLength();
   lengthToRead -= 12; // length of fixed portion

   RemoveAllDDRecords();

   int recordType = 0;
   mDDRecordList.Reserve(mNumberOfDDRecords);
   for (int i = 0; i < mNumberOfDDRecords; ++i)
   {
      DisDDRecord  recordHeader(aGenI);
      DisDDRecord* recordPtr;
      recordType = recordHeader.GetRecordType();
      switch (recordType)
      {
      case DisDDRecord::DAMAGE_DESCRIPTION_DD_RECORD_TYPE:
      {
         recordPtr = new DisDEDamageDescriptionRecord(recordHeader, aGenI);
      }
      break;
      default:
      {
         recordPtr = nullptr;
      }
      break;
      }
      if (recordPtr != nullptr)
      {
         mDDRecordList.Add(recordPtr); // List takes ownership
         lengthToRead -= recordPtr->GetLength();
      }
      else
      {
         DisUint16 pad16;
         aGenI >> pad16;
         aGenI >> pad16; // Skip 32-bits of padding
         lengthToRead -= recordHeader.GetLength();
      }
   }

   ReadExtraData(aGenI, lengthToRead);
}

void DisEntityDamageStatus::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mDamagedEntity;
   aGenO << DisUint16(0); // 16-bits of padding
   aGenO << DisUint16(0); // 16-bi6s of padding
   aGenO << mNumberOfDDRecords;

   std::vector<DisDDRecord*>::const_iterator iter(mDDRecordList.GetBegin());
   while (iter != mDDRecordList.GetEnd())
   {
      (*iter++)->Put(aGenO);
   }
}

bool DisEntityDamageStatus::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mDamagedEntity.IsValid())
      isValid = false;

   std::vector<DisDDRecord*>::const_iterator iter(mDDRecordList.GetBegin());
   while (iter != mDDRecordList.GetEnd())
   {
      if (!(*iter++)->IsValid())
         isValid = false;
   }

   return isValid;
}
