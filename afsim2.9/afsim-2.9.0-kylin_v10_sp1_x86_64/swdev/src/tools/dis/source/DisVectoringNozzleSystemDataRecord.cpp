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

#include "DisVectoringNozzleSystemDataRecord.hpp"

#include "DisValidationUtils.hpp"

DisVectoringNozzleSystemDataRecord::DisVectoringNozzleSystemDataRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisVectoringNozzleSystemDataRecord::DisVectoringNozzleSystemDataRecord(const DisVectoringNozzleSystemDataRecord& aSrc)
   : mHorizontalDeflectionAngle(aSrc.mHorizontalDeflectionAngle)
   , mVerticalDeflectionAngle(aSrc.mVerticalDeflectionAngle)
{
}

DisVectoringNozzleSystemDataRecord* DisVectoringNozzleSystemDataRecord::Clone() const
{
   return new DisVectoringNozzleSystemDataRecord(*this);
}

DisFloat32 DisVectoringNozzleSystemDataRecord::GetHorizontalDeflectionAngle() const
{
   return mHorizontalDeflectionAngle;
}

DisFloat32 DisVectoringNozzleSystemDataRecord::GetVerticalDeflectionAngle() const
{
   return mVerticalDeflectionAngle;
}

void DisVectoringNozzleSystemDataRecord::SetHorizontalDeflectionAngle(DisFloat32 aHorizontalDeflectionAngleValue)
{
   mHorizontalDeflectionAngle = aHorizontalDeflectionAngleValue;
}

void DisVectoringNozzleSystemDataRecord::SetVerticalDeflectionAngle(DisFloat32 aVerticalDeflectionAngleValue)
{
   mVerticalDeflectionAngle = aVerticalDeflectionAngleValue;
}

void DisVectoringNozzleSystemDataRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisVectoringNozzleSystemDataRecord::Put(GenO& aGenO)
{
   aGenO << mHorizontalDeflectionAngle;
   aGenO << mVerticalDeflectionAngle;
}

void DisVectoringNozzleSystemDataRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mHorizontalDeflectionAngle;
   aGenI >> mVerticalDeflectionAngle;
}

DisUint16 DisVectoringNozzleSystemDataRecord::GetLength()
{
   return cLENGTH;
}

std::string DisVectoringNozzleSystemDataRecord::ToString() const
{
   std::string returnString = "   Horizontal Deflection Angle = " + std::to_string(mHorizontalDeflectionAngle) + "\n";
   returnString += "   Vertical Deflection Angle   = " + std::to_string(mVerticalDeflectionAngle) + "\n";
   return returnString;
}

void DisVectoringNozzleSystemDataRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

bool DisVectoringNozzleSystemDataRecord::IsValid() const
{
   return ValidateScalar(mHorizontalDeflectionAngle) && ValidateScalar(mVerticalDeflectionAngle);
}
