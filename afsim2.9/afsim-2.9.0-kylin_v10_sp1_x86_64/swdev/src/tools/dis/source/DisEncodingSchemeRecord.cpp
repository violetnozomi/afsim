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

#include "DisEncodingSchemeRecord.hpp"

#include <iostream>

#include "GenI.hpp"
#include "GenO.hpp"
namespace
{

const DisUint32 cENCODING_TYPE_TDL_MESSAGES_MASK = 0x00003fff; // bit 0 - 13
const DisUint32 cENCODING_CLASS_MASK             = 0x0000C000; // bit 14&15
} // namespace

DisEncodingSchemeRecord::DisEncodingSchemeRecord(DisUint32 aValue)
   : mEncodingSchemeRecord(aValue){};

std::string DisEncodingSchemeRecord::ToString() const
{
   std::string returnString = "Encoding Scheme Record:\n";
   if (DisEnum::EncodingSchemeRecord::EncodingType::IsValid(
          (DisEnum::EncodingSchemeRecord::EncodingType::Enum)GetEncodingTypeOrNumberOfTDLMessages()))
   {
      returnString += "   Encoding Type (If Applicable):         " +
                      DisEnum::EncodingSchemeRecord::EncodingType::ToString(
                         (DisEnum::EncodingSchemeRecord::EncodingType::Enum)GetEncodingTypeOrNumberOfTDLMessages()) +
                      '\n';
      returnString +=
         "   Number Of TDLMessages (If Applicable): " + std::to_string(GetEncodingTypeOrNumberOfTDLMessages()) + '\n';
   }
   else
   {
      returnString +=
         "   Number Of TDLMessages              : " + std::to_string(GetEncodingTypeOrNumberOfTDLMessages()) + '\n';
   }

   returnString += "   Encoding Class:       " +
                   DisEnum::EncodingSchemeRecord::EncodingClass::ToString(
                      (DisEnum::EncodingSchemeRecord::EncodingClass::Enum)GetEncodingClass()) +
                   '\n';
   return returnString;
}

void DisEncodingSchemeRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}

void DisEncodingSchemeRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mEncodingSchemeRecord;
}

void DisEncodingSchemeRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisEncodingSchemeRecord::Put(GenO& aGenO) const
{
   aGenO << mEncodingSchemeRecord;
}

bool DisEncodingSchemeRecord::IsValid() const
{
   return true;
}

bool DisEncodingSchemeRecord::operator==(const DisEncodingSchemeRecord& aRhs) const
{
   return mEncodingSchemeRecord == aRhs.mEncodingSchemeRecord;
}

// The right-most integer in the expressions below are the number of bit shifts.
unsigned int DisEncodingSchemeRecord::GetEncodingTypeOrNumberOfTDLMessages() const
{
   return ((mEncodingSchemeRecord & cENCODING_TYPE_TDL_MESSAGES_MASK) >> 0);
}

unsigned int DisEncodingSchemeRecord::GetEncodingClass() const
{
   return ((mEncodingSchemeRecord & cENCODING_CLASS_MASK) >> 14);
}

void DisEncodingSchemeRecord::SetEncodingTypeOrNumberOfTDLMessages(unsigned int aValue)
{
   mEncodingSchemeRecord =
      ((aValue << 0) & cENCODING_TYPE_TDL_MESSAGES_MASK) | (mEncodingSchemeRecord & ~cENCODING_TYPE_TDL_MESSAGES_MASK);
}

void DisEncodingSchemeRecord::SetEncodingClass(unsigned int aValue)
{
   mEncodingSchemeRecord = ((aValue << 14) & cENCODING_CLASS_MASK) | (mEncodingSchemeRecord & ~cENCODING_CLASS_MASK);
};
