//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisIOCommNodeRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisIOCommNodeRecord::DisIOCommNodeRecord()
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_COMM_NODE)
   , mCommNodeType((DisEnum8)DisIORecordTypeEnum::IO_COMM_NODE)
   , mCommsNodeId()
{
   SetRecordLength(cRECORD_SIZE_IN_OCTETS);
}

DisIOCommNodeRecord::DisIOCommNodeRecord(const DisIOCommNodeRecord& aSrc)
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_COMM_NODE)
   , mCommNodeType(aSrc.mCommNodeType)
   , mCommsNodeId(aSrc.mCommsNodeId)
{
   SetRecordLength(cRECORD_SIZE_IN_OCTETS);
}

DisIOCommNodeRecord::DisIOCommNodeRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI)
   : DisIOBaseRecord(aBaseRecord)
{
   GetMemberData(aGenI);
}

DisIOCommNodeRecord::DisIOCommNodeRecord(GenI& aGenI)
   : DisIOBaseRecord()
{
   Get(aGenI);
}

DisIOCommNodeRecord* DisIOCommNodeRecord::Clone() const
{
   return new DisIOCommNodeRecord(*this);
}

DisIORecordTypeEnum DisIOCommNodeRecord::GetIORecordType() const
{
   return DisIORecordTypeEnum::IO_COMM_NODE;
}

const char* DisIOCommNodeRecord::GetScriptClassName() const
{
   return "DisIoCommNodeRecord";
}

void DisIOCommNodeRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisIOCommNodeRecord::GetLength()
{
   return cRECORD_SIZE_IN_OCTETS;
}


void DisIOCommNodeRecord::Put(GenO& aGenO) const
{
   DisUint8 pad8(0);

   DisIOBaseRecord::Put(aGenO);
   aGenO << mCommNodeType;
   aGenO << pad8;
   mCommsNodeId.Put(aGenO);
}

std::string DisIOCommNodeRecord::ToString() const
{
   std::string returnString = DisIOBaseRecord::ToString();
   returnString += "IO COMMUNICATIONS NODE\n";
   returnString += "Communications Node Type   = " + std::to_string(mCommNodeType) + "\n";
   returnString += mCommsNodeId.ToString() + "\n";
   returnString += "End IO COMMUNICATIONS NODE\n";
   return returnString;
}

void DisIOCommNodeRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

bool DisIOCommNodeRecord::IsValid() const
{
   return DisRecordHeader::IsValid() && mCommsNodeId.IsValid();
}

bool DisIOCommNodeRecord::operator==(const DisRecordHeader& aOther) const
{
   const DisIOCommNodeRecord* other = dynamic_cast<const DisIOCommNodeRecord*>(&aOther);
   if (!other)
   {
      return false;
   }
   return mCommNodeType == other->mCommNodeType && mCommsNodeId == other->mCommsNodeId;
}

void DisIOCommNodeRecord::SetCommNodeType(DisEnum8 aCommNodeType)
{
   mCommNodeType = aCommNodeType;
}

void DisIOCommNodeRecord::SetCommsNodeId(const DisIOCommunicationsNodeIdRecord aCommsNodeId)
{
   mCommsNodeId.SetElementId(aCommsNodeId.GetElementId());
   mCommsNodeId.SetEntityId(aCommsNodeId.GetEntityId());
   mCommsNodeId.SetRecordLength(aCommsNodeId.GetRecordLength());
   mCommsNodeId.SetRecordType(aCommsNodeId.GetRecordType());
}

DisUint32 DisIOCommNodeRecord::GetCommNodeType() const
{
   return mCommNodeType;
}

DisIOCommunicationsNodeIdRecord DisIOCommNodeRecord::GetCommsNodeId() const
{
   return mCommsNodeId;
}

void DisIOCommNodeRecord::GetMemberData(GenI& aGenI)
{
   DisIOBaseRecord::Get(aGenI);
   DisUint8 pad8(0);
   aGenI >> mCommNodeType;
   aGenI >> pad8;
   mCommsNodeId.Get(aGenI);
}
