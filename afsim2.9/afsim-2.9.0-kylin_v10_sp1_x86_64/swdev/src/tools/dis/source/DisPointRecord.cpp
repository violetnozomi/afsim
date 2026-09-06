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

#include "DisPointRecord.hpp"

#include "DisValidationUtils.hpp"

DisPointRecord::DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mLocation[i] = 0;
   }
}

DisPointRecord::DisPointRecord(const DisPointRecord& aSrc)
{
   for (int i = 0; i < 3; i++)
   {
      mLocation[i] = aSrc.mLocation[i];
   }
}

DisPointRecord::DisPointRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisPointRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisPointRecord::GetLength()
{
   return cMIN_LENGTH_OCTETS;
}

void DisPointRecord::Put(GenO& aGenO) const
{
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
}

bool DisPointRecord::IsValid() const
{
   return ValidateScalarArray(mLocation, 3);
}

DisPointRecord* DisPointRecord::Clone() const
{
   return new DisPointRecord(*this);
}

void DisPointRecord::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}

void DisPointRecord::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

std::string DisPointRecord::ToString() const
{
   std::string returnString = "-------Point Record-------\n";
   returnString += "X: " + std::to_string(mLocation[0]) + "\n";
   returnString += "Y: " + std::to_string(mLocation[1]) + "\n";
   returnString += "Z: " + std::to_string(mLocation[2]) + "\n";
   returnString += "-----End Point Record-----\n";
   return returnString;
}

void DisPointRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

void DisPointRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
}
