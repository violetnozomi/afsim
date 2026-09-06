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

#include "DisPointRecord2.hpp"

#include "DisValidationUtils.hpp"

DisPointRecord2::DisPointRecord2()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i] = 0;
   }
}

DisPointRecord2::DisPointRecord2(const DisPointRecord2& aSrc)
   : DisPointRecord(aSrc)
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i] = aSrc.mVelocity[i];
   }
}

DisPointRecord2::DisPointRecord2(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisPointRecord2::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisPointRecord2::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisPointRecord2::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mPadding;
}

bool DisPointRecord2::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalarArray(mVelocity, 3);
}

DisPointRecord2* DisPointRecord2::Clone() const
{
   return new DisPointRecord2(*this);
}

void DisPointRecord2::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisPointRecord2::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

std::string DisPointRecord2::ToString() const
{
   std::string returnString = "-------Point Record 2-------\n";
   returnString += DisPointRecord::ToString() + "\n";
   returnString += "X: " + std::to_string(mVelocity[0]) + "\n";
   returnString += "Y: " + std::to_string(mVelocity[1]) + "\n";
   returnString += "Z: " + std::to_string(mVelocity[2]) + "\n";
   returnString += "-----End Point Record 2-----\n";
   return returnString;
}

void DisPointRecord2::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

void DisPointRecord2::GetMemberData(GenI& aGenI)
{
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mPadding;
}
