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

#include "DisIOCommunicationNodeIdRecord.hpp"

#include "DisValidationUtils.hpp"

DisIOCommunicationsNodeIdRecord::DisIOCommunicationsNodeIdRecord()
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID)
   , mEntityId()
   , mElementId(0)
{
}

DisIOCommunicationsNodeIdRecord::DisIOCommunicationsNodeIdRecord(const DisIOCommunicationsNodeIdRecord& aSrc)
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID)
   , mEntityId(aSrc.mEntityId)
   , mElementId(aSrc.mElementId)
{
}

DisIOCommunicationsNodeIdRecord::DisIOCommunicationsNodeIdRecord(const DisIOBaseRecord& aBaseRecord, GenI& aGenI)
   : DisIOBaseRecord(aBaseRecord)
{
   GetMemberData(aGenI);
}

DisIOCommunicationsNodeIdRecord::DisIOCommunicationsNodeIdRecord(GenI& aGenI)
   : DisIOBaseRecord(DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID)
   , mElementId(0)
{
   Get(aGenI);
}

DisIOCommunicationsNodeIdRecord* DisIOCommunicationsNodeIdRecord::Clone() const
{
   return new DisIOCommunicationsNodeIdRecord(*this);
}

DisIORecordTypeEnum DisIOCommunicationsNodeIdRecord::GetIORecordType() const
{
   return DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID;
}

const char* DisIOCommunicationsNodeIdRecord::GetScriptClassName() const
{
   return "DisIoCommunicationNodeIdRecord";
}

void DisIOCommunicationsNodeIdRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisIOCommunicationsNodeIdRecord::GetLength()
{
   return cRECORD_SIZE_IN_OCTETS;
}

void DisIOCommunicationsNodeIdRecord::Put(GenO& aGenO) const
{
   DisIOBaseRecord::Put(aGenO);
   mEntityId.Put(aGenO);
   aGenO << mElementId;
}

void DisIOCommunicationsNodeIdRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

std::string DisIOCommunicationsNodeIdRecord::ToString() const
{
   std::string returnString = DisIOBaseRecord::ToString();
   returnString += "-------Communication Node Id Record-------\n";
   returnString += "Entity Id: " + mEntityId.ToString() + "\n";
   returnString += "Element Id: " + std::to_string(mElementId) + "\n";
   returnString += "-----End Communication Node Id Record-----\n";
   return returnString;
}

bool DisIOCommunicationsNodeIdRecord::operator==(const DisIOCommunicationsNodeIdRecord& aOther) const
{
   const DisIOCommunicationsNodeIdRecord* other = dynamic_cast<const DisIOCommunicationsNodeIdRecord*>(&aOther);

   if (!other)
   {
      return false;
   }
   return mEntityId == other->mEntityId && mElementId == other->mElementId;
}

bool DisIOCommunicationsNodeIdRecord::IsValid() const
{
   return DisIOBaseRecord::IsValid() && mEntityId.IsValid();
}

void DisIOCommunicationsNodeIdRecord::SetEntityId(const DisEntityId aEntityId)
{
   mEntityId = aEntityId;
}

void DisIOCommunicationsNodeIdRecord::SetElementId(const DisUint16 aElementId)
{
   mElementId = aElementId;
}

DisEntityId DisIOCommunicationsNodeIdRecord::GetEntityId() const
{
   return mEntityId;
}

DisUint16 DisIOCommunicationsNodeIdRecord::GetElementId() const
{
   return mElementId;
}

void DisIOCommunicationsNodeIdRecord::GetMemberData(GenI& aGenI)
{
   DisIOBaseRecord::Get(aGenI);
   mEntityId.Get(aGenI);
   aGenI >> mElementId;
}
