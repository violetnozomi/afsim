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

#include "DisSphereRecord2.hpp"

#include "DisValidationUtils.hpp"

DisSphereRecord2::DisSphereRecord2()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i]        = 0;
      mAngularVelocity[i] = 0;
   }
}

DisSphereRecord2::DisSphereRecord2(const DisSphereRecord2& aSrc)
   : DisPointRecord(aSrc)
   , mRadius(aSrc.mRadius)
   , mDeltaRadius(aSrc.mDeltaRadius)
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i]        = aSrc.mVelocity[i];
      mAngularVelocity[i] = aSrc.mAngularVelocity[i];
   }
}

DisSphereRecord2::DisSphereRecord2(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisSphereRecord2::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisSphereRecord2::GetLength()
{
   return DisPointRecord::GetLength() + cBASE_LENGTH_OCTETS;
}

void DisSphereRecord2::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mRadius;
   aGenO << mDeltaRadius;
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];
}

bool DisSphereRecord2::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mDeltaRadius) && ValidateScalar(mVelocity[0]) &&
          ValidateScalar(mVelocity[1]) && ValidateScalar(mVelocity[2]) && ValidateScalar(mAngularVelocity[0]) &&
          ValidateScalar(mAngularVelocity[1]) && ValidateScalar(mAngularVelocity[2]);
}

DisSphereRecord2* DisSphereRecord2::Clone() const
{
   return new DisSphereRecord2(*this);
}

DisFloat32 DisSphereRecord2::GetRadius() const
{
   return mRadius;
}

DisFloat32 DisSphereRecord2::GetDeltaRadius() const
{
   return mDeltaRadius;
}

void DisSphereRecord2::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisSphereRecord2::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                          DisFloat32& aAngularVelocityY,
                                          DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

void DisSphereRecord2::SetRadius(DisFloat32 aRadius)
{
   mRadius = aRadius;
}

void DisSphereRecord2::SetDeltaRadius(DisFloat32 aDeltaRadius)
{
   mDeltaRadius = aDeltaRadius;
}

void DisSphereRecord2::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisSphereRecord2::SetAngularVelocity(DisFloat32 aAngularVelocityX,
                                          DisFloat32 aAngularVelocityY,
                                          DisFloat32 aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

void DisSphereRecord2::Stream(std::ostream& aStream)
{
   aStream << ToString() << std::endl;
}

std::string DisSphereRecord2::ToString()
{
   std::string returnString = DisPointRecord::ToString() + "\n";
   returnString += "Delta Radius: " + std::to_string(mDeltaRadius) + "\n";
   returnString += "-------Velocity-------\n";
   returnString += "Velocity X: " + std::to_string(mVelocity[0]) + "\n";
   returnString += "Velocity Y: " + std::to_string(mVelocity[1]) + "\n";
   returnString += "Velocity Z: " + std::to_string(mVelocity[2]) + "\n";
   returnString += "-----End Velocity-----\n";
   returnString += "-------Angular Velocity-------\n";
   returnString += "Angular Velocity X: " + std::to_string(mAngularVelocity[0]) + "\n";
   returnString += "Angular Velocity Y: " + std::to_string(mAngularVelocity[1]) + "\n";
   returnString += "Angular Velocity Z: " + std::to_string(mAngularVelocity[2]) + "\n";
   returnString += "-----End Angular Velocity-----\n";
   return returnString;
}

void DisSphereRecord2::GetMemberData(GenI& aGenI)
{
   aGenI >> mRadius;
   aGenI >> mDeltaRadius;
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];
}
