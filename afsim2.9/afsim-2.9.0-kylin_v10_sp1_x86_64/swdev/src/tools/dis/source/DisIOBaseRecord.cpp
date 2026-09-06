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

#include "DisIOBaseRecord.hpp"

DisIOBaseRecord::DisIOBaseRecord(GenI& aGenI)
   : DisRecordHeader(aGenI)
   , mIORecordType(GetIORecordType(mRecordType))
{
}

DisIOBaseRecord::DisIOBaseRecord(DisIORecordTypeEnum aRecordType)
   : DisRecordHeader((DisEnum32)aRecordType)
   , mIORecordType(aRecordType)
{
}

void DisIOBaseRecord::Get(GenI& aGenI)
{
   DisRecordHeader::Get(aGenI);
}

DisUint16 DisIOBaseRecord::GetLength()
{
   return cRECORD_BASE_LENGTH_IN_OCTETS;
}

void DisIOBaseRecord::Put(GenO& aGenO) const
{
   DisRecordHeader::Put(aGenO);
}

bool DisIOBaseRecord::IsValid() const
{
   return DisRecordHeader::IsValid();
}

DisIORecordTypeEnum DisIOBaseRecord::GetIORecordType() const
{
   return mIORecordType;
}

DisIOBaseRecord* DisIOBaseRecord::Clone() const
{
   return new DisIOBaseRecord(*this);
}

const char* DisIOBaseRecord::GetScriptClassName() const
{
   return "IOBaseRecord";
}

void DisIOBaseRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisIOBaseRecord::ToString() const
{
   return DisRecordHeader::ToString();
}

DisIORecordTypeEnum DisIOBaseRecord::GetIORecordType(DisEnum32 aRecordValue)
{
   switch (aRecordValue)
   {
   case 0:
   {
      return DisIORecordTypeEnum::IO_NONE;
   }
   case 1:
   {
      return DisIORecordTypeEnum::IO_EFFECT;
   }
   case 2:
   {
      return DisIORecordTypeEnum::IO_COMM_NODE;
   }
   case 3:
   {
      return DisIORecordTypeEnum::IO_COMMUNICATION_NODE_ID;
   }
   default:
   {
      return DisIORecordTypeEnum::IO_NONE;
   }
   }
}
