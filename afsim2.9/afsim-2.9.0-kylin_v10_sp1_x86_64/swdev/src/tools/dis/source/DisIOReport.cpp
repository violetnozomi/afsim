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

#include "DisIOReport.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtCast.hpp"
#include "UtException.hpp"

DisIOReport::DisIOReport()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::InformationOperationsReport);
   SetProtocolFamily(DisEnum::Pdu::Family::InformationOperations);
   GetLength();
}

DisIOReport::DisIOReport(const DisIOReport& aSrc)
   : DisPdu(aSrc)
   , mOriginSimulationId(aSrc.mOriginSimulationId)
   , mIOSimulationSource(aSrc.mIOSimulationSource)
   , mIOReportType(aSrc.mIOReportType)
   , mIOAttackerEntityID(aSrc.mIOAttackerEntityID)
   , mIOPrimaryTargetEntityID(aSrc.mIOPrimaryTargetEntityID)
   , mRecordCount(aSrc.mRecordCount)
   , mRecords(aSrc.mRecords)
{
   SetPduType(DisEnum::Pdu::Type::InformationOperationsReport);
   SetProtocolFamily(DisEnum::Pdu::Family::InformationOperations);
   GetLength();
}

DisIOReport::DisIOReport(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisIOReport::DisIOReport(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisIOReport* DisIOReport::Clone() const
{
   return new DisIOReport(*this);
}

void DisIOReport::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisIOReport::ToString() const
{
   std::string returnString = "";
   returnString += "-IO Report PDU-\n";
   returnString += "Simulation Source: " + std::to_string(static_cast<unsigned int>(mIOSimulationSource)) + "\n";
   returnString += "Report     Type: " + std::to_string(static_cast<unsigned int>(mIOReportType)) + "\n";
   returnString += "Attacker ID:\n";
   returnString += mIOAttackerEntityID.ToString();
   returnString += "Primary Target ID:\n";
   returnString += mIOPrimaryTargetEntityID.ToString();
   returnString += "Number of standard variable records: " + std::to_string(static_cast<unsigned int>(mRecordCount)) + "\n";
   if (mRecordCount > 0)
   {
      returnString += PrintRecords();
   }

   return returnString;
}

std::string DisIOReport::PrintRecords() const
{
   std::string returnString = "";
   for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
   {
      returnString += (**it).ToString();
   }
   return returnString;
}

void DisIOReport::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIOReport::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cBASE_LENGTH_OCTETS;
   if (mRecordCount > 0)
   {
      for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
      {
         octets += (**it).GetLength();
      }
   }
   SetLength(octets);
   return octets;
}

void DisIOReport::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint32 pad8(0);
   DisUint16 pad16(0);

   mOriginSimulationId.Put(aGenO);
   aGenO << mIOSimulationSource;
   aGenO << mIOReportType;
   aGenO << pad8;
   aGenO << mIOAttackerEntityID;
   aGenO << mIOPrimaryTargetEntityID;
   aGenO << pad16;
   aGenO << pad16;
   aGenO << mRecordCount;
   if (mRecordCount > 0)
   {
      PutRecords(aGenO);
   }
}

bool DisIOReport::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mOriginSimulationId.IsValid() && mIOAttackerEntityID.IsValid() &&
                  mIOPrimaryTargetEntityID.IsValid();


   if (isValid)
   {
      if (mRecordCount > 0)
      {
         for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
         {
            isValid &= (**it).IsValid();
         }
      }
   }
   return isValid;
}

void DisIOReport::PutRecords(GenO& aGenO)
{
   for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

void DisIOReport::GetRecords(GenI& aGenI)
{
   mRecords.RemoveAll();

   for (int i = 0; i < mRecordCount; i++)
   {
      DisIOBaseRecord baseRecord(aGenI);
      if (baseRecord.GetIORecordType() == DisIORecordTypeEnum::IO_COMM_NODE)
      {
         DisIOCommNodeRecord* commNodeRecord = new DisIOCommNodeRecord(baseRecord, aGenI);
         mRecords.Add(commNodeRecord);
      }
      else if (baseRecord.GetIORecordType() == DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID)
      {
         DisIOCommunicationsNodeIdRecord* commNodeIdRecord = new DisIOCommunicationsNodeIdRecord(baseRecord, aGenI);
         mRecords.Add(commNodeIdRecord);
      }
      else if (baseRecord.GetIORecordType() == DisIORecordTypeEnum::IO_EFFECT)
      {
         DisIOEffectRecord* ioEffectRecord = new DisIOEffectRecord(baseRecord, aGenI);
         mRecords.Add(ioEffectRecord);
      }
      else
      {
         throw UtException("Invalid Record Type");
      }
   }
}

void DisIOReport::GetMemberData(GenI& aGenI)
{
   DisUint32 pad8(0);
   DisUint16 pad16(0);

   mOriginSimulationId.Get(aGenI);
   aGenI >> mIOSimulationSource;
   aGenI >> mIOReportType;
   aGenI >> pad8;
   aGenI >> mIOAttackerEntityID;
   aGenI >> mIOPrimaryTargetEntityID;
   aGenI >> pad16;
   aGenI >> pad16;
   aGenI >> mRecordCount;
   if (mRecordCount > 0)
   {
      GetRecords(aGenI);
   }
}

bool DisIOReport::AddRecord(DisIOBaseRecord* aRecord)
{
   bool added{aRecord != nullptr};
   if (added)
   {
      mRecords.Add(aRecord);
      mRecordCount++;
   }
   return added;
}

int DisIOReport::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::InformationOperationsReport);
}

int DisIOReport::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::InformationOperations);
}

const char* DisIOReport::GetScriptClassName() const
{
   return "DisIOReportPdu";
}

std::string DisIOReport::GetStringId() const
{
   return "DisIOReport";
}

std::vector<DisIOBaseRecord*>::const_iterator DisIOReport::GetRecordsBegin() const
{
   return mRecords.GetBegin();
}

std::vector<DisIOBaseRecord*>::iterator DisIOReport::GetRecordsBegin()
{
   return mRecords.GetBegin();
}

std::vector<DisIOBaseRecord*>::const_iterator DisIOReport::GetRecordsEnd() const
{
   return mRecords.GetEnd();
}

std::vector<DisIOBaseRecord*>::iterator DisIOReport::GetRecordsEnd()
{
   return mRecords.GetEnd();
}

// Accessors

const DisEntityId& DisIOReport::GetOriginatingEntity() const
{
   return *(mOriginSimulationId.ToEntityId());
}

const DisSimulationId& DisIOReport::GetOriginSimulationId() const
{
   return mOriginSimulationId;
}

DisEnum16 DisIOReport::GetIOSimulationSource() const
{
   return mIOSimulationSource;
}

DisEnum8 DisIOReport::GetIOReportType() const
{
   return mIOReportType;
}

const DisEntityId& DisIOReport::GetIOAttackerEntityId() const
{
   return mIOAttackerEntityID;
}

const DisEntityId& DisIOReport::GetIOPrimaryTargetEntityId() const
{
   return mIOPrimaryTargetEntityID;
}

DisUint16 DisIOReport::GetRecordsCount() const
{
   return mRecordCount;
}

const DisIOBaseRecord* DisIOReport::GetRecordAtIndex(int aIndex)
{
   const DisIOBaseRecord* recordPtr{nullptr};
   if (aIndex >= 0 && ut::cast_to_size_t(aIndex) < mRecords.GetSize())
   {
      recordPtr = mRecords[aIndex];
   }

   return recordPtr;
}

// Mutators

void DisIOReport::SetOriginSimulationId(const DisSimulationId& aSimId)
{
   mOriginSimulationId = aSimId;
}

void DisIOReport::SetIOSimulationSource(DisEnum16 aSource)
{
   mIOSimulationSource = aSource;
}

void DisIOReport::SetIOReportType(DisEnum8 aType)
{
   mIOReportType = aType;
}

void DisIOReport::SetIOAttackerEntityId(const DisEntityId& aEntityId)
{
   mIOAttackerEntityID = aEntityId;
}

void DisIOReport::SetIOPrimaryTargetEntityId(const DisEntityId& aEntityId)
{
   mIOPrimaryTargetEntityID = aEntityId;
}
