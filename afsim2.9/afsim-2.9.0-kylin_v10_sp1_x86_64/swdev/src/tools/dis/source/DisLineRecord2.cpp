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

#include "DisLineRecord2.hpp"

DisLineRecord2::DisLineRecord2()
{
   for (int i = 0; i < 3; i++)
   {
      mStartPointVelocity[i] = 0;
      mEndPointVelocity[i]   = 0;
   }
}

DisLineRecord2::DisLineRecord2(const DisLineRecord2& aSrc)
   : mStartPointLocation(aSrc.mStartPointLocation)
   , mEndPointLocation(aSrc.mEndPointLocation)
{
   for (int i = 0; i < 3; i++)
   {
      mStartPointVelocity[i] = aSrc.mStartPointVelocity[i];
      mEndPointVelocity[i]   = aSrc.mEndPointVelocity[i];
   }
}

DisLineRecord2::DisLineRecord2(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisLineRecord2::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisLineRecord2::GetLength()
{
   return cLENGTH_IN_OCTETS;
}

void DisLineRecord2::Put(GenO& aGenO) const
{
   mStartPointLocation.Put(aGenO);
   mEndPointLocation.Put(aGenO);
   aGenO << mStartPointVelocity[0] << mStartPointVelocity[1] << mStartPointVelocity[2];
   aGenO << mEndPointVelocity[0] << mEndPointVelocity[1] << mEndPointVelocity[2];
}

bool DisLineRecord2::IsValid() const
{
   return mStartPointLocation.IsValid() && mEndPointLocation.IsValid();
}

DisLineRecord2* DisLineRecord2::Clone() const
{
   return new DisLineRecord2(*this);
}

DisPointRecord2& DisLineRecord2::GetStartPointLocation()
{
   return mStartPointLocation;
}

DisPointRecord2& DisLineRecord2::GetEndPointLocation()
{
   return mEndPointLocation;
}

void DisLineRecord2::GetStartPointVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mStartPointVelocity[0];
   aVelocityY = mStartPointVelocity[1];
   aVelocityZ = mStartPointVelocity[2];
}

void DisLineRecord2::GetEndPointVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mEndPointVelocity[0];
   aVelocityY = mEndPointVelocity[1];
   aVelocityZ = mEndPointVelocity[2];
}

void DisLineRecord2::SetStartPointLocation(const DisPointRecord2& aStartPointLocation)
{
   mStartPointLocation = aStartPointLocation;
}

void DisLineRecord2::SetEndPointLocation(const DisPointRecord2& aEndPointLocation)
{
   mEndPointLocation = aEndPointLocation;
}

void DisLineRecord2::SetStartPointVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mStartPointVelocity[0] = aVelocityX;
   mStartPointVelocity[1] = aVelocityY;
   mStartPointVelocity[2] = aVelocityZ;
}

void DisLineRecord2::SetEndPointVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mEndPointVelocity[0] = aVelocityX;
   mEndPointVelocity[1] = aVelocityY;
   mEndPointVelocity[2] = aVelocityZ;
}

std::string DisLineRecord2::ToString() const
{
   std::string returnString = "-------Line Record 2-------\n";
   returnString += mStartPointLocation.ToString();
   returnString += mEndPointLocation.ToString();
   returnString += "-----End Line Record 2-----\n";
   return returnString;
}

void DisLineRecord2::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisLineRecord2::GetMemberData(GenI& aGenI)
{
   mStartPointLocation.Get(aGenI);
   mEndPointLocation.Get(aGenI);
   aGenI >> mStartPointVelocity[0] >> mStartPointVelocity[1] >> mStartPointVelocity[2];
   aGenI >> mEndPointVelocity[0] >> mEndPointVelocity[1] >> mEndPointVelocity[2];
}
