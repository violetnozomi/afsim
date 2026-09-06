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

#include "DisConeRecord2.hpp"

#include "DisValidationUtils.hpp"

DisConeRecord2::DisConeRecord2()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i]     = 0;
      mVelocity[i]        = 0;
      mAngularVelocity[i] = 0;
   }
}

DisConeRecord2::DisConeRecord2(const DisConeRecord2& aSrc)
   : DisPointRecord(aSrc)
   , mHeight(aSrc.mHeight)
   , mPeakAngle(aSrc.mPeakAngle)
   , mDeltaHeight(aSrc.mDeltaHeight)
   , mDeltaPeakAngle(aSrc.mDeltaPeakAngle)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i]     = aSrc.mOrientation[i];
      mVelocity[i]        = aSrc.mVelocity[i];
      mAngularVelocity[i] = aSrc.mAngularVelocity[i];
   }
}

DisConeRecord2::DisConeRecord2(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisConeRecord2::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisConeRecord2::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisConeRecord2::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];
   aGenO << mHeight;
   aGenO << mDeltaHeight;
   aGenO << mPeakAngle;
   aGenO << mDeltaPeakAngle;
   aGenO << mPadding;
}

bool DisConeRecord2::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalarArray(mOrientation, 3) && ValidateScalar(mHeight) &&
          ValidateScalar(mPeakAngle) && ValidateScalarArray(mVelocity, 3) && ValidateScalarArray(mAngularVelocity, 3) &&
          ValidateScalar(mDeltaHeight) && ValidateScalar(mDeltaPeakAngle);
}

DisConeRecord2* DisConeRecord2::Clone() const
{
   return new DisConeRecord2(*this);
}

void DisConeRecord2::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

DisFloat32 DisConeRecord2::GetHeight() const
{
   return mHeight;
}

DisFloat32 DisConeRecord2::GetPeakAngle() const
{
   return mPeakAngle;
}

void DisConeRecord2::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisConeRecord2::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                        DisFloat32& aAngularVelocityY,
                                        DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

DisFloat32 DisConeRecord2::GetDeltaHeight() const
{
   return mDeltaHeight;
}

DisFloat32 DisConeRecord2::GetDeltaPeakAngle() const
{
   return mDeltaPeakAngle;
}

void DisConeRecord2::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisConeRecord2::SetAngularVelocity(DisFloat32 aAngularVelocityX, DisFloat32 aAngularVelocityY, DisFloat32 aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

void DisConeRecord2::SetDeltaHeight(DisFloat32 aDeltaHeight)
{
   mDeltaHeight = aDeltaHeight;
}

void DisConeRecord2::SetDeltaPeakAngle(DisFloat32 aDeltaPeakAngle)
{
   mDeltaPeakAngle = aDeltaPeakAngle;
}

void DisConeRecord2::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisConeRecord2::SetHeight(DisFloat32 aHeight)
{
   mHeight = aHeight;
}

void DisConeRecord2::SetPeakAngle(DisFloat32 aPeakAngle)
{
   mPeakAngle = aPeakAngle;
}

std::string DisConeRecord2::ToString() const
{
   std::string returnString = "";
   returnString += "-------Cone Record 2-------\n";
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
   returnString += "Delta Height: " + std::to_string(mDeltaHeight) + '\n';
   returnString += "Delta Peak Angle: " + std::to_string(mDeltaPeakAngle) + '\n';
   returnString += "-----End Cone Record 2-----\n";
   return returnString;
}

void DisConeRecord2::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisConeRecord2::GetMemberData(GenI& aGenI)
{
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];
   aGenI >> mHeight;
   aGenI >> mDeltaHeight;
   aGenI >> mPeakAngle;
   aGenI >> mDeltaPeakAngle;
   aGenI >> mPadding;
}
