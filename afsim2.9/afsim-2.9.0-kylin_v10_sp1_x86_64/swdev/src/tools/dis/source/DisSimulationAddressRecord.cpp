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

#include "DisSimulationAddressRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSimulationAddressRecord::DisSimulationAddressRecord(DisUint16 aSiteNumber, DisUint16 aApplicationNumber)
   : mSiteNumber(aSiteNumber)
   , mApplicationNumber(aApplicationNumber)
{
}

DisSimulationAddressRecord::DisSimulationAddressRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisSimulationAddressRecord::DisSimulationAddressRecord(const DisSimulationAddressRecord& aSrc)
   : mSiteNumber(aSrc.mSiteNumber)
   , mApplicationNumber(aSrc.mApplicationNumber)
{
}

void DisSimulationAddressRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisSimulationAddressRecord::GetLength()
{
   return cLOCAL_RECORD_SIZE_IN_OCTETS;
}

void DisSimulationAddressRecord::Put(GenO& aGenO) const
{
   aGenO << mSiteNumber;
   aGenO << mApplicationNumber;
}

bool DisSimulationAddressRecord::IsValid() const
{
   return true;
}

DisSimulationAddressRecord* DisSimulationAddressRecord::Clone() const
{
   return new DisSimulationAddressRecord(*this);
}

void DisSimulationAddressRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

std::string DisSimulationAddressRecord::ToString() const
{
   std::string returnString = "-------Simulation Address Record-------\n";
   returnString += "Site Number:        " + std::to_string(mSiteNumber) + "\n";
   returnString += "Application Number: " + std::to_string(mApplicationNumber) + "\n";
   returnString += "-----End Simulation Address Record-----\n";
   return returnString;
}

DisUint16 DisSimulationAddressRecord::GetSiteNumber() const
{
   return mSiteNumber;
}

DisUint16 DisSimulationAddressRecord::GetApplicationNumber() const
{
   return mApplicationNumber;
}

void DisSimulationAddressRecord::SetSiteNumber(DisUint16 aSiteNumber)
{
   mSiteNumber = aSiteNumber;
}

void DisSimulationAddressRecord::SetApplicationNumber(DisUint16 aApplicationNumber)
{
   mApplicationNumber = aApplicationNumber;
}

void DisSimulationAddressRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSiteNumber;
   aGenI >> mApplicationNumber;
}

// Operators
bool DisSimulationAddressRecord::operator==(const DisSimulationAddressRecord& aRhs) const
{
   return ((mSiteNumber == aRhs.mSiteNumber) && (mApplicationNumber == aRhs.mApplicationNumber));
}

bool DisSimulationAddressRecord::operator!=(const DisSimulationAddressRecord& aRhs) const
{
   return ((mSiteNumber != aRhs.mSiteNumber) || (mApplicationNumber != aRhs.mApplicationNumber));
}

bool DisSimulationAddressRecord::operator<(const DisSimulationAddressRecord& aRhs) const
{
   bool isLess = true;

   if (mSiteNumber > aRhs.mSiteNumber)
   {
      isLess = false;
   }
   else if (mSiteNumber == aRhs.mSiteNumber)
   {
      if (mApplicationNumber >= aRhs.mApplicationNumber)
      {
         isLess = false;
      }
   }
   return isLess;
}
