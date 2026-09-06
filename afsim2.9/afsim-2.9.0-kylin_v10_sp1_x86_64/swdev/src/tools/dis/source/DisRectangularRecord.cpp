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

#include "DisRectangularRecord.hpp"

#include "DisValidationUtils.hpp"

DisRectangularRecord::DisRectangularRecord()
   : DisPointRecord()
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = 0;
   }
}

DisRectangularRecord::DisRectangularRecord(const DisRectangularRecord& aSrc)
   : DisPointRecord(aSrc)
   , mLengthX(aSrc.mLengthX)
   , mLengthY(aSrc.mLengthY)
   , mLengthZ(aSrc.mLengthZ)
{
   for (int i = 0; i < 3; i++)
   {
      mOrientation[i] = aSrc.mOrientation[i];
   }
}

DisRectangularRecord::DisRectangularRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisRectangularRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisRectangularRecord::GetLength()
{
   return DisPointRecord::GetLength() + cMIN_LENGTH_OCTETS;
}

void DisRectangularRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mLengthX;
   aGenO << mLengthY;
   aGenO << mLengthZ;
   aGenO << mOrientation[0] << mOrientation[1] << mOrientation[2];
}

bool DisRectangularRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mLengthX) && ValidateScalar(mLengthY) && ValidateScalar(mLengthZ) &&
          ValidateScalar(mOrientation[0]) && ValidateScalar(mOrientation[1]) && ValidateScalar(mOrientation[2]);
}

DisRectangularRecord* DisRectangularRecord::Clone() const
{
   return new DisRectangularRecord(*this);
}

DisFloat32 DisRectangularRecord::GetLengthX() const
{
   return mLengthX;
}

DisFloat32 DisRectangularRecord::GetLengthY() const
{
   return mLengthY;
}

DisFloat32 DisRectangularRecord::GetLengthZ() const
{
   return mLengthZ;
}

void DisRectangularRecord::GetOrientation(DisFloat32& aPsi, DisFloat32& aTheta, DisFloat32& aPhi) const
{
   aPsi   = mOrientation[0];
   aTheta = mOrientation[1];
   aPhi   = mOrientation[2];
}

void DisRectangularRecord::SetLengthX(DisFloat32 aLengthX)
{
   mLengthX = aLengthX;
}

void DisRectangularRecord::SetLengthY(DisFloat32 aLengthY)
{
   mLengthY = aLengthY;
}

void DisRectangularRecord::SetLengthZ(DisFloat32 aLengthZ)
{
   mLengthZ = aLengthZ;
}

void DisRectangularRecord::SetOrientation(DisFloat32 aPsi, DisFloat32 aTheta, DisFloat32 aPhi)
{
   mOrientation[0] = aPsi;
   mOrientation[1] = aTheta;
   mOrientation[2] = aPhi;
}

void DisRectangularRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisRectangularRecord::ToString() const
{
   std::string returnString = "-------Rectangular Record-------\n";
   returnString += "Length X: " + std::to_string(mLengthX) + "\n";
   returnString += "Length Y: " + std::to_string(mLengthY) + "\n";
   returnString += "Length Z: " + std::to_string(mLengthZ) + "\n";
   returnString += "-------Orientation-------\n";
   returnString += "Orientation X: " + std::to_string(mOrientation[0]) + "\n";
   returnString += "Orientation Y: " + std::to_string(mOrientation[1]) + "\n";
   returnString += "Orientation Z: " + std::to_string(mOrientation[2]) + "\n";
   returnString += "-----End Orientation-----\n";
   return returnString;
}

void DisRectangularRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mLengthX;
   aGenI >> mLengthY;
   aGenI >> mLengthZ;
   aGenI >> mOrientation[0] >> mOrientation[1] >> mOrientation[2];
}
