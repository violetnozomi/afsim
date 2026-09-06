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

#include "DisBoundingSphereRecord.hpp"

#include "DisTypes.hpp"
#include "DisValidationUtils.hpp"

DisBoundingSphereRecord::DisBoundingSphereRecord()
   : DisPointRecord()
{
}

DisBoundingSphereRecord::DisBoundingSphereRecord(const DisBoundingSphereRecord& aSrc)
   : DisPointRecord(aSrc)
   , mRadius(aSrc.mRadius)
{
}

DisBoundingSphereRecord::DisBoundingSphereRecord(GenI& aGenI)
   : DisPointRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisBoundingSphereRecord::Get(GenI& aGenI)
{
   DisPointRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisBoundingSphereRecord::GetLength()
{
   return DisPointRecord::GetLength() + cBASE_LENGTH_OCTETS;
}

void DisBoundingSphereRecord::Put(GenO& aGenO) const
{
   DisPointRecord::Put(aGenO);
   aGenO << mRadius;
   aGenO << mPadding;
}

bool DisBoundingSphereRecord::IsValid() const
{
   return DisPointRecord::IsValid() && ValidateScalar(mRadius);
}

DisBoundingSphereRecord* DisBoundingSphereRecord::Clone() const
{
   return new DisBoundingSphereRecord(*this);
}

DisFloat32 DisBoundingSphereRecord::GetRadius() const
{
   return mRadius;
}

void DisBoundingSphereRecord::SetRadius(DisFloat32 aRadius)
{
   mRadius = aRadius;
}

std::string DisBoundingSphereRecord::ToString() const
{
   std::string returnString = "-------Bounding Sphere Record-------\n";
   returnString += DisPointRecord::ToString();
   returnString += "Radius: " + std::to_string(mRadius) + '\n';
   returnString += "-----End Bounding Sphere Record-----\n";
   return returnString;
}

void DisBoundingSphereRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisBoundingSphereRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mRadius;
   aGenI >> mPadding;
}
