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

#include "DisGaussianPuffRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGaussianPuffRecord::DisGaussianPuffRecord()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOriginatingLocation[i] = 0;
      mOrientation[i]         = 0;
      mVelocity[i]            = 0;
      mAngularVelocity[i]     = 0;
   }
}

DisGaussianPuffRecord::DisGaussianPuffRecord(const DisGaussianPuffRecord& aSrc)
   : DisPointRecord(aSrc)
   , mSigmaX(aSrc.mSigmaX)
   , mSigmaY(aSrc.mSigmaY)
   , mSigmaZ(aSrc.mSigmaZ)
   , mDeltaSigmaX(aSrc.mDeltaSigmaX)
   , mDeltaSigmaY(aSrc.mDeltaSigmaY)
   , mDeltaSigmaZ(aSrc.mDeltaSigmaZ)
   , mCentroidHeight(aSrc.mCentroidHeight)
{
   for (int i = 0; i < 3; i++)
   {
      mOriginatingLocation[i] = aSrc.mOriginatingLocation[i];
      mOrientation[i]         = aSrc.mOrientation[i];
      mVelocity[i]            = aSrc.mVelocity[i];
      mAngularVelocity[i]     = aSrc.mAngularVelocity[i];
   }
}

DisGaussianPuffRecord::DisGaussianPuffRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisGaussianPuffRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGaussianPuffRecord::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisGaussianPuffRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mOriginatingLocation[0] << mOriginatingLocation[1] << mOriginatingLocation[2];
   aGenO << mSigmaX;
   aGenO << mSigmaY;
   aGenO << mSigmaZ;
   aGenO << mDeltaSigmaX;
   aGenO << mDeltaSigmaY;
   aGenO << mDeltaSigmaZ;
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mVelocity[0] << mVelocity[1] << mVelocity[2];
   aGenO << mAngularVelocity[0] << mAngularVelocity[1] << mAngularVelocity[2];
   aGenO << mCentroidHeight;
}

bool DisGaussianPuffRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalarArray(mOriginatingLocation, 3) && ValidateScalar(mSigmaX) &&
          ValidateScalar(mSigmaY) && ValidateScalar(mSigmaZ) && ValidateScalar(mDeltaSigmaX) &&
          ValidateScalar(mDeltaSigmaY) && ValidateScalar(mDeltaSigmaZ) && ValidateScalarArray(mVelocity, 3) &&
          ValidateScalarArray(mAngularVelocity, 3) && ValidateScalar(mCentroidHeight);
}

DisGaussianPuffRecord* DisGaussianPuffRecord::Clone() const
{
   return new DisGaussianPuffRecord(*this);
}

void DisGaussianPuffRecord::GetOriginatingLocation(DisFloat64& aXLocation, DisFloat64& aYLocation, DisFloat64& aZLocation) const
{
   aXLocation = mOriginatingLocation[0];
   aYLocation = mOriginatingLocation[1];
   aZLocation = mOriginatingLocation[2];
}

DisFloat32 DisGaussianPuffRecord::GetSigmaX() const
{
   return mSigmaX;
}

DisFloat32 DisGaussianPuffRecord::GetSigmaY() const
{
   return mSigmaY;
}

DisFloat32 DisGaussianPuffRecord::GetSigmaZ() const
{
   return mSigmaZ;
}

DisFloat32 DisGaussianPuffRecord::GetDeltaSigmaX() const
{
   return mDeltaSigmaX;
}

DisFloat32 DisGaussianPuffRecord::GetDeltaSigmaY() const
{
   return mDeltaSigmaY;
}

DisFloat32 DisGaussianPuffRecord::GetDeltaSigmaZ() const
{
   return mDeltaSigmaZ;
}

void DisGaussianPuffRecord::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

void DisGaussianPuffRecord::GetVelocity(DisFloat32& aVelocityX, DisFloat32& aVelocityY, DisFloat32& aVelocityZ) const
{
   aVelocityX = mVelocity[0];
   aVelocityY = mVelocity[1];
   aVelocityZ = mVelocity[2];
}

void DisGaussianPuffRecord::GetAngularVelocity(DisFloat32& aAngularVelocityX,
                                               DisFloat32& aAngularVelocityY,
                                               DisFloat32& aAngularVelocityZ) const
{
   aAngularVelocityX = mAngularVelocity[0];
   aAngularVelocityY = mAngularVelocity[1];
   aAngularVelocityZ = mAngularVelocity[2];
}

DisFloat32 DisGaussianPuffRecord::GetCentroidHeight() const
{
   return mCentroidHeight;
}

void DisGaussianPuffRecord::SetOriginatingLocation(DisFloat64 aXLocation, DisFloat64 aYLocation, DisFloat64 aZLocation)
{
   mOriginatingLocation[0] = aXLocation;
   mOriginatingLocation[1] = aYLocation;
   mOriginatingLocation[2] = aYLocation;
}

void DisGaussianPuffRecord::SetSigmaX(DisFloat32 aSigmaX)
{
   mSigmaX = aSigmaX;
}

void DisGaussianPuffRecord::SetSigmaY(DisFloat32 aSigmaY)
{
   mSigmaY = aSigmaY;
}

void DisGaussianPuffRecord::SetSigmaZ(DisFloat32 aSigmaZ)
{
   mSigmaZ = aSigmaZ;
}

void DisGaussianPuffRecord::SetDeltaSigmaX(DisFloat32 aDeltaSigmaX)
{
   mDeltaSigmaX = aDeltaSigmaX;
}

void DisGaussianPuffRecord::SetDeltaSigmaY(DisFloat32 aDeltaSigmaY)
{
   mDeltaSigmaY = aDeltaSigmaY;
}

void DisGaussianPuffRecord::SetDeltaSigmaZ(DisFloat32 aDeltaSigmaZ)
{
   mDeltaSigmaZ = aDeltaSigmaZ;
}

void DisGaussianPuffRecord::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisGaussianPuffRecord::SetVelocity(DisFloat32 aVelocityX, DisFloat32 aVelocityY, DisFloat32 aVelocityZ)
{
   mVelocity[0] = aVelocityX;
   mVelocity[1] = aVelocityY;
   mVelocity[2] = aVelocityZ;
}

void DisGaussianPuffRecord::SetAngularVelocity(DisFloat32 aAngularVelocityX,
                                               DisFloat32 aAngularVelocityY,
                                               DisFloat32 aAngularVelocityZ)
{
   mAngularVelocity[0] = aAngularVelocityX;
   mAngularVelocity[1] = aAngularVelocityY;
   mAngularVelocity[2] = aAngularVelocityZ;
}

void DisGaussianPuffRecord::SetCentroidHeight(DisFloat32 aCentroidHeight)
{
   mCentroidHeight = aCentroidHeight;
}

std::string DisGaussianPuffRecord::ToString() const
{
   std::string returnString = "-------Gaussian Puff Record-------\n";
   returnString += "NOTE: location data is the puff location equivalent\n";
   returnString += DisPointRecord::ToString() + '\n';
   returnString += "-------Originating Location-------\n";
   returnString += "Originating X: " + std::to_string(mOriginatingLocation[0]) + '\n';
   returnString += "Originating Y: " + std::to_string(mOriginatingLocation[1]) + '\n';
   returnString += "Originating Z: " + std::to_string(mOriginatingLocation[2]) + '\n';
   returnString += "-----End Originating Location-----\n";
   returnString += "Sigma X: " + std::to_string(mSigmaX) + '\n';
   returnString += "Sigma Y: " + std::to_string(mSigmaY) + '\n';
   returnString += "Sigma Z: " + std::to_string(mSigmaZ) + '\n';
   returnString += "Delta Sigma X: " + std::to_string(mDeltaSigmaX) + '\n';
   returnString += "Delta Sigma Y: " + std::to_string(mDeltaSigmaY) + '\n';
   returnString += "Delta Sigma Z: " + std::to_string(mDeltaSigmaZ) + '\n';
   returnString += "-------Orientation-------\n";
   returnString += "Orientation X: " + std::to_string(mOrientation[0]) + '\n';
   returnString += "Orientation Y: " + std::to_string(mOrientation[1]) + '\n';
   returnString += "Orientation Z: " + std::to_string(mOrientation[2]) + '\n';
   returnString += "-----End Orientation-----\n";
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
   returnString += "Centroid Height: " + std::to_string(mCentroidHeight) + '\n';
   returnString += "-----End Gaussian Puff Record-----\n";
   return returnString;
}

void DisGaussianPuffRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisGaussianPuffRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mOriginatingLocation[0] >> mOriginatingLocation[1] >> mOriginatingLocation[2];
   aGenI >> mSigmaX;
   aGenI >> mSigmaY;
   aGenI >> mSigmaZ;
   aGenI >> mDeltaSigmaX;
   aGenI >> mDeltaSigmaY;
   aGenI >> mDeltaSigmaZ;
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mVelocity[0] >> mVelocity[1] >> mVelocity[2];
   aGenI >> mAngularVelocity[0] >> mAngularVelocity[1] >> mAngularVelocity[2];
   aGenI >> mCentroidHeight;
}
