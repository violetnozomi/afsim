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

#include "DisNamedLocationIdentificationRecord.hpp"

#include "DisValidationUtils.hpp"

DisNamedLocationIdentificationRecord::DisNamedLocationIdentificationRecord(DisEnum16 aStationName, DisUint16 aStationNumber)
   : mStationName(aStationName)
   , mStationNumber(aStationNumber)
{
}

DisNamedLocationIdentificationRecord::DisNamedLocationIdentificationRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisNamedLocationIdentificationRecord::DisNamedLocationIdentificationRecord(const DisNamedLocationIdentificationRecord& aSrc)
   : mStationName(aSrc.mStationName)
   , mStationNumber(aSrc.mStationNumber)
{
}

void DisNamedLocationIdentificationRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisNamedLocationIdentificationRecord::GetLength()
{
   return cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisNamedLocationIdentificationRecord::Put(GenO& aGenO) const
{
   aGenO << mStationName;
   aGenO << mStationNumber;
}

bool DisNamedLocationIdentificationRecord::IsValid() const
{
   return true;
}

DisNamedLocationIdentificationRecord* DisNamedLocationIdentificationRecord::Clone() const
{
   return new DisNamedLocationIdentificationRecord(*this);
}

void DisNamedLocationIdentificationRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}

std::string DisNamedLocationIdentificationRecord::ToString() const
{
   std::string returnString = "-------Named Location Identification Record-------\n";
   returnString += "Station name:   " + std::to_string(mStationName) + "\n";
   returnString += "Station number: " + std::to_string(mStationNumber) + "\n";
   returnString += "-----End Named Location Identification Record-----\n";
   return returnString;
}

DisEnum16 DisNamedLocationIdentificationRecord::GetStationName() const
{
   return mStationName;
}

DisUint16 DisNamedLocationIdentificationRecord::GetStationNumber() const
{
   return mStationNumber;
}

void DisNamedLocationIdentificationRecord::SetStationName(DisEnum16 aStationName)
{
   mStationName = aStationName;
}

void DisNamedLocationIdentificationRecord::SetStationNumber(DisUint16 aStationNumber)
{
   mStationNumber = aStationNumber;
}

void DisNamedLocationIdentificationRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mStationName;
   aGenI >> mStationNumber;
}

// Operators
bool DisNamedLocationIdentificationRecord::operator==(const DisNamedLocationIdentificationRecord& aRhs) const
{
   return ((mStationName == aRhs.mStationName) && (mStationNumber == aRhs.mStationNumber));
}

bool DisNamedLocationIdentificationRecord::operator!=(const DisNamedLocationIdentificationRecord& aRhs) const
{
   return ((mStationName != aRhs.mStationName) || (mStationNumber != aRhs.mStationNumber));
}

bool DisNamedLocationIdentificationRecord::operator<(const DisNamedLocationIdentificationRecord& aRhs) const
{
   bool isLess = true;

   if (mStationName > aRhs.mStationName)
   {
      isLess = false;
   }
   else if (mStationName == aRhs.mStationName)
   {
      if (mStationNumber >= aRhs.mStationNumber)
      {
         isLess = false;
      }
   }
   return isLess;
}
