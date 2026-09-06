// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisEnvironmentalProcess.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"

DisEnvironmentalProcess::DisEnvironmentalProcess()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::EnvironmentalProcess);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisEnvironmentalProcess::DisEnvironmentalProcess(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mEnvironmentalProcessId()
   , mEnvironmentalType()
   , mEnvironmentalRecords()
{
   GetMemberData(aGenI);
}

DisEnvironmentalProcess::DisEnvironmentalProcess(GenI& aGenI)
   : DisPdu(aGenI)
   , mEnvironmentalProcessId()
   , mEnvironmentalType()
   , mEnvironmentalRecords()
{
   GetMemberData(aGenI);
}

DisEnvironmentalProcess::DisEnvironmentalProcess(const DisEnvironmentalProcess& aSrc)
   : DisPdu(aSrc)
   , mEnvironmentalProcessId(aSrc.mEnvironmentalProcessId)
   , mEnvironmentalType(aSrc.mEnvironmentalType)
   , mModelType(aSrc.mModelType)
   , mEnvironmentalStatus(aSrc.mEnvironmentalStatus)
   , mNumberOfEnvironmentalRecords(aSrc.mNumberOfEnvironmentalRecords)
   , mSequenceNumber(aSrc.mSequenceNumber)
   , mEnvironmentalRecords(aSrc.mEnvironmentalRecords)
{
   SetPduType(DisEnum::Pdu::Type::EnvironmentalProcess);
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   GetLength();
}

DisEnvironmentalProcess* DisEnvironmentalProcess::Clone() const
{
   return new DisEnvironmentalProcess(*this);
}

int DisEnvironmentalProcess::GetClass() const
{
   return DisEnum::Pdu::Type::EnvironmentalProcess;
}

int DisEnvironmentalProcess::GetFamily() const
{
   return DisEnum::Pdu::Family::SyntheticEnvironment;
}

void DisEnvironmentalProcess::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEnvironmentalProcess::GetLength()
{
   DisUint16                                            length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   std::vector<DisEnvironmentalRecord*>::const_iterator recordVector(mEnvironmentalRecords.GetBegin());
   while (recordVector != mEnvironmentalRecords.GetEnd())
   {
      length += (*recordVector)->GetLength();
   }

   SetLength(length);

   return length;
}

void DisEnvironmentalProcess::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mEnvironmentalProcessId.Put(aGenO);
   mEnvironmentalType.Put(aGenO);
   aGenO << mModelType;
   aGenO << mEnvironmentalStatus;
   aGenO << mNumberOfEnvironmentalRecords;
   aGenO << mSequenceNumber;
   for (auto it = mEnvironmentalRecords.GetBegin(); it != mEnvironmentalRecords.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

const DisEntityId& DisEnvironmentalProcess::GetEnvironmentalProcessId() const
{
   return mEnvironmentalProcessId;
}

const DisEntityType& DisEnvironmentalProcess::GetEnvironmentalType() const
{
   return mEnvironmentalType;
}

DisEnum8 DisEnvironmentalProcess::GetModelType() const
{
   return mModelType;
}

DisEnum8 DisEnvironmentalProcess::GetEnvironmentalStatus() const
{
   return mEnvironmentalStatus;
}

DisUint16 DisEnvironmentalProcess::GetNumberOfEnvironmentalRecords() const
{
   return mNumberOfEnvironmentalRecords;
}

DisUint16 DisEnvironmentalProcess::GetSequenceNumber() const
{
   return mSequenceNumber;
}

void DisEnvironmentalProcess::SetEnvironmentalProcessId(const DisEntityId& aEnvironmentalProcessId)
{
   mEnvironmentalProcessId = aEnvironmentalProcessId;
}

void DisEnvironmentalProcess::SetEnvironmentalType(const DisEntityType& aEnvironmentalType)
{
   mEnvironmentalType = aEnvironmentalType;
}

void DisEnvironmentalProcess::SetModelType(DisEnum8 aModelType)
{
   mModelType = aModelType;
}

void DisEnvironmentalProcess::SetEnvironmentalStatus(DisEnum8 aEnvironmentalStatus)
{
   mEnvironmentalStatus = aEnvironmentalStatus;
}

void DisEnvironmentalProcess::SetNumberOfEnvironmentalRecords(DisUint16 aNumberOfEnvironmentalRecords)
{
   mNumberOfEnvironmentalRecords = aNumberOfEnvironmentalRecords;
}

void DisEnvironmentalProcess::SetSequenceNumber(DisUint16 aSequenceNumber)
{
   mSequenceNumber = aSequenceNumber;
}

void DisEnvironmentalProcess::AddEntityPart(DisEnvironmentalRecord* aParameter)
{
   mEnvironmentalRecords.Add(aParameter);
}

void DisEnvironmentalProcess::RemoveEntityPart(DisEnvironmentalRecord* aParameter)
{
   mEnvironmentalRecords.Remove(aParameter);
}

void DisEnvironmentalProcess::RemoveEntityPartNoDelete(DisEnvironmentalRecord* aParameter)
{
   mEnvironmentalRecords.RemoveWithNoDelete(aParameter);
}

void DisEnvironmentalProcess::RemoveAllEntityParts()
{
   mEnvironmentalRecords.RemoveAll();
}

void DisEnvironmentalProcess::RemoveAllEntityPartsNoDelete()
{
   mEnvironmentalRecords.RemoveAllWithNoDelete();
}

std::vector<DisEnvironmentalRecord*>::iterator DisEnvironmentalProcess::GetEnvironmentalRecordBegin()
{
   return mEnvironmentalRecords.GetBegin();
}

std::vector<DisEnvironmentalRecord*>::const_iterator DisEnvironmentalProcess::GetEnvironmentalRecordBegin() const
{
   return mEnvironmentalRecords.GetBegin();
}

std::vector<DisEnvironmentalRecord*>::iterator DisEnvironmentalProcess::GetEnvironmentalRecordEnd()
{
   return mEnvironmentalRecords.GetEnd();
}

std::vector<DisEnvironmentalRecord*>::const_iterator DisEnvironmentalProcess::GetEnvironmentalRecordEnd() const
{
   return mEnvironmentalRecords.GetEnd();
}

const DisEnvironmentalRecord* DisEnvironmentalProcess::GetEntityPartAtIndex(int aIndex) const
{
   return mEnvironmentalRecords[aIndex];
}

bool DisEnvironmentalProcess::IsValid() const
{
   return mEnvironmentalProcessId.IsValid() && mEnvironmentalType.IsValid() && mEnvironmentalRecords.IsValid();
}

std::string DisEnvironmentalProcess::GetStringId() const
{
   return "WsfEnvironmentalProcess";
}

std::string DisEnvironmentalProcess::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "-------Environmental Process PDU-------\n";
   returnString += mEnvironmentalProcessId.ToString() + '\n';
   returnString += mEnvironmentalType.ToString();
   returnString += "Model Type: " + std::to_string(mModelType) + '\n';
   returnString += "Environmental Status: " + std::to_string(mEnvironmentalStatus) + '\n';
   returnString += "Number Of Environmental Records: " + std::to_string(mNumberOfEnvironmentalRecords) + '\n';
   returnString += "Sequence Number: " + std::to_string(mSequenceNumber) + '\n';
   returnString += "-----End Environmental Process PDU-----\n";
   return returnString;
}

void DisEnvironmentalProcess::GetMemberData(GenI& aGenI)
{
   mEnvironmentalProcessId.Get(aGenI);
   mEnvironmentalType.Get(aGenI);
   aGenI >> mModelType;
   aGenI >> mEnvironmentalStatus;
   aGenI >> mNumberOfEnvironmentalRecords;
   aGenI >> mSequenceNumber;
   for (int i = 0; i < mNumberOfEnvironmentalRecords; i++)
   {
      DisEnvironmentalRecord* newRecord = new DisEnvironmentalRecord(aGenI);
      mEnvironmentalRecords.Add(newRecord);
   }
   GetLength();
}
