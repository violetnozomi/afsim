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

#include "DisConeRecord.hpp"

#include "DisValidationUtils.hpp"

DisConeRecord::DisConeRecord()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = 0;
   }
}

DisConeRecord::DisConeRecord(const DisConeRecord& aSrc)
   : DisPointRecord(aSrc)
   , mHeight(aSrc.mHeight)
   , mPeakAngle(aSrc.mPeakAngle)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = aSrc.mOrientation[i];
   }
}

DisConeRecord::DisConeRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisConeRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisConeRecord::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisConeRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
   aGenO << mHeight;
   aGenO << mPeakAngle;
   aGenO << mPadding;
}

bool DisConeRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalarArray(mOrientation, 3) && ValidateScalar(mHeight) &&
          ValidateScalar(mPeakAngle);
}

DisConeRecord* DisConeRecord::Clone() const
{
   return new DisConeRecord(*this);
}

void DisConeRecord::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

DisFloat32 DisConeRecord::GetHeight() const
{
   return mHeight;
}

DisFloat32 DisConeRecord::GetPeakAngle() const
{
   return mPeakAngle;
}

void DisConeRecord::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisConeRecord::SetHeight(DisFloat32 aHeight)
{
   mHeight = aHeight;
}

void DisConeRecord::SetPeakAngle(DisFloat32 aPeakAngle)
{
   mPeakAngle = aPeakAngle;
}

void DisConeRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisConeRecord::ToString() const
{
   std::string returnString = "";
   returnString += "-------Cone Record-------\n";
   returnString += "-------Orientation Record-------\n";
   returnString += "Orientation X: " + std::to_string(mOrientation[0]) + '\n';
   returnString += "Orientation X: " + std::to_string(mOrientation[1]) + '\n';
   returnString += "Orientation X: " + std::to_string(mOrientation[2]) + '\n';
   returnString += "-----End Orientation Record-----\n";
   returnString += "Height: " + std::to_string(mHeight) + '\n';
   returnString += "Peak Angle: " + std::to_string(mPeakAngle) + '\n';
   returnString += "-----End Cone Record-----\n";
   return returnString;
}

void DisConeRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
   aGenI >> mHeight;
   aGenI >> mPeakAngle;
   aGenI >> mPadding;
}
