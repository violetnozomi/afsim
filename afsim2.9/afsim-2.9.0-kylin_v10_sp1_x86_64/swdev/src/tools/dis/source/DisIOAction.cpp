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

#include "DisIOAction.hpp"

#include "DisIOCommNodeRecord.hpp"
#include "DisIOCommunicationNodeIdRecord.hpp"
#include "DisIOEffectRecord.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtException.hpp"

DisIOAction::DisIOAction()
   : DisPdu()
{
   SetPduType(DisEnum::Pdu::Type::InformationOperationsAction);
   SetProtocolFamily(DisEnum::Pdu::Family::InformationOperations);
   GetLength();
}

DisIOAction::DisIOAction(const DisIOAction& aSrc)
   : DisPdu(aSrc)
   , mOriginSimulationId(aSrc.mOriginSimulationId)
   , mReceivingSimulationId(aSrc.mReceivingSimulationId)
   , mRequestId(aSrc.mRequestId)
   , mIOWarfareType(aSrc.mIOWarfareType)
   , mIOSimulationSource(aSrc.mIOSimulationSource)
   , mIOActionType(aSrc.mIOActionType)
   , mIOActionPhase(aSrc.mIOActionPhase)
   , mIOAttackerEntityID(aSrc.mIOAttackerEntityID)
   , mIOPrimaryTargetEntityID(aSrc.mIOPrimaryTargetEntityID)
   , mRecordCount(aSrc.mRecordCount)
   , mRecords(aSrc.mRecords)
{
   SetPduType(DisEnum::Pdu::Type::InformationOperationsAction);
   SetProtocolFamily(DisEnum::Pdu::Family::InformationOperations);
   GetLength();
}

DisIOAction::DisIOAction(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisIOAction::DisIOAction(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisIOAction* DisIOAction::Clone() const
{
   return new DisIOAction(*this);
}

void DisIOAction::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisIOAction::ToString() const
{
   std::string returnString = DisPdu::ToString();
   returnString += "-IO Action PDU-\n";
   returnString += "Receiving Entity ID:\n";
   returnString += mReceivingSimulationId.ToString();
   returnString += "Request ID:   " + std::to_string(static_cast<unsigned int>(mRequestId)) + '\n';
   returnString += "Warfare Type: " + std::to_string(static_cast<unsigned int>(mIOWarfareType)) + '\n';
   returnString += "Simulation Source: " + std::to_string(static_cast<unsigned int>(mIOSimulationSource)) + '\n';
   returnString += "Action Type: " + std::to_string(static_cast<unsigned int>(mIOActionType)) + '\n';
   returnString += "Action Phase: " + std::to_string(static_cast<unsigned int>(mIOActionPhase)) + '\n';
   returnString += "Attacker ID:\n";
   returnString += mOriginSimulationId.ToString();
   returnString += "Primary Target ID:\n";
   returnString += mIOPrimaryTargetEntityID.ToString();
   returnString += "Standard Variables:\n";

   if (mRecordCount > 0)
   {
      returnString += PrintRecords();
   }
   returnString += "-End IO Action PDU-\n";
   return returnString;
}

std::string DisIOAction::PrintRecords() const
{
   std::string returnString;
   for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
   {
      returnString += (**it).ToString() + '\n';
   }
   return returnString;
}

const char* DisIOAction::GetScriptClassName() const
{
   return "DisIOActionPdu";
}

void DisIOAction::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIOAction::GetLength()
{
   DisUint16 octets = DisPdu::GetBaseLength() + cBASE_LENGTH_OCTETS;
   for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
   {
      octets += (**it).GetLength();
   }
   SetLength(octets);
   return octets;
}

void DisIOAction::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);

   DisUint32 pad32(0);
   DisUint16 pad16(0);

   mOriginSimulationId.Put(aGenO);
   mReceivingSimulationId.Put(aGenO);
   aGenO << mRequestId;
   aGenO << mIOWarfareType;
   aGenO << mIOSimulationSource;
   aGenO << mIOActionType;
   aGenO << mIOActionPhase;
   aGenO << pad32;
   aGenO << mIOAttackerEntityID;
   aGenO << mIOPrimaryTargetEntityID;
   aGenO << pad16;
   aGenO << mRecordCount;
   if (mRecordCount > 0)
   {
      PutRecords(aGenO);
   }
}

bool DisIOAction::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mOriginSimulationId.IsValid() && mReceivingSimulationId.IsValid() &&
                  mIOAttackerEntityID.IsValid() && mIOPrimaryTargetEntityID.IsValid();

   if (mRecordCount > 0)
   {
      for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
      {
         isValid &= (*it)->IsValid();
      }
   }
   return isValid;
}

void DisIOAction::PutRecords(GenO& aGenO)
{
   for (auto it = mRecords.GetBegin(); it != mRecords.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

void DisIOAction::GetRecords(GenI& aGenI)
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

void DisIOAction::GetMemberData(GenI& aGenI)
{
   DisUint32 pad32(0);
   DisUint16 pad16(0);

   mOriginSimulationId.Get(aGenI);
   mReceivingSimulationId.Get(aGenI);
   aGenI >> mRequestId;
   aGenI >> mIOWarfareType;
   aGenI >> mIOSimulationSource;
   aGenI >> mIOActionType;
   aGenI >> mIOActionPhase;
   aGenI >> pad32;
   aGenI >> mIOAttackerEntityID;
   aGenI >> mIOPrimaryTargetEntityID;
   aGenI >> pad16;
   aGenI >> mRecordCount;
   if (mRecordCount > 0)
   {
      GetRecords(aGenI);
   }
}

bool DisIOAction::AddRecord(DisIOBaseRecord* aRecord)
{
   bool added{aRecord != nullptr};
   if (added)
   {
      mRecords.Add(aRecord);
      mRecordCount++;
   }
   return added;
}

int DisIOAction::GetClass() const
{
   return static_cast<int>(DisEnum::Pdu::Type::InformationOperationsAction);
}

int DisIOAction::GetFamily() const
{
   return static_cast<int>(DisEnum::Pdu::Family::InformationOperations);
}

std::string DisIOAction::GetStringId() const
{
   return "DisIOAction";
}

std::vector<DisIOBaseRecord*>::iterator DisIOAction::GetRecordsBegin()
{
   return mRecords.GetBegin();
}

std::vector<DisIOBaseRecord*>::const_iterator DisIOAction::GetRecordsBegin() const
{
   return mRecords.GetBegin();
}

std::vector<DisIOBaseRecord*>::iterator DisIOAction::GetRecordsEnd()
{
   return mRecords.GetEnd();
}

std::vector<DisIOBaseRecord*>::const_iterator DisIOAction::GetRecordsEnd() const
{
   return mRecords.GetEnd();
}

// Accessors
const DisEntityId& DisIOAction::GetOriginatingEntity() const
{
   return *(mOriginSimulationId.ToEntityId());
}

const DisSimulationId& DisIOAction::GetOriginSimulationId() const
{
   return mOriginSimulationId;
}

const DisSimulationId& DisIOAction::GetReceivingSimulationId() const
{
   return mReceivingSimulationId;
}

DisUint32 DisIOAction::GetRequestId() const
{
   return mRequestId;
}

DisEnum16 DisIOAction::GetIOWarfareType() const
{
   return mIOWarfareType;
}

DisEnum16 DisIOAction::GetIOSimulationSource() const
{
   return mIOSimulationSource;
}

DisEnum16 DisIOAction::GetIOActionType() const
{
   return mIOActionType;
}

DisEnum16 DisIOAction::GetIOActionPhase() const
{
   return mIOActionPhase;
}

const DisEntityId& DisIOAction::GetIOAttackerEntityId() const
{
   return mIOAttackerEntityID;
}

const DisEntityId& DisIOAction::GetIOPrimaryTargetEntityId() const
{
   return mIOPrimaryTargetEntityID;
}

DisUint16 DisIOAction::GetRecordsCount() const
{
   return mRecordCount;
}

const DisIOBaseRecord* DisIOAction::GetRecordAtIndex(int aIndex) const
{
   const DisIOBaseRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mRecords.GetSize()))
   {
      return mRecords[aIndex];
   }

   return recordPtr;
}

// Mutators

void DisIOAction::SetOriginSimulationId(const DisSimulationId& aSimId)
{
   mOriginSimulationId = aSimId;
}

void DisIOAction::SetReceivingSimulationId(const DisSimulationId& aSimId)
{
   mReceivingSimulationId = aSimId;
}

void DisIOAction::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

void DisIOAction::SetIOWarfareType(DisEnum16 aType)
{
   mIOWarfareType = aType;
}

void DisIOAction::SetIOSimulationSource(DisEnum16 aSource)
{
   mIOSimulationSource = aSource;
}

void DisIOAction::SetIOActionType(DisEnum16 aType)
{
   mIOActionType = aType;
}

void DisIOAction::SetIOActionPhase(DisEnum16 aPhase)
{
   mIOActionPhase = aPhase;
}

void DisIOAction::SetIOAttackerEntityId(const DisEntityId& aEntityId)
{
   mIOAttackerEntityID = aEntityId;
}

void DisIOAction::SetIOPrimaryTargetEntityId(const DisEntityId& aEntityId)
{
   mIOPrimaryTargetEntityID = aEntityId;
}
