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

#include "DisUniformGeometryRecord.hpp"

#include "DisValidationUtils.hpp"

DisUniformGeometryRecord::DisUniformGeometryRecord(const DisUniformGeometryRecord& aSrc)
   : mField(aSrc.mField)
{
}

DisUniformGeometryRecord::DisUniformGeometryRecord(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisUniformGeometryRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisUniformGeometryRecord::GetLength()
{
   return cMIN_LENGTH_OCTETS;
}

void DisUniformGeometryRecord::Put(GenO& aGenO) const
{
   aGenO << mField;
   aGenO << mPadding1 << mPadding2 << mPadding3;
}

bool DisUniformGeometryRecord::IsValid() const
{
   return true;
}

DisUniformGeometryRecord* DisUniformGeometryRecord::Clone() const
{
   return new DisUniformGeometryRecord(*this);
}

DisUint8 DisUniformGeometryRecord::GetField()
{
   return mField;
}

void DisUniformGeometryRecord::SetField(DisUint8 aField)
{
   mField = aField;
}

std::string DisUniformGeometryRecord::ToString() const
{
   std::string returnString = "-------Uniform Geometry Record-------\n";
   returnString += "Field: " + std::to_string(mField) + "\n";
   returnString += "-----End Uniform Geometry Record-----\n";
   return returnString;
}

void DisUniformGeometryRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

void DisUniformGeometryRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mField;
   aGenI >> mPadding1 >> mPadding2 >> mPadding3;
}
