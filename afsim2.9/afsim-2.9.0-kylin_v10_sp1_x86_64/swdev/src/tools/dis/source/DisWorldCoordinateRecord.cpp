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

#include "DisWorldCoordinateRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisWorldCoordinateRecord::DisWorldCoordinateRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mLocation[i] = 0;
   }
}

DisWorldCoordinateRecord::DisWorldCoordinateRecord(const DisWorldCoordinateRecord& aSrc)
{
   for (int i = 0; i < 3; i++)
   {
      mLocation[i] = aSrc.mLocation[i];
   }
}

DisWorldCoordinateRecord::DisWorldCoordinateRecord(GenI& aGenI)
{
   for (int i = 0; i < 3; i++)
   {
      mLocation[i] = 0;
   }
   GetMemberData(aGenI);
}

void DisWorldCoordinateRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisWorldCoordinateRecord::GetLength()
{
   return cBASE_LENGTH_IN_OCTETS;
}

void DisWorldCoordinateRecord::Put(GenO& aGenO) const
{
   aGenO << mLocation[0] << mLocation[1] << mLocation[2];
}

bool DisWorldCoordinateRecord::IsValid() const
{
   return ValidateScalarArray(mLocation, 3);
}

DisWorldCoordinateRecord* DisWorldCoordinateRecord::Clone() const
{
   return new DisWorldCoordinateRecord(*this);
}

void DisWorldCoordinateRecord::GetLocation(DisFloat64& aLocationX, DisFloat64& aLocationY, DisFloat64& aLocationZ) const
{
   aLocationX = mLocation[0];
   aLocationY = mLocation[1];
   aLocationZ = mLocation[2];
}

void DisWorldCoordinateRecord::SetLocation(DisFloat64 aLocationX, DisFloat64 aLocationY, DisFloat64 aLocationZ)
{
   mLocation[0] = aLocationX;
   mLocation[1] = aLocationY;
   mLocation[2] = aLocationZ;
}

void DisWorldCoordinateRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << std::endl;
}

std::string DisWorldCoordinateRecord::ToString() const
{
   std::string returnString = "-------World Coordinate Record-------\n";
   returnString += "X: " + std::to_string(mLocation[0]) + "\n";
   returnString += "Y: " + std::to_string(mLocation[1]) + "\n";
   returnString += "Z: " + std::to_string(mLocation[2]) + "\n";
   returnString += "-----End World Coordinate Record-----\n";
   return returnString;
}

void DisWorldCoordinateRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mLocation[0] >> mLocation[1] >> mLocation[2];
}
