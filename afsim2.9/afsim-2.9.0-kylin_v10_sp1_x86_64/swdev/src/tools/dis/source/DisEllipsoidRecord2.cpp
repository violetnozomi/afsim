//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "DisEllipsoidRecord2.hpp"

#include "DisValidationUtils.hpp"

DisEllipsoidRecord2::DisEllipsoidRecord2()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mVelocity[i]        = 0;
      mOrientation[i]     = 0;
      mAngularVelocity[i] = 0;
   }
}

DisEllipsoidRecord2::DisEllipsoidRecord2(const DisEllipsoidRecord2& aSrc)
   : DisPointRecord(aSrc)
   , mSigmaX(aSrc.mSigmaX)
   , mSigmaY(aSrc.mSigmaY)
   , mSigmaZ(aSrc.mSigmaZ)
   , mDeltaSigmaX(aSrc.mDeltaSigmaX)
   , mDeltaSigmaY(aSrc.mDeltaSigmaY)
   , mDeltaSigmaZ(aSrc.mDeltaSigmaZ)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i]     = aSrc.mOrientation[i];
      mVelocity[i]        = aSrc.mVelocity[i];
      mAngularVelocity[i] = aSrc.mAngularVelocity[i];
   }
}

DisEllipsoidRecord2::DisEllipsoidRecord2(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisEllipsoidRecord2::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEllipsoidRecord2::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisEllipsoidRecord2::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mSigmaX;
   aGenO << mSigmaY;
   aGenO << mSigmaZ;
   aGenO << mDeltaSigmaX;
   aGenO << mDeltaSigmaY;
   aGenO << mDeltaSigmaZ;
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];
   aGenO << mPadding;
}

bool DisEllipsoidRecord2::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mSigmaX) && ValidateScalar(mSigmaY) && ValidateScalar(mSigmaZ) &&
          ValidateScalarArray(mOrientation, 3) && ValidateScalar(mDeltaSigmaX) && ValidateScalar(mDeltaSigmaY) &&
          ValidateScalar(mDeltaSigmaZ) && ValidateScalarArray(mVelocity, 3) && ValidateScalarArray(mAngularVelocity, 3);
}

DisEllipsoidRecord2* DisEllipsoidRecord2::Clone() const
{
   return new DisEllipsoidRecord2(*this);
}

DisFloat32 DisEllipsoidRecord2::GetSigmaX() const
{
   return mSigmaX;
}

DisFloat32 DisEllipsoidRecord2::GetSigmaY() const
{
   return mSigmaY;
}

DisFloat32 DisEllipsoidRecord2::GetSigmaZ() const
{
   return mSigmaZ;
}

void DisEllipsoidRecord2::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

DisFloat32 DisEllipsoidRecord2::GetDeltaSigmaX() const
{
   return mDeltaSigmaX;
}

DisFloat32 DisEllipsoidRecord2::GetDeltaSigmaY() const
{
   return mDeltaSigmaY;
}

DisFloat32 DisEllipsoidRecord2::GetDeltaSigmaZ() const
{
   return mDeltaSigmaZ;
}

void DisEllipsoidRecord2::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisEllipsoidRecord2::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                             DisFloat32& aAngularVelocityY,
                                             DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

void DisEllipsoidRecord2::SetSigmaX(DisFloat32 aSigmaX)
{
   mSigmaX = aSigmaX;
}

void DisEllipsoidRecord2::SetSigmaY(DisFloat32 aSigmaY)
{
   mSigmaY = aSigmaY;
}

void DisEllipsoidRecord2::SetSigmaZ(DisFloat32 aSigmaZ)
{
   mSigmaZ = aSigmaZ;
}

void DisEllipsoidRecord2::SetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisEllipsoidRecord2::SetDeltaSigmaX(DisFloat32 aDeltaSigmaX)
{
   mDeltaSigmaX = aDeltaSigmaX;
}

void DisEllipsoidRecord2::SetDeltaSigmaY(DisFloat32 aDeltaSigmaY)
{
   mDeltaSigmaY = aDeltaSigmaY;
}

void DisEllipsoidRecord2::SetDeltaSigmaZ(DisFloat32 aDeltaSigmaZ)
{
   mDeltaSigmaZ = aDeltaSigmaZ;
}

void DisEllipsoidRecord2::SetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisEllipsoidRecord2::SetAngularVelocity(DisFloat32& aAngularVelocityX,
                                             DisFloat32& aAngularVelocityY,
                                             DisFloat32& aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

void DisEllipsoidRecord2::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisEllipsoidRecord2::ToString() const
{
   std::string returnString = "-------Ellipsoid Record-------\n";
   returnString += "Delta Sigma X: " + std::to_string(mDeltaSigmaX) + '\n';
   returnString += "Delta Sigma Y: " + std::to_string(mDeltaSigmaY) + '\n';
   returnString += "Delta Sigma Z: " + std::to_string(mDeltaSigmaZ) + '\n';
   returnString += "-------Velocity-------\n";
   returnString += "Velocity X: " + std::to_string(mVelocity[0]) + '\n';
   returnString += "Velocity Y: " + std::to_string(mVelocity[1]) + '\n';
   returnString += "Velocity Z: " + std::to_string(mVelocity[2]) + '\n';
   returnString += "-----End Velocity-----\n";
   returnString += "-------Angular Velocity-------\n";
   returnString += "Angular Velocity X: " + std::to_string(mAngularVelocity[0]) + '\n';
   returnString += "Angular Velocity Y: " + std::to_string(mAngularVelocity[1]) + '\n';
   returnString += "Angular Velocity Z: " + std::to_string(mAngularVelocity[2]) + '\n';
   returnString += "-----End Angular Velocity-----\n";
   returnString += "-----End Ellipsoid Record-----\n";
   return returnString;
}

void DisEllipsoidRecord2::GetMemberData(GenI& aGenI)
{
   aGenI >> mSigmaX;
   aGenI >> mSigmaY;
   aGenI >> mSigmaZ;
   aGenI >> mDeltaSigmaX;
   aGenI >> mDeltaSigmaY;
   aGenI >> mDeltaSigmaZ;
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];
   aGenI >> mPadding;
}
