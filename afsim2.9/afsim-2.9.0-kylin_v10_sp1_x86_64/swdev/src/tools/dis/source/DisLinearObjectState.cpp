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

#include "DisLinearObjectState.hpp"

#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisLinearObjectState::DisLinearObjectState()
   : DisPdu()
   , mRequesterSimulationId()
   , mReceivingSimulationId()
   , mObjectType()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetPduType(DisEnum::Pdu::Type::LinearObjectState);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisLinearObjectState::DisLinearObjectState(const DisLinearObjectState& aSrc)
   : DisPdu(aSrc)
   , mObjectId(aSrc.mObjectId)
   , mReferencedObjectId(aSrc.mReferencedObjectId)
   , mUpdateNumber(aSrc.mUpdateNumber)
   , mForceId(aSrc.mForceId)
   , mNumberOfLinearSegments(aSrc.mNumberOfLinearSegments)
   , mRequesterSimulationId(aSrc.mRequesterSimulationId)
   , mReceivingSimulationId(aSrc.mReceivingSimulationId)
   , mObjectType(aSrc.mObjectType)
   , mLinearSegmentParameters(aSrc.mLinearSegmentParameters)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetPduType(DisEnum::Pdu::Type::LinearObjectState);
   GetLength();
}

DisLinearObjectState::DisLinearObjectState(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mRequesterSimulationId()
   , mReceivingSimulationId()
   , mObjectType()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SyntheticEnvironment);
   SetPduType(DisEnum::Pdu::Type::LinearObjectState);
   GetMemberData(aGenI);
}

DisLinearObjectState* DisLinearObjectState::Clone() const
{
   return new DisLinearObjectState(*this);
}

int DisLinearObjectState::GetClass() const
{
   return DisEnum::Pdu::Type::LinearObjectState;
}

int DisLinearObjectState::GetFamily() const
{
   return DisEnum::Pdu::Family::SyntheticEnvironment;
}

void DisLinearObjectState::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisLinearObjectState::GetLength()
{
   DisUint16                                               length = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   std::vector<DisLinearSegmentParameterRecord*>::iterator iterator(mLinearSegmentParameters.GetBegin());
   while (iterator != mLinearSegmentParameters.GetEnd())
   {
      length += (*iterator++)->GetLength();
   }

   SetLength(length);
   return length;
}

void DisLinearObjectState::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   mObjectId.Put(aGenO);
   mReferencedObjectId.Put(aGenO);
   aGenO << mUpdateNumber;
   aGenO << mForceId;
   aGenO << mNumberOfLinearSegments;
   mRequesterSimulationId.Put(aGenO);
   mReceivingSimulationId.Put(aGenO);
   mObjectType.Put(aGenO);
   for (auto it = mLinearSegmentParameters.GetBegin(); it != mLinearSegmentParameters.GetEnd(); it++)
   {
      (**it).Put(aGenO);
   }
}

const DisEntityId DisLinearObjectState::GetObjectId() const
{
   return mObjectId;
}

const DisEntityId DisLinearObjectState::GetReferencedObjectId() const
{
   return mReferencedObjectId;
}

DisUint16 DisLinearObjectState::GetUpdateNumber() const
{
   return mUpdateNumber;
}

DisEnum8 DisLinearObjectState::GetForceId() const
{
   return mForceId;
}

DisUint8 DisLinearObjectState::GetNumberOfLinearSegments() const
{
   return mNumberOfLinearSegments;
}

const DisSimulationAddressRecord DisLinearObjectState::GetRequesterSimulationId() const
{
   return mRequesterSimulationId;
}

const DisSimulationAddressRecord DisLinearObjectState::GetReceivingSimulationId() const
{
   return mReceivingSimulationId;
}

const DisObjectType DisLinearObjectState::GetObjectType() const
{
   return mObjectType;
}

DisPtrContainer<DisLinearSegmentParameterRecord>* DisLinearObjectState::GetLinearSegmentParameterRecords()
{
   return &mLinearSegmentParameters;
}

const DisLinearSegmentParameterRecord* DisLinearObjectState::GetLinearSegmentParameterRecordAtIndex(int aIndex)
{
   const DisLinearSegmentParameterRecord* recordPtr{nullptr};
   if (aIndex >= 0 && aIndex < static_cast<int>(mLinearSegmentParameters.GetSize()))
   {
      return mLinearSegmentParameters[aIndex];
   }

   return recordPtr;
}

void DisLinearObjectState::SetObjectId(const DisEntityId& aObjectId)
{
   mObjectId = aObjectId;
}

void DisLinearObjectState::SetReferenceObjectId(const DisEntityId& aObjectReferenceId)
{
   mReferencedObjectId = aObjectReferenceId;
}

void DisLinearObjectState::SetUpdateNumber(DisUint16 aUpdateNumber)
{
   mUpdateNumber = aUpdateNumber;
}

void DisLinearObjectState::SetForceId(DisEnum8 aForceId)
{
   mForceId = aForceId;
}

void DisLinearObjectState::SetNumberOfLinearSegments(DisEnum8 aNumberOfLineSegments)
{
   mNumberOfLinearSegments = aNumberOfLineSegments;
}

void DisLinearObjectState::SetObjectType(const DisObjectType& aObjectType)
{
   mObjectType = aObjectType;
}

void DisLinearObjectState::SetRequesterSimulationId(const DisSimulationAddressRecord& aRequesterSimulationId)
{
   mRequesterSimulationId = aRequesterSimulationId;
}

void DisLinearObjectState::SetReceivingSimulationId(const DisSimulationAddressRecord& aRecievingSimulationId)
{
   mReceivingSimulationId = aRecievingSimulationId;
}

void DisLinearObjectState::AddLinearSegmentParameterRecord(DisLinearSegmentParameterRecord* aLinearSegmentParameterRecord)
{
   mLinearSegmentParameters.Add(aLinearSegmentParameterRecord);
}

bool DisLinearObjectState::IsValid() const
{
   bool isValid = DisPdu::IsValid() && mObjectId.IsValid() && mReferencedObjectId.IsValid() && mObjectType.IsValid() &&
                  mRequesterSimulationId.IsValid() && mReceivingSimulationId.IsValid();
   for (auto it = mLinearSegmentParameters.GetBegin(); it != mLinearSegmentParameters.GetEnd(); it++)
   {
      isValid &= (*it)->IsValid();
   }
   return isValid;
}

std::string DisLinearObjectState::GetStringId() const
{
   return "DisLinearObjectState";
}

std::string DisLinearObjectState::ToString() const
{
   std::string returnString = "-------Linear Object State-------\n";
   returnString += "Object Id: \n";
   returnString += mObjectId.ToString() + '\n';
   returnString += "Object Reference Id: \n";
   returnString += mReferencedObjectId.ToString() + '\n';
   returnString += "Update Number: " + std::to_string(mUpdateNumber) + '\n';
   returnString += "Force Id: " + std::to_string(mForceId) + '\n';
   returnString += "Number of linear segments: " + std::to_string(mNumberOfLinearSegments) + '\n';
   returnString += "Requester Simulation Id: \n";
   returnString += mRequesterSimulationId.ToString() + '\n';
   returnString += "Receiving Simulation Id: \n";
   returnString += mReceivingSimulationId.ToString() + '\n';
   returnString += "-------Linear Segment Parameters-------\n";
   int index = 1;
   for (auto it = mLinearSegmentParameters.GetBegin(); it != mLinearSegmentParameters.GetEnd(); it++)
   {
      returnString += "Parameter: " + std::to_string(index) + '\n' + (**it).ToString(mObjectType) +
                      "\n End Parameter: " + std::to_string(index) + '\n';
      index++;
   }
   returnString += "-----End Linear Segment Parameters-----\n";
   returnString += "-----End Linear Object State-----\n";
   return returnString;
}

void DisLinearObjectState::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

void DisLinearObjectState::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   mObjectId.Get(aGenI);
   mReferencedObjectId.Get(aGenI);
   aGenI >> mUpdateNumber;
   aGenI >> mForceId;
   aGenI >> mNumberOfLinearSegments;
   mRequesterSimulationId.Get(aGenI);
   mReceivingSimulationId.Get(aGenI);
   mObjectType.Get(aGenI);
   for (int i = 0; i < mNumberOfLinearSegments; i++)
   {
      mLinearSegmentParameters.Add(new DisLinearSegmentParameterRecord(aGenI));
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}
