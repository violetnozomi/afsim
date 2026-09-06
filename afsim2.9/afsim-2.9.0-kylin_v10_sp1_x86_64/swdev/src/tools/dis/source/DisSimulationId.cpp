// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisSimulationId.hpp"

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisSimulationId::DisSimulationId()
   : DisSimulationAddressRecord()
{
}

DisSimulationId::DisSimulationId(DisUint16 aSiteNumber, DisUint16 aApplicationNumber, DisUint16 aReferenceNumber)
   : DisSimulationAddressRecord(aSiteNumber, aApplicationNumber)
   , mReferenceNumber(aReferenceNumber)
{
}

DisSimulationId::DisSimulationId(DisUint16 aSiteNumber, DisUint16 aApplicationNumber)
   : DisSimulationAddressRecord(aSiteNumber, aApplicationNumber)
{
}

DisSimulationId::DisSimulationId(const DisSimulationId& aSrc)
   : DisSimulationAddressRecord(aSrc)
   , mReferenceNumber(aSrc.mReferenceNumber)
{
}

DisSimulationId::DisSimulationId(const DisSimulationAddressRecord& aSimulationAddressRecord)
   : DisSimulationAddressRecord(aSimulationAddressRecord)
{
}

DisSimulationId* DisSimulationId::Clone() const
{
   return new DisSimulationId(*this);
}

DisEntityId* DisSimulationId::ToEntityId() const
{
   return new DisEntityId(GetSiteNumber(), GetApplicationNumber(), mReferenceNumber);
}

void DisSimulationId::Get(GenI& aGenI)
{
   DisSimulationAddressRecord::Get(aGenI);
   aGenI >> mReferenceNumber;
}

DisUint16 DisSimulationId::GetLength()
{
   return DisSimulationAddressRecord::GetLength() + cSIZE_IN_OCTETS;
}

void DisSimulationId::Put(GenO& aGenO) const
{
   DisSimulationAddressRecord::Put(aGenO);
   aGenO << mReferenceNumber;
}

DisSimulationId& DisSimulationId::operator=(const DisSimulationId& aRhs)
{
   if (this != &aRhs)
   {
      DisSimulationAddressRecord::SetApplicationNumber(aRhs.GetApplicationNumber());
      DisSimulationAddressRecord::SetSiteNumber(aRhs.GetSiteNumber());
      mReferenceNumber = aRhs.mReferenceNumber;
   }
   return *this;
}

bool DisSimulationId::operator<(const DisSimulationId& aRhs) const
{
   return DisSimulationAddressRecord::operator<(aRhs);
}

std::string DisSimulationId::ToString() const
{
   std::string returnString = "Simulation ID:\n";
   returnString += "Simulation Address Record:\n";
   returnString += DisSimulationAddressRecord::ToString();
   returnString += "End Simulation Address Record:\n";
   returnString += "Reference Number: " + std::to_string(mReferenceNumber) + "\n";
   returnString += "End Simulation ID:\n";
   return returnString;
}

void DisSimulationId::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

bool DisSimulationId::IsValid() const
{
   return DisSimulationAddressRecord::IsValid();
}
