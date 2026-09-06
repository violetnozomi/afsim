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

#include "DisRectangularRecord2.hpp"

#include "DisValidationUtils.hpp"

DisRectangularRecord2::DisRectangularRecord2()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i]        = 0;
      mOrientation[i]     = 0;
      mAngularVelocity[i] = 0;
   }
}

DisRectangularRecord2::DisRectangularRecord2(const DisRectangularRecord2& aSrc)
   : DisPointRecord(aSrc)
   , mLengthX(aSrc.mLengthX)
   , mLengthY(aSrc.mLengthY)
   , mLengthZ(aSrc.mLengthZ)
   , mDeltaLengthX(aSrc.mDeltaLengthX)
   , mDeltaLengthY(aSrc.mDeltaLengthY)
   , mDeltaLengthZ(aSrc.mDeltaLengthZ)
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i]        = aSrc.mVelocity[i];
      mOrientation[i]     = aSrc.mOrientation[i];
      mAngularVelocity[i] = aSrc.mAngularVelocity[i];
   }
}

DisRectangularRecord2::DisRectangularRecord2(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisRectangularRecord2::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisRectangularRecord2::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisRectangularRecord2::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mLengthX;
   aGenO << mLengthY;
   aGenO << mLengthZ;
   aGenO << mDeltaLengthX;
   aGenO << mDeltaLengthY;
   aGenO << mDeltaLengthZ;
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];
   aGenO << mPadding;
}

bool DisRectangularRecord2::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mDeltaLengthX) && ValidateScalar(mDeltaLengthY) &&
          ValidateScalar(mDeltaLengthZ);
}

DisRectangularRecord2* DisRectangularRecord2::Clone() const
{
   return new DisRectangularRecord2(*this);
}

DisFloat32 DisRectangularRecord2::GetLengthX() const
{
   return mLengthX;
}

DisFloat32 DisRectangularRecord2::GetLengthY() const
{
   return mLengthY;
}

DisFloat32 DisRectangularRecord2::GetLengthZ() const
{
   return mLengthZ;
}

DisFloat32 DisRectangularRecord2::GetDeltaLengthX() const
{
   return mDeltaLengthX;
}

DisFloat32 DisRectangularRecord2::GetDeltaLengthY() const
{
   return mDeltaLengthY;
}

DisFloat32 DisRectangularRecord2::GetDeltaLengthZ() const
{
   return mDeltaLengthZ;
}

void DisRectangularRecord2::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisRectangularRecord2::GetOrientation(DisFloat32& aOrientationX, DisFloat32& aOrientationY, DisFloat32& aOrientationZ) const
{
   aOrientationX = mOrientation[0];
   aOrientationY = mOrientation[1];
   aOrientationZ = mOrientation[2];
}

void DisRectangularRecord2::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                               DisFloat32& aAngularVelocityY,
                                               DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

void DisRectangularRecord2::SetLengthX(DisFloat32 aLengthX)
{
   mLengthX = aLengthX;
}

void DisRectangularRecord2::SetLengthY(DisFloat32 aLengthY)
{
   mLengthY = aLengthY;
}

void DisRectangularRecord2::SetLengthZ(DisFloat32 aLengthZ)
{
   mLengthZ = aLengthZ;
}

void DisRectangularRecord2::SetDeltaLengthX(DisFloat32 aDeltaLengthX)
{
   mDeltaLengthX = aDeltaLengthX;
}

void DisRectangularRecord2::SetDeltaLengthY(DisFloat32 aDeltaLengthY)
{
   mDeltaLengthY = aDeltaLengthY;
}

void DisRectangularRecord2::SetDeltaLengthZ(DisFloat32 aDeltaLengthZ)
{
   mDeltaLengthZ = aDeltaLengthZ;
}

void DisRectangularRecord2::SetOrientation(DisFloat32 aOrientationX, DisFloat32 aOrientationY, DisFloat32 aOrientationZ)
{
   mOrientation[0] = aOrientationX;
   mOrientation[1] = aOrientationY;
   mOrientation[2] = aOrientationZ;
}

void DisRectangularRecord2::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisRectangularRecord2::SetAngularVelocity(DisFloat32 aAngularVelocityX,
                                               DisFloat32 aAngularVelocityY,
                                               DisFloat32 aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

std::string DisRectangularRecord2::ToString() const
{
   std::string returnString = "-------Rectangular Record 2-------\n";
   returnString += DisPointRecord::ToString();
   returnString += "Delta Length X: " + std::to_string(mDeltaLengthX) + "\n";
   returnString += "Delta Length Y: " + std::to_string(mDeltaLengthY) + "\n";
   returnString += "Delta Length Z: " + std::to_string(mDeltaLengthZ) + "\n";
   returnString += "-------Velocity-------\n";
   returnString += "   Velocity X: " + std::to_string(mVelocity[0]) + "\n";
   returnString += "   Velocity Y: " + std::to_string(mVelocity[1]) + "\n";
   returnString += "   Velocity Z: " + std::to_string(mVelocity[2]) + "\n";
   returnString += "-----End Velocity-----\n";
   returnString += "-------Angular Velocity-------\n";
   returnString += "   Angular Velocity X: " + std::to_string(mAngularVelocity[0]) + "\n";
   returnString += "   Angular Velocity Y: " + std::to_string(mAngularVelocity[1]) + "\n";
   returnString += "   Angular Velocity Z: " + std::to_string(mAngularVelocity[2]) + "\n";
   returnString += "-----End Angular Velocity-----\n";
   returnString += "-----End Rectangular Record 2-----\n";
   return returnString;
}

void DisRectangularRecord2::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisRectangularRecord2::GetMemberData(GenI& aGenI)
{
   aGenI >> mLengthX;
   aGenI >> mLengthY;
   aGenI >> mLengthZ;
   aGenI >> mDeltaLengthX;
   aGenI >> mDeltaLengthY;
   aGenI >> mDeltaLengthZ;
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];
   aGenI >> mPadding;
}
