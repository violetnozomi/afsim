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

#include "DisICTypeRecord.hpp"

#include "DisRecordHeader.hpp"
#include "GenI.hpp"
#include "GenO.hpp"


DisICTypeRecord::DisICTypeRecord(DisEnum16 aRecordType, DisUint16 aRecordLength)
   : mRecordType(aRecordType)
   , mRecordLength(aRecordLength)
{
}

DisICTypeRecord::DisICTypeRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisICTypeRecord::DisICTypeRecord(const DisICTypeRecord& aSrc)
   : mRecordType(aSrc.mRecordType)
   , mRecordLength(aSrc.mRecordLength)
{
}

DisICTypeRecord* DisICTypeRecord::Clone() const
{
   return new DisICTypeRecord(*this);
}

DisUint16 DisICTypeRecord::GetLength()
{
   return cBASE_LENGTH_OCTETS;
}

void DisICTypeRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisICTypeRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mRecordType;
   aGenI >> mRecordLength;
}

void DisICTypeRecord::Put(GenO& aGenO) const
{
   aGenO << mRecordType;
   aGenO << mRecordLength;
}

DisEnum16 DisICTypeRecord::GetRecordType() const
{
   return mRecordType;
}

DisUint16 DisICTypeRecord::GetRecordLength() const
{
   return mRecordLength;
}

void DisICTypeRecord::SetRecordType(DisEnum16 aRecordType)
{
   mRecordType = aRecordType;
}

void DisICTypeRecord::SetRecordLength(DisUint16 aRecordLength)
{
   mRecordLength = aRecordLength;
}

void DisICTypeRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisICTypeRecord::ToString() const
{
   std::string returnString = "--------Dis IC Type Record-------";
   returnString += "Record Type:   " + std::to_string(mRecordType);
   returnString += "Record Length: " + std::to_string(mRecordLength);
   returnString += "------End Dis IC Type Record-----";
   return returnString;
}

bool DisICTypeRecord::IsValid() const
{
   return true;
}
