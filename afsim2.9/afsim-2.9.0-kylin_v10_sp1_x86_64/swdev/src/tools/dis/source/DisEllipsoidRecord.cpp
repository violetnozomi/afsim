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

#include "DisEllipsoidRecord.hpp"

#include "DisValidationUtils.hpp"

DisEllipsoidRecord::DisEllipsoidRecord()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = 0;
   }
}

DisEllipsoidRecord::DisEllipsoidRecord(const DisEllipsoidRecord& aSrc)
   : DisPointRecord(aSrc)
   , mSigmaX(aSrc.mSigmaX)
   , mSigmaY(aSrc.mSigmaY)
   , mSigmaZ(aSrc.mSigmaZ)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = aSrc.mOrientation[i];
   }
}

DisEllipsoidRecord::DisEllipsoidRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisEllipsoidRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisEllipsoidRecord::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisEllipsoidRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mSigmaX;
   aGenO << mSigmaY;
   aGenO << mSigmaZ;
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
}

bool DisEllipsoidRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mSigmaX) && ValidateScalar(mSigmaY) && ValidateScalar(mSigmaZ) &&
          ValidateScalarArray(mOrientation, 3);
}

DisEllipsoidRecord* DisEllipsoidRecord::Clone() const
{
   return new DisEllipsoidRecord(*this);
}

DisFloat32 DisEllipsoidRecord::GetSigmaX() const
{
   return mSigmaX;
}

DisFloat32 DisEllipsoidRecord::GetSigmaY() const
{
   return mSigmaY;
}

DisFloat32 DisEllipsoidRecord::GetSigmaZ() const
{
   return mSigmaZ;
}

void DisEllipsoidRecord::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

void DisEllipsoidRecord::SetSigmaX(DisFloat32 aSigmaX)
{
   mSigmaX = aSigmaX;
}

void DisEllipsoidRecord::SetSigmaY(DisFloat32 aSigmaY)
{
   mSigmaY = aSigmaY;
}

void DisEllipsoidRecord::SetSigmaZ(DisFloat32 aSigmaZ)
{
   mSigmaZ = aSigmaZ;
}

void DisEllipsoidRecord::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisEllipsoidRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisEllipsoidRecord::ToString() const
{
   std::string returnString = "-------Ellipsoid Record-------\n";
   returnString += DisPointRecord::ToString() + '\n';
   returnString += "Sigma X: " + std::to_string(mSigmaX) + '\n';
   returnString += "Sigma Y: " + std::to_string(mSigmaY) + '\n';
   returnString += "Sigma Z: " + std::to_string(mSigmaZ) + '\n';
   returnString += "-------Orientation-------\n";
   returnString += "Psi: " + std::to_string(mOrientation[0]) + '\n';
   returnString += "Theta: " + std::to_string(mOrientation[1]) + '\n';
   returnString += "Phi: " + std::to_string(mOrientation[2]) + '\n';
   returnString += "-----End Orientation-----\n";
   returnString += "-----End Ellipsoid Record-----\n";
   return returnString;
}

void DisEllipsoidRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSigmaX;
   aGenI >> mSigmaY;
   aGenI >> mSigmaZ;
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
}
