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

#include "DisRecordHeader.hpp"

#include <iostream>

#include "Dis.hpp"
#include "DisFalseTargetsAttributeRecord.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisRecordHeader::DisRecordHeader(DisEnum32 aRecordType)
   : mRecordType(aRecordType)
{
}

DisRecordHeader::DisRecordHeader(GenI& aGenI)
{
   Get(aGenI);
}

DisUint16 DisRecordHeader::GetLength()
{
   // Record Length is the total length of the Record
   return mRecordLength;
}

void DisRecordHeader::Get(GenI& aGenI)
{
   aGenI >> mRecordType;
   aGenI >> mRecordLength;
}

void DisRecordHeader::Put(GenO& aGenO) const
{
   aGenO << mRecordType;
   aGenO << mRecordLength;
}

void DisRecordHeader::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

bool DisRecordHeader::IsValid() const
{
   return true;
}

std::string DisRecordHeader::ToString() const
{
   std::string returnString;

   returnString += "Record Type:   " + std::to_string(mRecordType) + "\n";

   // To preserve the existing output format for False Targets records, the length field is only provided for other record types
   const DisFalseTargetsAttributeRecord* dftar = dynamic_cast<const DisFalseTargetsAttributeRecord*>(this);
   if (dftar)
   {
      returnString = dftar->ToString();
   }
   else
   {
      returnString += "Length:        " + std::to_string(mRecordLength) + "\n";
   }
   return returnString;
}

void DisRecordHeader::HandleReadError(ut::log::MessageStream& aOut)
{
   if (Dis::ShowError(Dis::cIO_ERRORS))
   {
      aOut.AddNote() << "Type: " << static_cast<unsigned int>(mRecordType);
      aOut.AddNote() << "Length: " << mRecordLength;
   }
}

// virtual
void DisRecordHeader::GetDerivedClass(GenI& /*aGenI*/)
{
   // Do nothing base class implementation
}

// virtual
void DisRecordHeader::PutDerivedClass(GenO& /*aGenO*/) const
{
   // Do nothing base class implementation
}

// virtual
DisUint16 DisRecordHeader::GetDerivedClassLength()
{
   // Do nothing base class implementation
   return 0;
}

bool DisRecordHeader::operator==(const DisRecordHeader& aOther) const
{
   return mRecordLength == aOther.mRecordLength && mRecordType == aOther.mRecordType;
}
