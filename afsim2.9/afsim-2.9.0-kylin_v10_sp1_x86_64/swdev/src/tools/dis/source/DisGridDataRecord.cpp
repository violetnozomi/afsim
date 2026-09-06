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

#include "DisGridDataRecord.hpp"

#include "DisGridDataRepresentationTypeOne.hpp"
#include "DisGridDataRepresentationTypeTwo.hpp"
#include "DisGridDataRepresentationTypeZero.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGridDataRecord::DisGridDataRecord()
   : mGridDataRepresentationType(nullptr)
{
}

DisGridDataRecord::DisGridDataRecord(const DisGridDataRecord& aSrc)
   : mGridDataRepresentationType(aSrc.mGridDataRepresentationType)
{
}

DisGridDataRecord::DisGridDataRecord(const DisGridDataRecord& aSrc, GenI& aGenI)
   : mGridDataRepresentationType(aSrc.mGridDataRepresentationType)
{
   GetMemberData(aGenI);
}

DisGridDataRecord::DisGridDataRecord(GenI& aGenI)
   : mGridDataRepresentationType(nullptr)
{
   GetMemberData(aGenI);
}

void DisGridDataRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisGridDataRecord::GetLength()
{
   DisUint16 length = cBASE_LENGTH_IN_OCTETS;
   if (mGridDataRepresentationType != nullptr)
   {
      length += mGridDataRepresentationType->GetLength();
   }
   return length;
}

void DisGridDataRecord::Put(GenO& aGenO) const
{
   aGenO << mSampleType;
   aGenO << mDataRepresentationType;
   if (mGridDataRepresentationType != nullptr)
   {
      mGridDataRepresentationType->Put(aGenO);
   }
}

DisEnum16 DisGridDataRecord::GetSampleType() const
{
   return mSampleType;
}

DisEnum16 DisGridDataRecord::GetDataRepresentationType() const
{
   return mDataRepresentationType;
}

const DisGridDataRepresentationType* DisGridDataRecord::GetGridDataRepresentationType() const
{
   return mGridDataRepresentationType;
}

void DisGridDataRecord::SetGridDataRepresentationType(DisGridDataRepresentationType* aGridDataObject)
{
   mGridDataRepresentationType = aGridDataObject;
}

void DisGridDataRecord::SetSampleType(DisEnum16 aSampleType)
{
   mSampleType = aSampleType;
}

void DisGridDataRecord::SetDataRepresentationType(DisEnum16 aDataRepresentationType)
{
   mDataRepresentationType = aDataRepresentationType;
}

bool DisGridDataRecord::IsValid() const
{
   return true;
}

DisGridDataRecord* DisGridDataRecord::Clone() const
{
   return new DisGridDataRecord(*this);
}

void DisGridDataRecord::Stream(std::ostream& aStream) const
{
   aStream << ToString() << "\n";
}

std::string DisGridDataRecord::ToString() const
{
   std::string returnString = "Grid Data Record\n";
   returnString += "Sample Type:              " + std::to_string(mSampleType);
   returnString += "Data Representation Type: " + std::to_string(mDataRepresentationType);
   returnString += mGridDataRepresentationType->ToString();
   returnString += "End Grid Data Record\n";
   return returnString;
}

void DisGridDataRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mSampleType;
   aGenI >> mDataRepresentationType;
   GetRecord(aGenI);
}

void DisGridDataRecord::GetRecord(GenI& aGenI)
{
   switch (mDataRepresentationType)
   {
   case 0:
   {
      mGridDataRepresentationType = new DisGridDataRepresentationTypeZero(aGenI);
      break;
   }
   case 1:
   {
      mGridDataRepresentationType = new DisGridDataRepresentationTypeOne(aGenI);
      break;
   }
   case 2:
   {
      mGridDataRepresentationType = new DisGridDataRepresentationTypeTwo(aGenI);
      break;
   }
   default:
   {
   }
   }
}
